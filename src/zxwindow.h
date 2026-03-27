#pragma once

#ifndef ZXWINDOW_H
#define ZXWINDOW_H

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

#include "zxs.h"
#include "zxcontrolpanel.h"
#include "sdlgamepad.h"
#include <memory>
#include <filesystem>
#include <QMainWindow>
#include <QPainter>
#include <QPointer>
#include <QChronoTimer>
#include <QMediaDevices>
#include <QAudioSink>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class ZxWindow;
}
QT_END_NAMESPACE

class ZxWindow:
    public QMainWindow,
    public event::handler<ZxWindow, EVENT::SYS_SHOW_MESSAGE, EVENT::SYS_SHOW_STATUS, EVENT::SYS_SELECT_OPTION, EVENT::SYS_CONFIRM_YES_NO, EVENT::SYS_RESTORE_ROM, EVENT::SYS_SET_PERFORMANCE_FACTOR, EVENT::GPAD_CONNECTED, EVENT::GPAD_DISCONNECTED> {
    Q_OBJECT

  public:

    explicit ZxWindow(QWidget *parent = nullptr);
    ~ZxWindow() override;

    QMenu *createPopupMenu() override {
        return nullptr;
    };

  private slots:

    void openFile() noexcept;
    void saveFile() noexcept;
    void copyScreenshot() noexcept;
    void nextJoystick() noexcept;
    void nextMouse() noexcept;
    void pause(bool flag) noexcept;
    void reset() noexcept;
    void toggleControlPanel() noexcept;

  private:

    void initLookups() noexcept;
    void initAudio(const QAudioDevice &deviceInfo) noexcept;
    void initArtifacts() noexcept;
    void updateStatus() noexcept;

    INLINE
    void suspend() noexcept {
        if (!_suspend_counter++) {
            _timer.stop();
            _zxs.beeper.reset();
        }
    }

    INLINE
    void resume() noexcept {
        if (_suspend_counter && !--_suspend_counter) {
            _timer.start();
            setFocus();
        }
    }

    template<typename ... Args>
    INLINE
    auto with_emulation_suspended(auto f, Args && ...args) noexcept {
        struct suspend_guard_t {
            ZxWindow &_self;

            explicit suspend_guard_t(ZxWindow &self): _self{self} {
                _self.suspend();
            }

            ~suspend_guard_t() {
                _self.resume();
            }
        }
        suspend_guard(*this);
        return f(std::forward<Args>(args)...);
    }

    template<typename ... Args>
    INLINE
    auto with_confirm_dialog(auto f, Args && ...args, const std::string &message = "Are you sure?") noexcept {
        with_emulation_suspended([this, &f, args..., &message] {
            if (QMessageBox::question(this, "Confirm", QString::fromStdString(message), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                return f(std::forward<Args>(args)...);
            }
            return decltype(f(std::forward<Args>(args)...)) {};
        });
    }

    template<typename ... Args>
    INLINE
    void with_info_dialog(auto f, Args && ...args, const std::string &message_ok = "Successfully!", const std::string &message_fail = "Failed") noexcept {
        with_emulation_suspended([this, &f, args..., &message_ok, &message_fail] {
            auto result = static_cast<bool>(f(std::forward<Args>(args)...));
            QMessageBox::information(this, "Information", QString::fromStdString(result ? message_ok : message_fail), QMessageBox::Ok) ;
        });
    }

    template<typename ... Args>
    INLINE
    auto with_mouse_event(auto f, QMouseEvent *event, Args && ...args) noexcept;

    std::expected<bool, std::string> load_file(const std::filesystem::path &path) noexcept;
    std::expected<bool, std::string> export_file(const std::filesystem::path &path) noexcept;
    void capture_mouse() noexcept;
    void release_mouse() noexcept;

  protected:

    void keyPressEvent(QKeyEvent * event) noexcept override;
    void keyReleaseEvent(QKeyEvent * event) noexcept override;
    bool event(QEvent * event) noexcept override;

    void mousePressEvent(QMouseEvent *event) noexcept override;
    void mouseReleaseEvent(QMouseEvent *event) noexcept override;
    void mouseMoveEvent(QMouseEvent *event) noexcept override;

    void dragEnterEvent(QDragEnterEvent *event) noexcept override;
    void dropEvent(QDropEvent *event) noexcept override;


  public:

    INLINE
    void handle(const event::data<EVENT::SYS_SHOW_MESSAGE> &ed) noexcept {
        with_emulation_suspended([this, &ed] {
            switch (ed.type) {
                case MSG_KIND::INFO:
                    QMessageBox::information(this, "Information", QString::fromStdString(std::string(ed.message)), QMessageBox::Ok) ;
                    break;

                case MSG_KIND::WARN:
                    QMessageBox::warning(this, "Warning", QString::fromStdString(std::string(ed.message)), QMessageBox::Ok) ;
                    break;

                case MSG_KIND::ERROR:
                    QMessageBox::critical(this, "Error", QString::fromStdString(std::string(ed.message)), QMessageBox::Ok) ;
                    break;
            }
        });
    }

    INLINE
    void handle(event::data<EVENT::SYS_SELECT_OPTION> &ed) noexcept {
        if (ed.choice < 0) {
            with_emulation_suspended([this, &ed] {
                QStringList options;
                bool ok;

                for (const auto &item : ed.items) {
                    options.append(QString::fromStdString(std::string(item)));
                }

                QString selected = QInputDialog::getItem(this,
                    QString::fromStdString(std::string(ed.title)),
                    QString::fromStdString(std::string(ed.hint)),
                    options, 0, false, &ok, Qt::WindowStaysOnTopHint | Qt::Dialog);

                ed.choice = ok ? options.indexOf(selected) : -1;
            });
        }
    }

    INLINE
    void handle(event::data<EVENT::SYS_CONFIRM_YES_NO> &ed) noexcept {
        if (ed.choice < 0) {
            with_emulation_suspended([this, &ed] {
                ed.choice = QMessageBox::question(this,
                                                  QString::fromStdString(std::string(ed.title)),
                                                  QString::fromStdString(std::string(ed.question)),
                                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
            });
        }
    }

    INLINE
    void handle(const event::data<EVENT::SYS_RESTORE_ROM> &) noexcept {
        with_info_dialog([this] {
            return _zxs.restore_ROM();
        }, "Restored original ROM.");
    }

    INLINE
    void handle(const event::data<EVENT::SYS_SET_PERFORMANCE_FACTOR> &ed) noexcept {
        if (_perf_factor != ed.perf_factor) {
            _perf_factor = std::clamp(ed.perf_factor, 1u, 10u);
            _zxs.beeper.reset();
            updateStatus();
        }
    }

    INLINE
    void handle(const event::data<EVENT::GPAD_CONNECTED> &ed) noexcept {
        QTimer::singleShot(0, this, [this, text = QString::fromStdString(std::format("Gamepad: {}", ed.gamepad_name))] {
            _status_gamepad->setText(text);
        });
    }

    INLINE
    void handle(const event::data<EVENT::GPAD_DISCONNECTED> &ed) noexcept {
        QTimer::singleShot(0, this, [this] {
            _status_gamepad->setText("Gamepad: None");
        });
    }

    void handle(const event::data<EVENT::SYS_SHOW_STATUS> &ed) noexcept;

  private:

    using inherited = QMainWindow;
    std::unique_ptr<Ui::ZxWindow> ui;

    ZxS _zxs;
    QChronoTimer _timer;
    unsigned _suspend_counter {0u};
    unsigned _perf_factor {1u};

    QImage _image{zx::screen::disp_width, zx::screen::disp_height, QImage::Format_RGB32};
    QPainter _painter{&_image};

    QPointer<QMediaDevices> _devices { new QMediaDevices(this) };
    QPointer<QIODevice> _audio_io { nullptr };
    std::unique_ptr<QAudioSink> _audio_sink { };

    std::vector<std::string_view> _joysticks;
    size_t _curr_joystick;

    std::vector<std::string_view> _mouses;
    size_t _curr_mouse;

    bool _mouse_captured {false};
    double _mouse_x = zx::screen::disp_width / 2.0;
    double _mouse_y = zx::screen::disp_height / 2.0;

    using loader_t = std::function < std::expected<int, std::string> (std::vector<uint8_t> &&) >;
    using exporter_t = std::function <std::expected<std::vector<uint8_t>, std::string> ()>;
    using loader_map_t = std::map<std::string, loader_t>;
    using exporter_map_t = std::map<std::string, exporter_t>;
    using loader_kind_map_t = std::map<std::string, loader_map_t>;
    using exporter_kind_map_t = std::map<std::string, exporter_map_t>;

    const loader_kind_map_t _loaders;
    const exporter_kind_map_t _exporters;
    loader_map_t _loader_lookup;
    exporter_map_t _exporter_lookup;
    QString _open_selector;
    QString _save_selector;
    QPointer<ZxControlPanel> _cpanel {new ZxControlPanel(this)};
    QPointer<QLabel> _status_joystick {new QLabel()};
    QPointer<QLabel> _status_gamepad {new QLabel()};
    QPointer<QLabel> _status_mouse {new QLabel()};
    QPointer<QLabel> _status_perf {new QLabel()};
    SdlGamepad _gamepad;
};
#endif // ZXWINDOW_H
