#!/usr/bin/env python3
"""Arte propio del port (nada del juego original): icono 48x48, banner 256x128 y sonido del banner.
Uso: tools/.venv/bin/python tools/make_icon.py   (escribe port/meta/icon.png, banner.png, banner.wav)
Necesita Pillow. La fuente es la del sistema (Arial Black en macOS); solo se guarda la imagen resultante.
"""
import math
import os
import struct
import wave

from PIL import Image, ImageDraw, ImageFilter, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
META = os.path.join(HERE, "..", "port", "meta")
FONT = "/System/Library/Fonts/Supplemental/Arial Black.ttf"


def ball(size, cx, cy, r):
    """Bola roja con sombreado radial y brillo (dibujada a escala size)."""
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    px = img.load()
    lx, ly, lz = -0.45, -0.55, 0.70   # luz arriba a la izquierda
    for y in range(size):
        for x in range(size):
            dx, dy = (x - cx) / r, (y - cy) / r
            d2 = dx * dx + dy * dy
            if d2 > 1:
                continue
            nz = math.sqrt(1 - d2)
            diff = max(0.0, dx * lx + dy * ly + nz * lz)
            spec = max(0.0, (dx * lx + dy * ly + nz * lz)) ** 24
            rim = (1 - nz) ** 3
            red = min(255, int(90 + 165 * diff + 255 * spec))
            green = min(255, int(10 + 40 * diff + 255 * spec - 20 * rim))
            blue = min(255, int(15 + 30 * diff + 255 * spec))
            alpha = 255 if d2 < 0.97 else int(255 * (1 - d2) / 0.03)
            px[x, y] = (red, max(0, green), blue, alpha)
    return img


def icon():
    S = 48 * 8
    img = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # sombra
    d.ellipse((S * 0.18, S * 0.82, S * 0.82, S * 0.94), fill=(0, 0, 0, 70))
    img = img.filter(ImageFilter.GaussianBlur(S * 0.02))
    img.alpha_composite(ball(S, S * 0.5, S * 0.46, S * 0.38))
    # etiqueta "3D"
    d = ImageDraw.Draw(img)
    font = ImageFont.truetype(FONT, int(S * 0.30))
    box = (S * 0.50, S * 0.58, S * 0.98, S * 0.92)
    d.rounded_rectangle(box, radius=S * 0.07, fill=(255, 214, 0, 255), outline=(60, 40, 0, 255), width=int(S * 0.025))
    d.text(((box[0] + box[2]) / 2, (box[1] + box[3]) / 2 + S * 0.01), "3D", font=font, fill=(60, 40, 0, 255), anchor="mm")
    return img.resize((48, 48), Image.LANCZOS)


def banner():
    W, H, K = 256, 128, 4
    img = Image.new("RGBA", (W * K, H * K))
    d = ImageDraw.Draw(img)
    for y in range(H * K):   # cielo
        t = y / (H * K)
        d.line([(0, y), (W * K, y)], fill=(int(90 + 100 * t), int(170 + 60 * t), 255, 255))
    pts = [(x, H * K * (0.78 - 0.10 * math.sin(x / (W * K) * math.pi * 1.3 + 0.4))) for x in range(0, W * K + 1, 8)]
    d.polygon(pts + [(W * K, H * K), (0, H * K)], fill=(70, 185, 60, 255))   # colina
    d.line(pts, fill=(40, 140, 40, 255), width=K * 2)
    for i, a in enumerate((60, 100, 150)):   # estela
        x = W * K * (0.10 + 0.07 * i)
        d.ellipse((x - 14 * K, H * K * 0.50 - 14 * K, x + 14 * K, H * K * 0.50 + 14 * K), fill=(255, 255, 255, a))
    img.alpha_composite(ball(W * K, W * K * 0.36, H * K * 0.48, 30 * K))
    font = ImageFont.truetype(FONT, 26 * K)
    small = ImageFont.truetype(FONT, 15 * K)
    tx = W * K * 0.70
    for dx, dy, col in ((3 * K, 3 * K, (40, 40, 90, 255)), (0, 0, (255, 255, 255, 255))):
        d.text((tx + dx, H * K * 0.30 + dy), "BOUNCE", font=font, fill=col, anchor="mm")
        d.text((tx + dx, H * K * 0.53 + dy), "TOUCH", font=font, fill=col, anchor="mm")
    d.rounded_rectangle((tx - 34 * K, H * K * 0.66, tx + 34 * K, H * K * 0.84), radius=6 * K, fill=(255, 214, 0, 255))
    d.text((tx, H * K * 0.75), "3D DEMO", font=small, fill=(60, 40, 0, 255), anchor="mm")
    return img.resize((W, H), Image.LANCZOS)


def boing(path, rate=22050, secs=1.2):
    """Sonido del banner sintetizado: un 'boing' (seno con tono que cae y vibrato)."""
    frames = []
    for i in range(int(rate * secs)):
        t = i / rate
        f = 180 + 520 * math.exp(-t * 5) + 25 * math.sin(2 * math.pi * 9 * t)
        env = math.exp(-t * 3.2) * min(1.0, t * 80)
        frames.append(struct.pack("<h", int(12000 * env * math.sin(2 * math.pi * f * t))))
    with wave.open(path, "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(b"".join(frames))


if __name__ == "__main__":
    os.makedirs(META, exist_ok=True)
    icon().save(os.path.join(META, "icon.png"))
    banner().save(os.path.join(META, "banner.png"))
    boing(os.path.join(META, "banner.wav"))
    print("ok: port/meta/icon.png, banner.png, banner.wav")
