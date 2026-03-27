#pragma once
#ifndef ZXCONTROLPANEL_H
#define ZXCONTROLPANEL_H

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

#include "zxevents.h"
#include <flat_set>
#include <QToolButton>
#include <QTabWidget>

namespace Ui {
class ZxControlPanel;
}

class ZxControlPanel:
    public QTabWidget,
    public event::handler <ZxControlPanel, EVENT::TAPE_IMAGE_LOADED, EVENT::TAPE_STARTED, EVENT::TAPE_STOPPED, EVENT::TAPE_CURSOR_MOVED> {
    Q_OBJECT

  public:

    explicit ZxControlPanel(QWidget *parent = nullptr);
    ~ZxControlPanel() override;

    bool eventFilter(QObject *watched, QEvent *event) noexcept override;

  public:

    void handle(const event::data<EVENT::TAPE_IMAGE_LOADED> &ed) noexcept;
    void handle(const event::data<EVENT::TAPE_STARTED> &ed) noexcept;
    void handle(const event::data<EVENT::TAPE_STOPPED> &ed) noexcept;
    void handle(const event::data<EVENT::TAPE_CURSOR_MOVED> &ed) noexcept;

  private slots:

    void slotTapePlayOn() noexcept;
    void slotTapePlayOff() noexcept;
    void slotTapeRewind() noexcept;
    void slotTapeFastForward() noexcept;
    void slotTapeRewindToOrigin() noexcept;
    void slotQuickLoad() noexcept;
    void slotTapeReset() noexcept;
    void slotNMI() noexcept;
    void slotRestoreROM() noexcept;
    void on_slPerformance_valueChanged(int value);
    void on_cbPerfOnTapeStart_toggled(bool checked);
    void on_cbPerfOnTapeStop_toggled(bool checked);
    void on_cbxAPDS_stateChanged(int state);

  private:

    using inherited = QTabWidget;
    Ui::ZxControlPanel *ui;
    std::flat_set<QToolButton *> _held_buttons {};
};

#endif // ZXCONTROLPANEL_H
