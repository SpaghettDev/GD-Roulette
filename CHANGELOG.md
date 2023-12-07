# GD-Roulette Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [1.2.0] - 2023-12-06

### Added
- Added a json manager (to save options)
- Added a percentage goal besides the LevelInfoLayer percentage text
- Added a percentage goal in the PauseLayer

### Changed

- Separated the layers contained in the RouletteLayer.cpp file into sub-files

### Fixed

- Fixed RouletteManager not managing data correctly after JSON saving was added
- Fixed demonDiffArr resetting everything to false when RouletteLayer::onClose is called

## [1.1.7] - 2023-10-29

### Added
- Added a DEV_CONSOLE macro enabled only when compiling RelWithDebInfo
- Added a check for MegaHack v7 to change the roulette button's position

<!-- maybe add the other versions here too -->
