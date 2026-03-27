# RISE User Guide

This guide describes how to use the RISE ZX Spectrum 48K emulator.

> 💡 **Tip**: All keyboard shortcuts are shown in tooltips — just hover over any button.

---

## 📑 Table of Contents

- [Quick Start](#-quick-start)
- [Opening Files](#-opening-files)
- [Saving States](#-saving-states)
- [Input Devices](#-input-devices)
  - [Keyboard](#keyboard)
  - [Virtual Joysticks](#virtual-joysticks)
  - [Gamepad Support](#gamepad-support)
  - [Mouse Support](#mouse-support)
- [Tape Emulation](#-tape-emulation)
  - [Automatic Loading (APDS)](#automatic-loading-apds)
  - [Manual Tape Control](#manual-tape-control)
  - [Quick Load](#quick-load)
- [System Settings](#-system-settings)
- [Virtual Keyboard](#-virtual-keyboard)
- [Status Bar](#-status-bar)
- [Known Limitations & Roadmap](#-known-limitations--roadmap)

---

## 🚀 Quick Start

### 🎮 Loading a Game from Tape (3 clicks)

```
1. Click 📂 Open (or F3) and select a .tap or .tzx file
   → The Devices window will open automatically on the Tape tab

2. Click 🚀 Quick Load (rocket button)
   → Tape rewinds, LOAD "" command is sent, playback starts

3. Wait for loading to complete
   → Play! 🎉
```

### 🕹️ Playing with a Gamepad

```
1. Connect a USB/Bluetooth gamepad
   → It will be detected automatically (shown in the status bar)

2. Press Tab or 🕹️ Joystick until "Kempston" is selected
   → Status bar shows: "Virt. joystick 3/5: Kempston"

3. Use the gamepad:
   - Stick/D-pad → joystick directions
   - X button → Space, Y button → Enter
   - All other buttons → fire
```

### 💾 Saving Progress

```
1. Press F5 or 💾 Save
2. Choose a location and enter a filename with .z80 extension (e.g., `game.z80`)
3. To load later: File → Open → select your .z80 file
```

---

## 📂 Opening Files

**Supported formats:**

| Type | Extensions | Notes |
|------|-----------|-------|
| **ROM** | `.rom`, `.bin` | Max 16 KB (smaller dumps are also supported) |
| **Snapshot** | `.z80`, `.sna`, `.mgc` | Loading any; saving only in `.z80` (v1.0) |
| **Tape** | `.tap`, `.tzx` | Full TZX block support |

**How to open:**

- 🔘 **Button**: Click 📂 Open (F3) → select file
- 🖱️ **Drag & Drop**: Drag any supported file onto the emulator window *(recommended when working with multiple files)*

**Loading a custom ROM:**
- Use File → Open (F3) and select a `.rom` or `.bin` file
- The emulator will restart with the new ROM
- To revert to the original ROM, click **Restore ROM** on the System tab

---

## 💾 Saving States

**Current version (v1.0):**

- ✅ Saving only in `.z80` format
- 📁 Location: chosen by the user via standard file dialog
- 📝 **You must manually add the `.z80` extension** — enter the filename with extension, e.g., `game.z80`
- ♻️ **Loading**: Saved `.z80` files can be opened via Open (F3)

> 🔮 **Planned**: Support for `.sna`, `.mgc` formats for saving and tape recording in future versions. Format selection will be available via a dropdown in the save dialog.

---

## 🎮 Input Devices

### Keyboard

**Key mapping:**

| Spectrum Key | PC Key |
|-----------------|------------|
| Arrows (5,6,7,8) | Arrow keys |
| Fire button | `Left Alt` |
| Caps Shift | `Right Shift` |
| Symbol Shift | `Right Ctrl` |
| Space | `Space` |
| Enter | `Enter` |

> 💡 Full layout: see virtual keyboard (Devices → Keyboard).

### Virtual Joysticks

**5 modes** (switch: `Tab` or 🕹️):

| Mode | Description | Best For |
|------|-------------|----------|
| **Cursor control** | Emulates **arrow keys in BASIC mode**: cursor movement with Caps Shift held | BASIC navigation, program editing |
| **Kempston** | Widely used third-party joystick interface | Most games |
| **Sinclair Interface 2** | Two-joystick support (left/right ports) via a single standard | Games with dual controls |
| **Protek** | Also known as AGF | Certain games |
| **Cursor** | Emulates **direct key presses of 5–8 and 0** (no modifiers) | Games that use these keys as joystick |

**Brief explanation:**
- On the ZX Spectrum, physical keys **5, 6, 7, 8** act as arrow keys
- In BASIC, they move the cursor **only when Caps Shift is held** ( **Cursor control** mode)
- In games, they are often used as a joystick directly ( **Cursor** mode)

**Status bar**: Shows current mode, e.g., `Virt. joystick 3/5: Kempston`

### Gamepad Support

**Auto-detection**: Connect/disconnect at any time via USB or Bluetooth.

**Button mapping** (fixed for simplicity):

| Gamepad Input | Emulated Action |
|--------------|-----------------|
| Left/right stick, D-pad | Joystick directions |
| `X` button | `Space` |
| `Y` button | `Enter` |
| All other buttons (`A`, `B`, shoulders, triggers) | Fire button |

**Selecting a gamepad when multiple are connected:**

If more than one gamepad is connected, a **modal dialog** will appear asking you to select the active controller. Emulation will pause until a choice is made.

> ⚙️ **Design note**: Button mapping is not configurable to keep the interface simple. Defaults are chosen for typical use.

### Mouse Support

**3 modes** (toggle: 🖱️; no hotkey):

| Mode | Description |
|------|-------------|
| **Disconnected** *(default)* | Mouse emulation is disabled |
| **Kempston** | Standard Kempston mouse emulation |
| **Kempston (inverted)** | Buttons swapped (for compatibility with software that uses opposite bit assignments) |

**How to use**:

1. Select Kempston or Kempston (inverted)
2. **Click inside the emulator window** → mouse is captured
3. Move/click to interact with emulated software
4. Press `Esc` → release mouse capture

**How to disable mouse:**
- Click 🖱️ again and select **Disconnected**
- Or simply exit capture mode by pressing `Esc`

> ⚠️ **Why Disconnected by default?** Mouse capture prevents using the mouse outside the emulator window. Since mouse support is rarely needed, it's disabled by default for convenience.

---

## 📼 Tape Emulation

### Automatic Loading (APDS)

**APDS** (Automatic Polling Detection System) automates tape playback:

```
✅ Spectrum polls the tape port → APDS starts playback
✅ Loading completes → APDS automatically stops the tape
```

**Two modes** (switch on the Tape tab):

| Mode | Behavior | When to Use |
|------|----------|-------------|
| **Strict** *(default)* | Triggers only if **no keyboard row is being polled** (except the space row) | Most games and loaders |
| **Lenient** | Allows tape detection even if keyboard polling looks similar | Programs with non-standard input handling |

**Recommended workflow**:

```
1️⃣ Try loading with APDS Strict (default)
   → Works for most software

2️⃣ If loading hangs unexpectedly (no on-screen prompts)
   → Switch to APDS Lenient and try again

3️⃣ If it still doesn't work
   → Disable APDS, use manual control (see below)
```

**Known APDS limitations** (require manual control):

| Issue | Why It Happens | Solution |
|-------|----------------|----------|
| Very short tape blocks | APDS cannot react faster than ~0.6 seconds of virtual time | Disable APDS, control manually |
| Loaders skipping blocks | APDS stops tape → loader looks → APDS restarts same block → cycle | Disable APDS |
| Copy protection polling pauses | APDS interprets pause as "done" | Disable APDS |

> 👁️ **Debug tip**: The red arrow in the block list shows the current playback position — helps identify where loading got stuck.

### Manual Tape Control

**Controls** (Devices → Tape tab):

| Button | Action |
|--------|--------|
| ▶️ Play | Start playback from the current block |
| ⏹️ Stop | Stop playback (cursor stays in place) |
| ⏮️ Rewind | Move cursor back 1 block |
| ⏭️ Fast Forward | Move cursor forward 1 block |
| ⏪ Rewind to Start | Go to first block + reset internal state (loop/call stack) |
| 🗑️ Clear | Unload tape image ("eject cassette") |

**Block list** (informational only):

| Column | Shows |
|--------|-------|
| Type | Block type (Std. speed, Pure data, etc.) |
| Size | Block size (hex) |
| Description | Technical details (pilot, pause, etc.) |

> 🔒 List is **read-only** to prevent accidental modification of block order.

### Quick Load

**🚀 Rocket button** = one-click loading:

```
1. Rewinds to start (Rewind to Start)
2. Sends LOAD command to Spectrum
3. Starts tape playback
```

**Command type** (dropdown next to rocket):

| Option | BASIC Command | Purpose |
|--------|--------------|---------|
| **Program** *(default)* | `LOAD ""` | Loading programs |
| **Code** | `LOAD "" CODE` | Loading machine code |
| **Screen** | `LOAD "" SCREEN$` | Loading screen images |

**Workflow**:
```
Open tape file → Click 🚀 → Done!
```

---

## ⚙️ System Settings

**Performance slider**:

- 🐢 **x1** *(default)*: Real-time emulation (1:1)
- 🚀 **x2…x10**: Emulation speed multiplier

**Hooks** (automatic speed changes):

| Hook | Default | Purpose |
|------|---------|---------|
| On tape start | x10 | Fast loading |
| On tape stop | x1 | Return to normal speed for gameplay |

- 🔢 Values configurable (1–10)
- ☑️ Can be completely disabled

**Action buttons**:

| Button | Action |
|--------|--------|
| **NMI** | Generate Non-Maskable Interrupt (hardware button on real Spectrum) |
| **Restore ROM** | Restore original spectrum.rom + reset emulator *(no confirmation)* |

> ⚠️ **Restore ROM** replaces any custom ROM immediately. Useful if a bad ROM dump causes instability.

---

## ⌨️ Virtual Keyboard

**Location**: Open the Devices window (toolbar button) → **Keyboard** tab.

![Virtual Keyboard](docs/images/screenshot-keyboard.png)

**Mouse control**:

| Action | Result |
|--------|--------|
| **Left click** on a key | Press that key once in the emulator |
| **Right click** on Caps Shift / Symbol Shift | **Lock** the modifier until the next press |

**Example: entering a red keyword**
```
1. Right-click "Caps Shift" → it stays pressed
2. Left-click "A" → EDIT command is sent (Caps+A)
3. Caps Shift automatically releases
```

> ✅ Allows entering complex key combinations with mouse only — no need to hold two keys simultaneously.

---

## 📊 Status Bar

Shows the current emulator state in real time:

| Element | Example | Meaning |
|---------|--------|---------|
| Virt. joystick | `3/5: Kempston` | Current joystick mode (3 of 5) |
| Gamepad | `SHANWAN Android Gamepad` | Connected gamepad name |
| Mouse | `1/3: Disconnected` | Current mouse mode |
| Perf | `x1` or `x10` | Emulation speed multiplier |

> 🔄 Updates automatically when settings change.

---

## ⚠️ Known Limitations & Roadmap

### Not Implemented in v1.0

| Feature | Status |
|---------|--------|
| AY-3-8912 sound chip | ❌ Planned |
| ZX Spectrum 128K / +2 / +3 emulation | ❌ Planned |
| Disk drive support (+D, DISCiPLE, etc.) | ❌ Planned |
| General Sound / DMA Ultra Sound | ❌ Planned (sponsor-driven) |

### Development Roadmap

Feature priorities are determined by community interest and sponsor support via [Boosty](https://boosty.to/rise_emulator):

```
🎯 Near-term plans:
   • AY-3-8912 sound
   • 128K memory modes

🔜 Medium-term plans:
   • Disk drive emulation
   • Additional snapshot formats for saving

🗳️ Long-term plans (sponsor requests):
   • General Sound, DMA Ultra Sound
   • Extended video modes
   • Network features
```

> 💡 **Want to accelerate development?** Support the project on [Boosty](https://boosty.to/rise_emulator) — your wishes help set priorities!

---

## 📬 Help & Support

- 🐛 **Bug Reports**: [GitHub Issues](https://github.com/dmitry-starushko/rise/issues)
- 💬 **Questions & Ideas**: [GitHub Discussions](https://github.com/dmitry-starushko/rise/discussions)
- 📧 **Email**: dmitry.starushko@gmail.com

*Last updated: March 2026*
