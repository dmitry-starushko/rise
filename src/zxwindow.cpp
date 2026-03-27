/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *  ____  ___ ____  _____
 * |  _ \|_ _/ ___|| ____|
 * | |_) || |\___ \|  _|
 * |  _ < | | ___) | |___
 * |_| \_\___|____/|_____|
 *
 * Copyright (C) 2026 Dmitry Starushko <dmitry.starushko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include "zxwindow.h"
#include "zxdefs.h"
#include "zxutils.h"
#include "ui_zxwindow.h"
#include <thread>
#include <ranges>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <QKeyEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QGuiApplication>
#include <QClipboard>

ZxWindow::ZxWindow(QWidget *parent)
    : QMainWindow(parent),
      ui{std::make_unique<Ui::ZxWindow>()},

/**INDENT-OFF**/
      _loaders {
          { "Tape image", {
                  { ".tzx", [this] INLINE (auto data) noexcept { return _zxs.tape.open_tzx(std::move(data)); }},
                  { ".tap", [this] INLINE (auto data) noexcept { return _zxs.tape.open_tap(std::move(data)); }},}},
          { "Snapshot",   {
                  { ".z80", [this] INLINE (auto data) noexcept { return _zxs.load_from_z80(std::move(data)); }},
                  { ".mgc", [this] INLINE (auto data) noexcept { return _zxs.load_from_mgc(std::move(data)); }},
                  { ".sna", [this] INLINE (auto data) noexcept { return _zxs.load_from_sna(std::move(data)); }},}},
          { "Firmware",   {
                  { ".rom", [this] INLINE (auto data) noexcept { return _zxs.load_firmware(std::move(data)); }},
                  { ".bin", [this] INLINE (auto data) noexcept { return _zxs.load_firmware(std::move(data)); }},}},
      },
      _exporters {
          { "Snapshot",   {
                  { ".z80", [this] noexcept { return _zxs.z80_snapshot(); }},}},
      }
/**INDENT-ON**/

