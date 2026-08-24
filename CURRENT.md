# Current State

Updated: 2026-08-22

## Project identity

This is Franklin's long-lived FoloToy AI Passport hardware project. The
`firmware/` repository is the code authority; the parent workspace also holds
the preserved original-flash backup and local installation configuration.

## Status

Leader mini-game v1 is implemented, flashed, and accepted on the physical
device. The project context is now initialized for continuation across
conversations.

## Current focus

The next product direction is a family voice terminal: a mother-friendly,
button-first device that can upload a question over Wi-Fi, receive an AI voice
answer, and produce a nightly summary for Franklin. This is a proposed next
phase, not yet implemented in firmware.

## Confirmed decisions

- Preserve the original 8 MB flash backup before any further destructive work.
- Keep the official hardware identity as `AI Passport`; the personal nickname
  can be `口袋护照`.
- Keep current Leader interactions and 55% default backlight as the accepted
  baseline.
- For the family voice concept, prefer push-to-talk over always-on listening
  for simplicity, battery life, and privacy.
- Keep API keys and cloud credentials off the device; a Mac or cloud bridge
  should own them.

## Phase

Leader mini-game v1 is accepted; family voice-terminal discovery is next.

## Objective

Maintain the accepted offline AI Passport baseline and extend it into useful
personal and family-facing modes without erasing the preserved stock firmware
backup.

## Implementation state

- Sprite sheet generated and converted to four 96x180 RGB565 frames.
- Leader demo is wired into the main menu and button dispatcher.
- Final Chinese TTS asset is generated with `baqiqingshu`, sad/emotive direction,
  and converted to 16 kHz mono PCM.

## Verification state

- Static diff checks: passed.
- Existing `test_ui_pixel_math` host test: passed.
- ESP-IDF 5.5.3 build: passed; generated `build/FoloToy-AI-Passport.bin`
  (0xd5c70 bytes, 16% free in the 1 MiB app partition).
- Device flash: passed on `/dev/cu.usbmodem1101`; bootloader, partition table,
  and app hashes verified.
- Boot log: passed; display, ADC buttons, ES8311 audio codec, and CW2017
  battery gauge all initialized successfully.
- Default display backlight adjusted to 55% after physical feedback that 100%
  was too bright; rebuilt and reflashed successfully.
- Audio safety fix: playback now closes the codec/PA after PCM playback and on
  audio-demo exit, preventing continuous speaker底噪; rebuilt, reflashed, and
  boot-verified.
- Physical interaction acceptance: passed; user confirmed the display, buttons,
  voice playback, brightness, and post-playback silence are all OK.

## Stop conditions

The firmware satisfies the build, asset, flash, boot, and physical-interaction
gates. Keep the original backup untouched for rollback.

## Next step

Prototype the bridge on Franklin's Mac with a saved audio file before adding
Wi-Fi to the FoloToy firmware. The bridge should transcribe audio, call the AI,
generate a spoken reply, persist the transcript, and produce a nightly summary.

## Open questions / blockers

- Does the mother's home have stable 2.4 GHz Wi-Fi?
- Should the first family flow be push-to-talk or a wake-word interaction?
- Should the bridge remain on the Mac during validation or move to an
  always-on cloud host after the local proof of concept?
- API access and billing must be configured separately from a ChatGPT
  subscription before the bridge can call OpenAI services.
