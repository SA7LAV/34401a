# Changelog

All notable changes to this project are documented here.
Versions follow [Semantic Versioning](https://semver.org/).

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
