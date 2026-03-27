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

#include "zxcontrolpanel.h"
#include "ui_zxcontrolpanel.h"
#include <zstd.h>
#include <QTimer>
#include <QStandardItemModel>
#include <QEvent>
#include <QMouseEvent>

ZxControlPanel::ZxControlPanel(QWidget *parent)
    : QTabWidget(parent)
    , ui(new Ui::ZxControlPanel) {

    ui->setupUi(this);
    setWindowTitle("Devices");
    setWindowFlags(windowFlags() | Qt::Tool);
    adjustSize();
    setFixedSize(size());

    // -- Tape

    ui->tbPlay->setDefaultAction(ui->actionTapePlayOn);
    ui->tbStop->setDefaultAction(ui->actionTapePlayOff);
    ui->tbRewind->setDefaultAction(ui->actionTapeRewind);
    ui->tbFastForward->setDefaultAction(ui->actionTapeFastForward);
    ui->tbRewindToStart->setDefaultAction(ui->actionTapeRewindToOrigin);
    ui->tbReset->setDefaultAction(ui->actionTapeReset);
    ui->tbQuickLoad->setDefaultAction(ui->actionQuickLoad);

    ui->tvTape->horizontalHeader()->setStretchLastSection(true);
    ui->tvTape->setAlternatingRowColors(true);
    ui->tvTape->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tvTape->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tvTape->setShowGrid(true);
    ui->tvTape->setFocusPolicy(Qt::NoFocus);
    ui->cbxAPDS->setCheckState(Qt::CheckState::Checked);

    // -- Virtual keyboard

    std::unordered_map<QToolButton *, zx::kbd::key> btn_map = {
        {ui->kb0, zx::kbd::key::D0},
        {ui->kb1, zx::kbd::key::D1},
        {ui->kb2, zx::kbd::key::D2},
        {ui->kb3, zx::kbd::key::D3},
        {ui->kb4, zx::kbd::key::D4},
        {ui->kb5, zx::kbd::key::D5},
        {ui->kb6, zx::kbd::key::D6},
        {ui->kb7, zx::kbd::key::D7},
        {ui->kb8, zx::kbd::key::D8},
        {ui->kb9, zx::kbd::key::D9},
        {ui->kbA, zx::kbd::key::A},
        {ui->kbB, zx::kbd::key::B},
        {ui->kbC, zx::kbd::key::C},
        {ui->kbD, zx::kbd::key::D},
        {ui->kbE, zx::kbd::key::E},
        {ui->kbF, zx::kbd::key::F},
        {ui->kbG, zx::kbd::key::G},
        {ui->kbH, zx::kbd::key::H},
        {ui->kbI, zx::kbd::key::I},
        {ui->kbJ, zx::kbd::key::J},
        {ui->kbK, zx::kbd::key::K},
        {ui->kbL, zx::kbd::key::L},
        {ui->kbM, zx::kbd::key::M},
        {ui->kbN, zx::kbd::key::N},
        {ui->kbO, zx::kbd::key::O},
        {ui->kbP, zx::kbd::key::P},
        {ui->kbQ, zx::kbd::key::Q},
        {ui->kbR, zx::kbd::key::R},
        {ui->kbS, zx::kbd::key::S},
        {ui->kbT, zx::kbd::key::T},
        {ui->kbU, zx::kbd::key::U},
        {ui->kbV, zx::kbd::key::V},
        {ui->kbW, zx::kbd::key::W},
        {ui->kbX, zx::kbd::key::X},
        {ui->kbY, zx::kbd::key::Y},
        {ui->kbZ, zx::kbd::key::Z},
        {ui->kbEnter, zx::kbd::key::ENTER},
        {ui->kbCaps, zx::kbd::key::CAPS_SHIFT},
        {ui->kbSymbol, zx::kbd::key::SYMBOL_SHIFT},
        {ui->kbBreak, zx::kbd::key::SPACE},
    };

    for (const auto &b : btn_map) {
        connect(b.first, &QToolButton::pressed, this, [this, btn = b.first, key = b.second] noexcept {
            if (!_held_buttons.contains(btn)) {
                event::dispatch(event::data<EVENT::KEY_DOWN> {.key = key});
            }
        });

        connect(b.first, &QToolButton::released, this, [this, btn = b.first, key = b.second] noexcept {
            const auto held = _held_buttons;
            _held_buttons.clear();

            for (const auto btn : held) {
                btn->released();
                btn->setDown(false);
            }

            if (!held.contains(btn)) {
                event::dispatch(event::data<EVENT::KEY_UP> {.key = key});
            }
        });
    }

    ui->kbCaps->installEventFilter(this);
    ui->kbSymbol->installEventFilter(this);

    // -- System

    ui->tbNMI->setDefaultAction(ui->actionNMI);
    ui->tbRestoreROM->setDefaultAction(ui->actionRestoreROM);
}