{
    ui->setupUi(this);
    ui->toolBar->setFloatable(false);
    ui->toolBar->setMovable(false);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->toolBar->setStyleSheet("QToolButton { width: 54px; }");
    ui->centralwidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->screen_view->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    _status_joystick->setStyleSheet(
                "QLabel {"
                "  font-size: 8pt;"
                "  letter-spacing: -0.25px;"
                "  border: 1px solid #808080;"
                "  border-radius: 2px;"
                "  padding: 2px;"
                "  margin-bottom: 8px;"
                "  margin-left: 8px;"
                "}");
    _status_gamepad->setStyleSheet(
                "QLabel {"
                "  font-size: 8pt;"
                "  letter-spacing: -0.25px;"
                "  border: 1px solid #808080;"
                "  border-radius: 2px;"
                "  padding: 2px;"
                "  margin-bottom: 8px;"
                "}");
    _status_mouse->setStyleSheet(
                "QLabel {"
                "  font-size: 8pt;"
                "  letter-spacing: -0.25px;"
                "  border: 1px solid #808080;"
                "  border-radius: 2px;"
                "  padding: 2px;"
                "  margin-bottom: 8px;"
                "}");
    _status_perf->setStyleSheet(
                "QLabel {"
                "  font-size: 8pt;"
                "  letter-spacing: -0.25px;"
                "  border: 1px solid #808080;"
                "  border-radius: 2px;"
                "  padding: 2px;"
                "  margin-bottom: 8px;"
                "}");
    _status_gamepad->setText("Gamepad: None");

    ui->statusbar->addWidget(_status_joystick);
    ui->statusbar->addWidget(_status_gamepad);
    ui->statusbar->addWidget(_status_mouse);
    ui->statusbar->addWidget(_status_perf);

    _cpanel->hide();

    setFixedSize(minimumSizeHint());
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setAcceptDrops(true);

    initAudio(_devices->defaultAudioOutput());
    initArtifacts();
    initLookups();

    connect(&_timer, &QChronoTimer::timeout, this, // TODO: move this routine to ZxS!
    [this, _ticks_ctr = 0, _frame_ticks = 0, _blink_ctr = 0.0] mutable noexcept {
        using namespace std::chrono;
        constexpr auto overload_time_us = zx::rate::frame_duration_us * 5 / 6;

        auto frame_stage = [this, &_ticks_ctr, &_frame_ticks] INLINE (int duration) noexcept {
            _ticks_ctr += duration;

            for (int ticks; _ticks_ctr > 0; _ticks_ctr -= ticks) {
                _frame_ticks += (ticks = _zxs.z80.exec());
                _zxs.ports.frame_ticks(_frame_ticks %= zx::ticks::per_frame);
            }
        };

        auto start = steady_clock::now();
        bool overload = false;

        for (int pf = _perf_factor; pf; --pf) {
            frame_stage(zx::ticks::per_vblank);
            int line = 0;

            for (; line < zx::lines::border_top; ++line) {
                event::dispatch(event::data<EVENT::ULA_BORDER_LINE_START> {.line = line});
                frame_stage(zx::ticks::per_line);
            }

            for (; line < zx::lines::border_top + zx::lines::bitmap; ++line) {
                event::dispatch(event::data<EVENT::ULA_BORDER_LEDGE_START> {.line = line});
                frame_stage(zx::ticks::border_side);
                event::dispatch(event::data<EVENT::ULA_BITMAP_ROW_START> {.line = line});
                frame_stage(zx::ticks::bitmap_line);
                event::dispatch(event::data<EVENT::ULA_BORDER_REDGE_START> {.line = line});
                frame_stage(zx::ticks::border_side);
                event::dispatch(event::data<EVENT::ULA_HBLANK_START> {.line = line});
                frame_stage(zx::ticks::hblank);
            }

            for (; line < zx::lines::border_top + zx::lines::bitmap + zx::lines::border_bottom; ++line) {
                event::dispatch(event::data<EVENT::ULA_BORDER_LINE_START> {.line = line});
                frame_stage(zx::ticks::per_line);
            }

            _zxs.z80.handle_int(0xFF_u8);
            event::dispatch(event::data<EVENT::ULA_VBLANK_START> { });

            if ((overload = (duration_cast<nanoseconds>(steady_clock::now() - start).count() > overload_time_us))) {
                break;
            }
        }

        std::thread screen_thread([this, &_blink_ctr] INLINE noexcept {
            _zxs.ula.translate_screen((_blink_ctr = fmod(_blink_ctr + 1.0, zx::rate::frame_rate)) > 0.5 * zx::rate::frame_rate,
            [this] (int x, int y, const zx::color::rgb_t & c) noexcept {
                _image.setPixelColor(x, y, QColor(std::get<0>(c), std::get<1>(c), std::get<2>(c)));
            },
            [this] (int x0, int y0, int x1, int y1, const zx::color::rgb_t & c) noexcept {
                QPen pen(QColor(std::get<0>(c), std::get<1>(c), std::get<2>(c)));
                pen.setWidth(0);
                _painter.setPen(pen);
                _painter.drawLine(x0, y0, x1, y1);
            });
            ui->screen_view->setPixmap(QPixmap::fromImage(_image).scaled(640, 480, Qt::KeepAspectRatio, Qt::FastTransformation));
        });

        event::dispatch(event::data<EVENT::SYS_PERIODIC>{.perf_factor = _perf_factor, .overload = overload});

        if (_perf_factor == 1u) {
            std::thread audio_thread([this] INLINE noexcept {
                if (_audio_io && _audio_sink->state() != QAudio::StoppedState) {
                    size_t len = _audio_sink->bytesFree();
                    QByteArray buffer(len, 0);

                    if ((len = _zxs.beeper.fill_buffer(reinterpret_cast<uint8_t *>(buffer.data()), len))) {
                        _audio_io->write(buffer.data(), len);
                    }
                }
            });

            audio_thread.join();
        }

        screen_thread.join();
    });

    _timer.setTimerType(Qt::TimerType::PreciseTimer);
    _timer.setInterval(std::chrono::nanoseconds(llround(zx::rate::frame_duration_us)));
    _timer.start();
}

ZxWindow::~ZxWindow() {
    if (_audio_sink->state() != QAudio::StoppedState) {
        _audio_sink->stop();
    }
}

void ZxWindow::initLookups() noexcept {
    namespace vw = std::views;
    namespace rg = std::ranges;
    namespace ba = boost::algorithm;

    auto prepare = [] (const auto & kindmap, auto & lookup) noexcept {
        auto asterisk = [] (auto s) noexcept {
            return std::format("*{}", s);
        };
        std::list<std::string> str_list;

        lookup = kindmap | vw::values | vw::join | rg::to<std::map>();
        str_list.push_back(std::format("Any supported file ({})", ba::join(lookup | vw::keys | vw::transform(asterisk) | rg::to<std::vector>(), " ")));

        for (const auto & group : kindmap | vw::keys) {
            str_list.push_back(std::format("{} ({})", group, ba::join(kindmap.at(group) | vw::keys | vw::transform(asterisk) | rg::to<std::vector>(), " ")));
        }

        return QString::fromStdString(ba::join(str_list, ";;"));
    };

    _open_selector = prepare(_loaders, _loader_lookup);
    _save_selector = prepare(_exporters, _exporter_lookup);
}

