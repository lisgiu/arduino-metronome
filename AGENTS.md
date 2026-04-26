# Arduino Metronome

## Project Type
Arduino sketch (`.ino` file), not a typical software project. No build/test/lint scripts exist.

## How to Work With This Repo
- Open `metronom.ino` in Arduino IDE to edit, verify, or upload to Arduino Uno
- Or use `arduino-cli compile --upload` with appropriate board flags

## Hardware
- Arduino Uno (Rev3)
- LCD 16x2 display (LiquidCrystal library)
- 2 LEDs: red (beat), yellow (subdivision)
- Piezo speaker for audio click
- Potentiometer A0: controls BPM (40-240 range)
- 3 pushbuttons (A1): tick on/off, sound on/off, count (1-8)

## Code Architecture
- Single-file sketch with state machine for button handling and metronome timing
- Custom LCD chars: play/stopped, sound on/off icons
- BPM mapped from analog pot: `map(potVal, 0, 1023, 40, 240)`
- Beat timing: `interval = 60L * 1000 / bpm` (ms between ticks)

## Key Constraints
- No automated tests - verify manually with hardware
- Serial debug lines commented out (uncomment if needed)