ZxControlPanel::~ZxControlPanel() {
    delete ui;
}

void ZxControlPanel::handle(const event::data<EVENT::TAPE_IMAGE_LOADED> &ed) noexcept {
    QTimer::singleShot(0, this, [this, ed = ed] {
        QStandardItemModel *model = new QStandardItemModel(ed.blocks.size(), 4);
        model->setHorizontalHeaderItem(0, new QStandardItem(""));
        model->setHorizontalHeaderItem(1, new QStandardItem("Type"));
        model->setHorizontalHeaderItem(2, new QStandardItem("Size"));
        model->setHorizontalHeaderItem(3, new QStandardItem("Description"));

        for (size_t i = 0; const auto &block : ed.blocks) {
            model->setItem(i, 0, new QStandardItem(""));
            model->setItem(i, 1, new QStandardItem(QString::fromStdString(block.type)));
            model->setItem(i, 2, new QStandardItem(QString::fromStdString(block.size)));
            model->setItem(i++, 3, new QStandardItem(QString::fromStdString(block.desc)));
        }

        ui->tvTape->setModel(model);
        ui->tvTape->setColumnWidth(0, 20);

        bool has_blocks = ed.blocks.size();
        ui->actionTapePlayOn->setEnabled(has_blocks);
        ui->actionTapePlayOff->setEnabled(false);
        ui->actionTapeRewind->setEnabled(has_blocks);
        ui->actionTapeFastForward->setEnabled(has_blocks);
        ui->actionTapeRewindToOrigin->setEnabled(has_blocks);
        ui->actionTapeReset->setEnabled(has_blocks);
        ui->actionQuickLoad->setEnabled(has_blocks);

        if (this->currentIndex() != 0) {
            this->setCurrentIndex(0);
        }

        if (this->isHidden()) {
            this->show();
        }
    });
}

void ZxControlPanel::handle(const event::data<EVENT::TAPE_STARTED> &/*ed*/) noexcept {
    QTimer::singleShot(0, this, [this] {
        ui->actionTapePlayOn->setEnabled(false);
        ui->actionTapePlayOff->setEnabled(true);
        ui->actionTapeRewind->setEnabled(false);
        ui->actionTapeFastForward->setEnabled(false);
        ui->actionTapeRewindToOrigin->setEnabled(false);
        ui->actionTapeReset->setEnabled(false);
        ui->actionQuickLoad->setEnabled(false);

        if (ui->cbPerfOnTapeStart->checkState() == Qt::Checked) {
            ui->slPerformance->setValue(ui->sbxPerfOnTapeStart->value());
        }
    });
}

void ZxControlPanel::handle(const event::data<EVENT::TAPE_STOPPED> &ed) noexcept {
    QTimer::singleShot(0, this, [this, has_blocks = ed.has_blocks] {
        ui->actionTapePlayOn->setEnabled(has_blocks);
        ui->actionTapePlayOff->setEnabled(false);
        ui->actionTapeRewind->setEnabled(has_blocks);
        ui->actionTapeFastForward->setEnabled(has_blocks);
        ui->actionTapeRewindToOrigin->setEnabled(has_blocks);
        ui->actionTapeReset->setEnabled(has_blocks);
        ui->actionQuickLoad->setEnabled(has_blocks);
        static_cast<QWidget * >(parent())->activateWindow();

        if (ui->cbPerfOnTapeStop->checkState() == Qt::Checked) {
            ui->slPerformance->setValue(ui->sbxPerfOnTapeStop->value());
        }
    });
}

void ZxControlPanel::handle(const event::data<EVENT::TAPE_CURSOR_MOVED> &ed) noexcept {
    QTimer::singleShot(0, this, [this, index = ed.block_index] {
        if (QStandardItemModel *model = static_cast<QStandardItemModel *>(ui->tvTape->model()); model) {
            ui->tvTape->scrollTo(model->index(index, 0), QAbstractItemView::PositionAtCenter);

            for (size_t rc = model->rowCount(), ri = 0; ri < rc; ++ri) {
                model->item(ri, 0)->setText(index == ri ? "→" : "");
            }
        }
    });
}

