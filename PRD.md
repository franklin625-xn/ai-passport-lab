# AI Passport Leader Mini-Game

## Product claim

On the AI Passport device, a user can play a short, clearly fictional pixel-art
interaction with a replaceable cartoon police-leader character using only the
three hardware buttons.

## First user path

Open `Leader` from the device menu, press UP or DOWN to trigger the two comic
reaction animations, hear the apology voice, and press OK to reset. Long-press
OK to return to the menu.

## Version 1 scope

- Four replaceable RGB565 character frames.
- UP = attack reaction; DOWN = slap reaction; short OK = reset.
- A short 16 kHz mono apology voice asset.
- Existing BSP demos remain available from the menu.

## Non-goals

- No real-person likeness, real police insignia, blood, weapons, or real-world
  violence.
- No account binding, network service, or modification of the original backup.
- No editor or asset-management UI in this first version.

## Acceptance criteria

- `idf.py build` succeeds on ESP-IDF 5.5.3.
- The Leader page renders on the 240x320 display and reacts to all three keys.
- The apology asset is audible after either action and does not block button UI.
- The original 8 MB flash backup remains unchanged and available outside build.
