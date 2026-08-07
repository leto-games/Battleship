#!/usr/bin/env python3
"""
Генерирует C-массивы uint8_t[] для SSD1306_Screen::Bitmap().

Формат буфера (page-major, SSD1306-native):
  Для bitmap ширины W и высоты H:
    pages = (H + 7) // 8
    data[x * pages + page] — байт для столбца x, страницы 'page'
    бит 0 = пиксель (x, page*8 + 0) — верхний
    бит 7 = пиксель (x, page*8 + 7) — нижний в странице

Поддержка: PNG, JPG и др. (через Pillow).
Изображение НЕ масштабируется — только обрезка/дополнение.
"""

import sys
import os
from PIL import Image
import math


def convert_image_to_bitmap(img_path, width=None, height=None, invert=False, dither=True, pad=True):
    """
    Возвращает: (data_bytes, actual_width, actual_height)
    """
    # --- Загрузка и подготовка изображения ---
    img = Image.open(img_path).convert('RGBA')
    orig_w, orig_h = img.size

    # Определяем целевые размеры
    target_w = width if width is not None else min(orig_w, 128)
    target_h = height if height is not None else min(orig_h, 64)

    # Обработка: обрезка или дополнение (без ресайза!)
    if pad:
        # Новое чёрно-белое изображение (фон = белый = 1)
        new_img = Image.new('1', (target_w, target_h), color=1)
        # Конвертируем исходное в 1-бит
        img_gray = img.convert('L')
        img_bw = img_gray.convert('1', dither=Image.FLOYDSTEINBERG if dither else Image.NONE)
        # Вставляем в левый верхний угол
        new_img.paste(img_bw, (0, 0))
        final_img = new_img
    else:
        # Обрезаем до target_w × target_h
        cropped = img.crop((0, 0, target_w, target_h))
        img_gray = cropped.convert('L')
        final_img = img_gray.convert('1', dither=Image.FLOYDSTEINBERG if dither else Image.NONE)

    if invert:
        final_img = Image.eval(final_img, lambda x: 255 - x)

    # --- Генерация page-major буфера ---
    pages = (target_h + 7) // 8
    data = bytearray(target_w * pages)

    for col in range(target_w):
        for page in range(pages):
            byte = 0
            for bit in range(8):  # bit 0 = top of page
                y = page * 8 + bit
                if y >= target_h:
                    break
                # В режиме '1': 0 = чёрный (пиксель включён)
                if final_img.getpixel((col, y)) == 0:
                    byte |= (1 << bit)  # bit 0 = y = page*8 + 0
            idx = col * pages + page
            data[idx] = byte

    return bytes(data), target_w, target_h

def generate_h_array(data, width, height, var_name, struct) -> list[str]:
    pages = (height + 7) // 8
    total_bytes = len(data)

    lines = []
    lines.append(f"// Bitmap {width}×{height} px ({pages} pages, {total_bytes} bytes)")
    if not struct:
        lines.append(f"extern uint8_t* {var_name};")
        lines.append(f"#define {var_name}_WIDTH  {width}")
        lines.append(f"#define {var_name}_HEIGHT {height}")
    else:
        lines.append(f"#include <Graphics/BitmapData.hpp>")
        lines.append(f"extern BitmapData {var_name};") 

    return lines


def generate_c_array(data, width, height, var_name, struct) -> list[str]:
    pages = (height + 7) // 8
    total_bytes = len(data)

    lines = []
    lines.append(f"// Bitmap {width}×{height} px ({pages} pages, {total_bytes} bytes)")
    lines.append(f"static uint8_t __static_{var_name}__[] = {{")

    # Группируем по столбцам для наглядности (по pages байт на столбец)
    for col in range(width):
        start = col * pages
        chunk = data[start:start + pages]
        if not chunk:
            continue
        # Форматируем байты в двоичном виде для удобства отладки
        bin_strs = [f"0b{b:08b}" for b in chunk]
        comment = f"  // col {col}"
        if col == 0:
            comment += " (top bits = rows 0..7, etc.)"
        lines.append(f"  {', '.join(bin_strs)}{',' if col < width - 1 else ''}{comment}")

    lines.append("};")

    if not struct:
        lines.append(f"uint8_t* {var_name} = __static_{var_name}__;")
    else:
        lines.append(f"#include <Graphics/BitmapData.hpp>")
        lines.append(f"BitmapData {var_name} {{ __static_{var_name}__, {width}, {height} }};") 

    return lines


# ======================
# CLI
# ======================
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 img2bitmap.py <image.png> [output.h] [--size WxH] [--invert] [--no-dither] [--no-pad]")
        print("Examples:")
        print("  python3 img2bitmap.py icon11.png --size 11x11")
        print("  python3 img2bitmap.py cursor.png cursor.h --size 8x12 --invert")
        sys.exit(1)

    all_paths = []

    # Parse args
    width = height = None
    invert = '--invert' in sys.argv
    struct = '--struct' in sys.argv
    dither = '--no-dither' not in sys.argv
    pad = '--no-pad' not in sys.argv

    # --size WxH
    for i, arg in enumerate(sys.argv):
        if arg.startswith('--size='):
            w, h = arg.split('=')[1].split('x')
            width, height = int(w), int(h)
        elif arg == '--size' and i + 1 < len(sys.argv):
            w, h = sys.argv[i + 1].split('x')
            width, height = int(w), int(h)

    # paths
    for arg in sys.argv[1:]:
        if arg.startswith('--'):
            break
        all_paths.append(arg)

    img_paths = all_paths[:-1]
    out_name = all_paths[-1]
    
    print(all_paths)

    try:
        h_code = []
        cpp_code = []
        sum_data = 0;
        
        for img_path in img_paths:
            data, w, h = convert_image_to_bitmap(
                img_path, width=width, height=height,
                invert=invert, dither=dither, pad=pad
            )
            var_name = os.path.splitext(os.path.basename(img_path))[0]
            var_name = ''.join(c if c.isalnum() else '_' for c in var_name).strip('_')
            if not var_name:
                var_name = "bitmap"

            h_code.append(f"// Format: page-major (SSD1306-native), LSB = top pixel in page")
            h_code.append(f"#include <stdint.h>")
            h_code.extend(generate_h_array(data, w, h, var_name, struct))
            
            cpp_code.append(f"// Format: page-major (SSD1306-native), LSB = top pixel in page")
            cpp_code.append(f"#include <stdint.h>")
            cpp_code.extend(generate_c_array(data, w, h, var_name, struct))
            
            sum_data += len(data)
            print(f"✅ {w}×{h} → {len(data)} bytes added")

        if out_name:
            with open(out_name + ".h", 'w', encoding='utf-8') as f:
                f.write("\n".join(h_code) + "\n")
            with open(out_name + ".cpp", 'w', encoding='utf-8') as f:
                f.write("\n".join(cpp_code) + "\n")
            print(f"✅ {sum_data} bytes → {out_name}")
        else:
            print(c_code)

    except Exception as e:
        print(f"❌ Error: {e}", file=sys.stderr)
        raise