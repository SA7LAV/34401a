# HP / Agilent / Keysight 34401A – Linux GUI

Linux control software for the HP/Agilent/Keysight 34401A digital multimeter.  
Supports all 10 measurement modes with live display, MIN/MAX/AVG statistics and configurable settings.

![Screenshot](assets/screenshot.png)

---

## Features

- **10 measurement modes:** VDC, ADC, VAC, AAC, 2W Ω, 4W Ω, Frequency, Period, Diode, Continuity
- **Live display** with large readout and SI prefix scaling (m, k, M, …)
- **MIN / MAX / AVG** statistics
- **Configurable sampling rate** (50 ms / 200 ms / 500 ms)
- **NPLC** (integration time) and **Auto-Zero** settings
- **Language switch** (Deutsch / English) – live, no restart needed
- **Adjustable decimal places** (2 / 4 / 6)
- Settings are saved across sessions

---

## Hardware Requirements

- HP / Agilent / Keysight **34401A** multimeter
- **USB-to-Serial adapter** (e.g. CH340, FTDI, CP2102)
- **Null-modem cable or adapter** (crossed RX/TX lines – required!)
- Multimeter set to: Interface → **RS-232**, **SCPI mode** (not HP mode), **9600 baud**

> **Note:** A straight RS-232 cable will not work. You need a null-modem cable or a null-modem adapter between a straight cable and the multimeter.

---

## Installation

### Option 1 – AppImage (any Linux distro, no install needed)

Download `HP_34401A_GUI-1.0.0-x86_64.AppImage` from the [releases page](https://git.cls.net/kalle/Agilent_34401a_GUI/releases).

```bash
chmod +x HP_34401A_GUI-1.0.0-x86_64.AppImage
./HP_34401A_GUI-1.0.0-x86_64.AppImage
```

No Python, no dependencies – everything is bundled.

---

### Option 2 – Arch Linux / CachyOS (.pkg.tar.zst)

Download `hp34401a-gui-1.0.0-1-x86_64.pkg.tar.zst` from the releases page.

```bash
sudo pacman -U hp34401a-gui-1.0.0-1-x86_64.pkg.tar.zst
hp34401a
```

---

### Option 3 – Ubuntu / Debian (.deb)

Download `hp34401a-gui_1.0.0_amd64.deb` from the releases page.

```bash
sudo dpkg -i hp34401a-gui_1.0.0_amd64.deb
hp34401a
```

After installation the app also appears in your application menu under **Science → HP 34401A Multimeter**.

---

### Option 4 – Build from source

Requirements: Python 3.10+, PyQt5, pyserial

```bash
git clone https://git.cls.net/kalle/Agilent_34401a_GUI.git
cd Agilent_34401a_GUI
pip install PyQt5 pyserial
python3 main.py
```

---

## Serial Port Permissions

On most Linux systems your user must be in the `dialout` (Ubuntu/Debian) or `uucp` (Arch/CachyOS) group to access serial ports:

```bash
# Ubuntu / Debian
sudo usermod -aG dialout $USER

# Arch / CachyOS
sudo usermod -aG uucp $USER
```

Log out and back in for the change to take effect.

---

## First Launch

1. Connect the USB-to-Serial adapter and null-modem cable to the multimeter
2. Power on the multimeter
3. On the multimeter: press **SHIFT → MENU** → Interface → RS-232, Baud → 9600, Parity → None, SCPI
4. Start the app: `hp34401a`
5. Click **⚙ Setup** → **Interface** tab → select the correct port (usually `/dev/ttyUSB0`)
6. Click **Connect** – the status bar shows *Connected: /dev/ttyUSB0 @ 9600*
7. Measurements appear live in the display

---

## Usage

### Measurement modes

Click any button in the **FUNCTION** panel on the left to switch modes.  
The **RANGE** buttons at the bottom update automatically for the selected mode.  
**Auto** selects the optimal range automatically.

### MIN / MAX / AVG

Statistics are calculated from the moment you connect. Click a different **FUNCTION** to reset them.

### Setup dialog (⚙ Setup)

| Tab | Settings |
|-----|----------|
| **Language / Sprache** | Switch between Deutsch and English (live preview) |
| **Interface / Schnittstelle** | Port, baud rate, parity, stop bits, data bits, timeout |
| **Measurement / Messung** | Sampling rate, NPLC (integration time), Auto-Zero |
| **Display / Anzeige** | Decimal places (2 / 4 / 6), show/hide MIN/MAX/AVG |

Settings are saved to `~/.config/hp34401a/serial_config.ini`.

### Buttons

| Button | Function |
|--------|----------|
| **⚙ Setup** | Open settings dialog |
| **Connect / Verbinden** | Open connection dialog and connect; click again to disconnect |
| **Stop** | Pause measurements (keeps connection) |
| **Local** | Release the multimeter from remote control (SYST:LOC) |

---

## Multimeter Setup (RS-232)

The multimeter must be configured for RS-232/SCPI mode before connecting:

1. Press **SHIFT** + **MENU** on the front panel
2. Navigate to: **I/O** → **INTERFACE** → **RS-232**
3. Set **BAUD** to **9600**
4. Set **PARITY** to **NONE**
5. Set **LANGUAGE** to **SCPI** (not HP)
6. Press **ENTER** to confirm

Default serial settings used by this app: 9600 baud, 8N1 (8 data bits, no parity, 1 stop bit).  
These match the multimeter factory defaults.

---

## Building Packages

To build all package formats from source on Arch/CachyOS:

```bash
# Install build dependencies (one-time)
pip install pyinstaller
sudo pacman -S dpkg
yay -S appimagetool-bin

# Build everything
bash packaging/build-all.sh

# Additionally build the .pkg.tar.zst
cd packaging && makepkg -f
```

**Output files:**

| File | Format | Platform |
|------|--------|----------|
| `dist/HP_34401A_GUI-1.0.0-x86_64.AppImage` | AppImage | Any Linux |
| `dist/hp34401a-gui_1.0.0_amd64.deb` | .deb | Ubuntu / Debian |
| `packaging/hp34401a-gui-1.0.0-1-x86_64.pkg.tar.zst` | pacman | Arch / CachyOS |

---

## Troubleshooting

**No serial port visible in the Setup dialog:**  
→ Check USB-to-Serial adapter is connected: `ls /dev/ttyUSB*`  
→ Check group membership: `groups $USER`

**Connection error / timeout:**  
→ Verify null-modem cable (not a straight cable)  
→ Verify multimeter is in SCPI mode (not HP mode)  
→ Try increasing Timeout in Setup → Interface

**Reading shows +9.90000E+37:**  
→ Multimeter is in overload. The app ignores these values automatically.

**App opens but display stays at `----`:**  
→ Click **Connect**, select port and confirm.

---

## License

MIT – see source repository for details.

## Author

Kalle Hagen – [dev@hammarang-solutions.se](mailto:dev@hammarang-solutions.se)  
Repository: [git.cls.net/kalle/Agilent_34401a_GUI](https://git.cls.net/kalle/Agilent_34401a_GUI)
