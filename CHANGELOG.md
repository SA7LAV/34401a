# Changelog

All notable changes to this project are documented here.
Versions follow [Semantic Versioning](https://semver.org/).

---

## [1.1.8] – 2026-10-01

### Added
- Resolution (counts) setting in Setup → Measurement
  (100000 / 10000 / 1000) to speed up the measurement rate, e.g. for a
  continuous continuity beeper
- `SYST:ERR?` error codes are shown in the status bar when a `READ?` goes
  unanswered

### Fixed
- Connecting and switching measurement function/range stalled for the full
  serial timeout (~9 s): the instrument ignores `READ?` while it is busy
  measuring, and the fast 50 ms poll kept hitting that busy window. Reads are
  now paced — one `READ?` at a time, retried until the response arrives — so
  a new value appears ~2 s after connect or a switch instead of ~9 s
- The instrument's error LED lit up and the (undisableable) error beep
  sounded repeatedly: the 34401A has no `:RANG` command, so every range
  button sent an invalid command; the range is now set as the first
  `CONFigure` parameter. An invalid `NPLC` command was also sent for AC
  measurements (only dc voltage/current and resistance support NPLC), and a
  redundant auto-range `CONFigure` followed every mode change into the
  instrument's reconfiguration window
- The instrument's error LED lit up frequently: hammering it with `READ?`
  every 50 ms overran its small serial buffer. With paced polling the
  command rate matches the measurement rate, so no more overruns

---

## [1.1.7] – 2026-10-01

### Fixed
- Polling stutter: serial responses are now read in non-blocking chunks via a
  line buffer instead of a blocking `readLine()`, and pending commands pull
  the next poll forward instead of waiting a full sampling interval

---

## [1.1.6] – 2026-07-03

### Added
- The Stop button now toggles to "Resume": measurements can be paused and
  continued on the still-open connection without disconnecting and
  reconnecting

### Fixed
- Error messages are no longer overwritten by the "stopped" status when a
  measurement is halted
- Documentation: corrected the outdated troubleshooting note claiming
  overrange (`+9.9E+37`) values are ignored — they are now shown as
  `OVL.D` / `OPEN` / `OVLD`

---

## [1.1.5] – 2026-07-02

### Added
- Configurable overlay background colour (Setup → Overlay), defaulting to
  chroma-key green `#00FF00`

### Fixed
- Overrange readings (`+9.9E+37`) were discarded silently, leaving the display
  frozen on the last valid value. An overload is now shown explicitly,
  mirroring the front panel: `OVL.D` (2W/4W resistance), `OPEN`
  (diode/continuity), `OVLD` (all other functions), on both the main display
  and the OBS overlay

---

## [1.1.4] – 2026-06-30

### Changed
- README: removed "Building Packages" section (packages are distributed pre-built)
- README: download links updated to GitHub releases page
- README: fixed `cd` directory name after `git clone`

---

## [1.1.3] – 2026-06-30

### Added
- MIT LICENSE file
- Doxyfile for generating HTML API documentation
- CHANGELOG.md

### Changed
- `.gitignore` updated for C++/Qt6 project (removed Python/PyQt5 remnants)
- README author and repository updated to kalle@sa7lav.se / github.com/SA7LAV/34401a
- Doxygen-standard documentation added to all source headers and key implementation files

---

## [1.1.2] – 2026-06-30

### Added
- Swedish (Svenska) as a third UI language, switchable live without restart

---

## [1.1.1] – 2026-06-30

### Added
- OBS streaming overlay: frameless chroma-key window with configurable font, colour and size
- Version number displayed in the bottom-right corner of the status bar
- Window title updated to **HP/AGILENT/KEYSIGHT 34401A – Multimeter Control**

### Fixed
- Gap between measurement value and unit label (VDC, VDA …) was not dynamic;
  the SI-prefix label is now hidden when empty so the unit sits flush against the value

### Changed
- Complete rewrite from Python/PyQt5 to **C++17/Qt6** — same feature set,
  significantly better performance and packaging
- Packaging: AppImage, `.deb`, and `.pkg.tar.zst` now built from C++ source

---

*Earlier versions (1.0.x) were Python/PyQt5 releases and are not documented here.*