void ZxControlPanel::slotTapePlayOn() noexcept {
    event::dispatch(event::data<EVENT::TAPE_PLAY_ON> {});
}

void ZxControlPanel::slotTapePlayOff() noexcept {
    event::dispatch(event::data<EVENT::TAPE_PLAY_OFF> {});
}

void ZxControlPanel::slotTapeRewind() noexcept {
    event::dispatch(event::data<EVENT::TAPE_REWIND> {});
}

void ZxControlPanel::slotTapeFastForward() noexcept {
    event::dispatch(event::data<EVENT::TAPE_FAST_FORWARD> {});
}

void ZxControlPanel::slotTapeRewindToOrigin() noexcept {
    event::dispatch(event::data<EVENT::TAPE_REWIND_TO_START> {});
}

const std::array<std::tuple<const std::vector<uint8_t>, const size_t>, 3> loaders {
    std::tuple{
        std::vector<uint8_t>{
#embed "../bin/load.z80.zst"
        }, 0x04B4z},
    {
        std::vector<uint8_t>{
#embed "../bin/load-code.z80.zst"
        }, 0x04AEz
    },
    {
        std::vector<uint8_t>{
#embed "../bin/load-screen.z80.zst"
        }, 0x04B8z
    }
};

void ZxControlPanel::slotQuickLoad() noexcept {
    auto const &[ld_img_zst, dc_sz] = loaders[std::clamp(ui->cobLoadCommand->currentIndex(), 0, int(loaders.size()) - 1)];
    std::vector<uint8_t> dc_bytes(dc_sz);

    if (size_t const dc_real_sz = ZSTD_decompress(
            dc_bytes.data(),
            dc_bytes.size(),
            ld_img_zst.data(),
            ld_img_zst.size()); !ZSTD_isError(dc_real_sz) && dc_real_sz == dc_sz) {
        event::dispatch(event::data<EVENT::SYS_LOAD_Z80_SNAPSHOT> {.z80_snapshot = std::move(dc_bytes)});
        event::dispatch(event::data<EVENT::TAPE_REWIND_TO_START> {});
        event::dispatch(event::data<EVENT::TAPE_PLAY_ON> {});
    }
}

void ZxControlPanel::slotTapeReset() noexcept {
    event::dispatch(event::data<EVENT::TAPE_RESET> {});
}

void ZxControlPanel::slotNMI() noexcept {
    event::dispatch(event::data<EVENT::SYS_NMI> {});
}

void ZxControlPanel::slotRestoreROM() noexcept {
    event::dispatch(event::data<EVENT::SYS_RESTORE_ROM> {});
}

bool ZxControlPanel::eventFilter(QObject *watched, QEvent *event) noexcept {
    if (auto *btn = qobject_cast<QToolButton * >(watched); btn && event->type() == QEvent::MouseButtonPress) {
        if (auto *mouse_ev = static_cast<QMouseEvent *>(event); mouse_ev->button() == Qt::RightButton) {
            if (!_held_buttons.contains(btn)) {
                btn->setDown(true);
                btn->pressed();
                _held_buttons.insert(btn);
            }

            return true;
        }
    }

    return inherited::eventFilter(watched, event);
}

void ZxControlPanel::on_slPerformance_valueChanged(int value) {
    ui->lbPerformance->setText(QString::fromStdString(std::format("×{}", value)));
    event::dispatch(event::data<EVENT::SYS_SET_PERFORMANCE_FACTOR> {.perf_factor = static_cast<unsigned>(value)});
}

void ZxControlPanel::on_cbPerfOnTapeStart_toggled(bool checked) {
    ui->sbxPerfOnTapeStart->setEnabled(checked);
}

void ZxControlPanel::on_cbPerfOnTapeStop_toggled(bool checked) {
    ui->sbxPerfOnTapeStop->setEnabled(checked);
}

void ZxControlPanel::on_cbxAPDS_stateChanged(int state) {
    event::data<EVENT::TAPE_SET_APDS_MODE> ed {.mode = static_cast<APDS_MODE>(state), .mode_name = {}};
    event::dispatch(ed);
    ui->cbxAPDS->setText(QString::fromStdString(std::format("APDS ({})", ed.mode_name)));
}

