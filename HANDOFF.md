# Project Handoff

Updated: 2026-08-22

## Completed

- Preserved the original full 8 MB device flash at
  `../backups/ai-passport-original-20260822.bin`.
- Installed ESP-IDF 5.5.3 and built the custom firmware successfully.
- Added and physically verified the fictional pixel Leader mini-game:
  colored character, UP attack, DOWN slap, Chinese apology voice, short-OK
  reset, and long-OK menu return.
- Set default display backlight to 55%.
- Fixed continuous speaker noise by closing the codec/PA after playback and on
  audio-demo exit.
- Added `玩法说明.md` and initialized project continuity in `CURRENT.md`.

## Current baseline

- Firmware repository: this directory; upstream is `FoloToy/ai-passport`.
- Device: ESP32-C3, 8 MB Flash, native USB Serial/JTAG.
- Current firmware is offline; the board has Wi-Fi hardware, but the BSP and
  `main` do not initialize a wireless stack yet.
- The original backup remains untouched.

## Proposed next phase

Build a mother-friendly family voice terminal: push-to-talk recording on the
device, a Mac/cloud bridge for speech-to-text → AI → text-to-speech, persisted
conversation records, and a nightly summary for Franklin.

## Next action

Start with a local Mac bridge using a saved audio file. Prove the full AI
pipeline before changing device networking or buying another board.

## Important constraints

- Do not put API keys in firmware.
- Prefer push-to-talk over always-on listening for privacy, battery, and
  reliability.
- Separate build results, boot logs, and physical acceptance in reports.
- Keep the original flash backup available for rollback.