void ZxWindow::initAudio(const QAudioDevice & deviceInfo) noexcept {
    QAudioFormat format;

    format.setSampleFormat(QAudioFormat::SampleFormat::Float);
    format.setSampleRate(ZxBeeper::sample_rate());
    format.setChannelCount(ZxBeeper::channels());

    if (_audio_sink = std::make_unique<QAudioSink>(deviceInfo, format); !(_audio_io = _audio_sink->start())) {
        QMessageBox::warning(this, "Audio start failed", "Device rejected the format or is unavailable.");
    }
}

void ZxWindow::initArtifacts() noexcept {
    _joysticks = _zxs.bridge.available_joyticks();
    _curr_joystick = std::find(_joysticks.begin(), _joysticks.end(), _zxs.bridge.current_virtual_joystick()) - _joysticks.begin();
    _mouses = _zxs.bridge.available_mouses();
    _curr_mouse = std::find(_mouses.begin(), _mouses.end(), _zxs.bridge.current_mouse()) - _mouses.begin();
    updateStatus();
}

void ZxWindow::updateStatus() noexcept {
    _status_joystick->setText(
                QString::fromStdString(
                            std::format("Virt. joystick {}/{}: {}",
                                        _curr_joystick + 1,
                                        _joysticks.size(),
                                        _zxs.bridge.current_virtual_joystick())));
    _status_mouse->setText(
                QString::fromStdString(
                            std::format("Mouse {}/{}: {}",
                                        _curr_mouse + 1,
                                        _mouses.size(),
                                        _zxs.bridge.current_mouse())));
    _status_perf->setText(
                QString::fromStdString(
                            std::format("Perf: ×{}", _perf_factor)));
}

void ZxWindow::reset() noexcept {
    with_confirm_dialog([this] {
        _zxs.z80.reset();
    }, "Are you sure you want to reset?");
}

void ZxWindow::toggleControlPanel() noexcept {
    if (_cpanel->isVisible()) {
        _cpanel->hide();
    } else {
        _cpanel->show();
        _cpanel->raise();
        _cpanel->activateWindow();
    }
}

void ZxWindow::keyPressEvent(QKeyEvent * event) noexcept {
    if (!event->isAutoRepeat()) {
        _zxs.bridge.key_down(event->nativeScanCode());
    }

    inherited::keyPressEvent(event);
}

void ZxWindow::keyReleaseEvent(QKeyEvent * event) noexcept {
    if (!event->isAutoRepeat()) {
        _zxs.bridge.key_up(event->nativeScanCode());
    }

    inherited::keyReleaseEvent(event);
}

bool ZxWindow::event(QEvent * event) noexcept {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Tab) {
            nextJoystick();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Escape) {
            release_mouse();
        }
    }

    return inherited::event(event);
}

void ZxWindow::capture_mouse() noexcept {
    if (!_zxs.bridge.mouse_is_null_object() && !_mouse_captured) {
        setCursor(Qt::BlankCursor);
        QCursor::setPos(ui->screen_view->mapToGlobal(ui->screen_view->rect().center()));
        _mouse_x = zx::screen::disp_width / 2.0;
        _mouse_y = zx::screen::disp_height / 2.0;
        _mouse_captured = true;
    }
}

void ZxWindow::release_mouse() noexcept {
    if (_mouse_captured) {
        unsetCursor();
        _mouse_captured = false;
    }
}

template<typename ... Args>
INLINE
inline auto ZxWindow::with_mouse_event(auto f, QMouseEvent * event, Args &&...args) noexcept {
    constexpr double factor = double(zx::screen::pixels_h) / zx::screen::disp_width;
    const auto center = ui->screen_view->rect().center();
    event->accept();

    if (_mouse_captured) {
        auto [dx, dy] = factor * (ui->screen_view->mapFromGlobal(event->globalPosition()) - center);
        auto buttons = event->buttons();

        QCursor::setPos(ui->screen_view->mapToGlobal(ui->screen_view->rect().center()));
        return f(static_cast<uint8_t>(std::round(_mouse_x += dx)),
                 static_cast<uint8_t>(std::round(_mouse_y += dy)),
                 buttons & Qt::LeftButton,
                 buttons & Qt::MiddleButton,
                 buttons & Qt::RightButton,
                 std::forward<Args>(args)...);
    }
}

void ZxWindow::mousePressEvent(QMouseEvent * event) noexcept {
    capture_mouse();
    with_mouse_event([this] (auto x, auto y, auto left, auto middle, auto right) {
        _zxs.bridge.update_mouse(x, y, left, middle, right);
    }, event);
}

void ZxWindow::mouseReleaseEvent(QMouseEvent * event) noexcept {
    with_mouse_event([this] (auto x, auto y, auto left, auto middle, auto right) {
        _zxs.bridge.update_mouse(x, y, left, middle, right);
    }, event);
}

