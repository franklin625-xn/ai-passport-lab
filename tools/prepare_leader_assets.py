#!/usr/bin/env python3
"""Prepare the generated leader sprite sheet and the apology voice for ESP-IDF.

The device uses LVGL RGB565 images and streams small 16 kHz PCM chunks to the
ES8311 codec. Keeping this conversion in the repo makes the asset pipeline
repeatable when the character is replaced later.
"""

from __future__ import annotations

import argparse
import wave
from pathlib import Path

from PIL import Image


SPRITE_SIZE = (96, 180)
SKY = (0x16, 0x89, 0xE8, 0xFF)
CELL_BOUNDS = (0, 400, 880, 1380, 1774)


def rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def write_rgb565(image: Image.Image, destination: Path) -> None:
    pixels = image.convert("RGB").load()
    data = bytearray()
    for y in range(image.height):
        for x in range(image.width):
            value = rgb565(*pixels[x, y])
            data += value.to_bytes(2, "little")
    destination.write_bytes(data)


def prepare_sprites(source: Path, out_dir: Path) -> None:
    sheet = Image.open(source).convert("RGBA")
    for index, (left, right) in enumerate(zip(CELL_BOUNDS, CELL_BOUNDS[1:])):
        cell = sheet.crop((left, 0, right, sheet.height))
        alpha = cell.getchannel("A")
        bbox = alpha.getbbox()
        if bbox is None:
            raise ValueError(f"sprite cell {index} is empty")
        sprite = cell.crop(bbox)
        scale = min(SPRITE_SIZE[0] / sprite.width, SPRITE_SIZE[1] / sprite.height)
        resized = sprite.resize(
            (max(1, round(sprite.width * scale)), max(1, round(sprite.height * scale))),
            Image.Resampling.NEAREST,
        )
        canvas = Image.new("RGBA", SPRITE_SIZE, SKY)
        canvas.alpha_composite(resized, ((SPRITE_SIZE[0] - resized.width) // 2,
                                         SPRITE_SIZE[1] - resized.height))
        canvas.convert("RGB").save(out_dir / f"leader_frame_{index}.png")
        write_rgb565(canvas, out_dir / f"leader_frame_{index}.rgb565")


def prepare_audio(source: Path, out_dir: Path) -> None:
    with wave.open(str(source), "rb") as wav:
        if (wav.getnchannels(), wav.getsampwidth(), wav.getframerate()) != (1, 2, 16000):
            raise ValueError("voice asset must be mono, 16-bit, 16 kHz PCM")
        (out_dir / "leader_apology.pcm").write_bytes(wav.readframes(wav.getnframes()))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--sheet", type=Path, required=True)
    parser.add_argument("--voice", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    prepare_sprites(args.sheet, args.out)
    prepare_audio(args.voice, args.out)


if __name__ == "__main__":
    main()