void ZxWindow::mouseMoveEvent(QMouseEvent * event) noexcept {
    with_mouse_event([this] (auto x, auto y, auto left, auto middle, auto right) {
        _zxs.bridge.update_mouse(x, y, left, middle, right);
    }, event);
}

void ZxWindow::dragEnterEvent(QDragEnterEvent *event) noexcept {
    if (const QMimeData &mimeData = *event->mimeData(); mimeData.hasUrls()) {
        for (const QUrl &url : mimeData.urls()) {
            if (url.isLocalFile()) {
                event->acceptProposedAction();
            }
        }
    }
}

void ZxWindow::dropEvent(QDropEvent *event) noexcept {
    if (const QMimeData &mimeData = *event->mimeData(); mimeData.hasUrls()) {
        for (const QUrl &url : mimeData.urls()) {
            if (url.isLocalFile()) {
                event->acceptProposedAction();
                with_emulation_suspended([this, &url] INLINE noexcept {
                    if (auto result = load_file(url.toLocalFile().toStdString()); !result) {
                        QMessageBox::critical(this, "Error", QString::fromStdString(result.error()));
                    }
                });
            }
        }
    }
}

void ZxWindow::copyScreenshot() noexcept {
    with_info_dialog([this] {
        QPixmap pixmap = ui->screen_view->pixmap();

        if (!pixmap.isNull()) {
            QGuiApplication::clipboard()->setPixmap(pixmap);
            return true;
        }

        return false;
    }, "The screenshot has been copied to the clipboard.");
}

void ZxWindow::nextJoystick() noexcept {
    if (!_joysticks.empty()) {
        if (auto next = (_curr_joystick + 1) % _joysticks.size(); _zxs.bridge.select_virtual_joystick(_joysticks[next])) {
            _curr_joystick = next;
            updateStatus();
        }
    }
}

void ZxWindow::nextMouse() noexcept {
    if (!_mouses.empty()) {
        if (auto next = (_curr_mouse + 1) % _mouses.size(); _zxs.bridge.select_mouse(_mouses[next])) {
            _curr_mouse = next;
            updateStatus();
        }
    }
}

void ZxWindow::pause(bool flag) noexcept {
    if (flag) {
        suspend();
    } else {
        resume();
    }
}

std::expected<bool, std::string> ZxWindow::load_file(const std::filesystem::path &path) noexcept {
    if (std::filesystem::is_regular_file(path)) {
        if (auto bytes = zxutils::read_file(path); bytes) {
            std::string ext = boost::to_lower_copy(path.extension().string());

            if (auto loader = _loader_lookup.find(ext); loader != _loader_lookup.end()) {
                if (auto result = loader->second(std::move(*bytes)); result) {
                    return true;
                } else {
                    return std::unexpected(result.error());
                }
            } else {
                return std::unexpected(std::format("Unsupported extension: {}", ext));
            }
        } else {
            return std::unexpected(bytes.error());
        }
    } else {
        return std::unexpected(std::format("File '{}' isn't regular file", path.generic_string()));
    }
}

std::expected<bool, std::string> ZxWindow::export_file(const std::filesystem::path &path) noexcept {
    std::string ext = boost::to_lower_copy(path.extension().string());

    if (auto exporter = _exporter_lookup.find(ext); exporter != _exporter_lookup.end()) {
        if (auto bytes = exporter->second(); bytes) {
            if (auto result = zxutils::save_file(path, std::span(bytes->begin(), bytes->end())); result) {
                return true;
            } else {
                return std::unexpected(result.error());
            }
        } else {
            return std::unexpected(bytes.error());
        }
    } else {
        return std::unexpected(std::format("Unsupported extension: '{}'", ext));
    }
}

void ZxWindow::openFile() noexcept {
    with_emulation_suspended([this] noexcept {
        if (auto file_name = QFileDialog::getOpenFileName(this, "Open", QDir::homePath(), _open_selector); !file_name.isEmpty()) {
            if (auto result = load_file(file_name.toStdString()); !result) {
                QMessageBox::critical(this, "Error", QString::fromStdString(result.error()));
            }
        }
    });
}

void ZxWindow::saveFile() noexcept {
    with_emulation_suspended([this] noexcept {
        if (auto file_name = QFileDialog::getSaveFileName(this, "Save", QDir::homePath(), _save_selector); !file_name.isEmpty()) {
            if (auto result = export_file(file_name.toStdString()); !result) {
                QMessageBox::critical(this, "Error", QString::fromStdString(result.error()));
            }
        }
    });
}

void ZxWindow::handle(const event::data<EVENT::SYS_SHOW_STATUS> &ed) noexcept {
    QTimer::singleShot(0, this, [this, text = QString::fromStdString(std::string(ed.text)), time = ed.show_time] {
        ui->statusbar->showMessage(text, std::clamp(time, 1u, 60u) * 1000);
    });
}
