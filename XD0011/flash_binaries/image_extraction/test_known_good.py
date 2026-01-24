#!/usr/bin/env python3
"""Quick test to see why known good images aren't being detected"""
import sys
sys.path.insert(0, '/home/cody/RAZ-RE/XD0011/flash_binaries')
from extract_images import try_extract_image, is_valid_rgb565_data

# Test known good flame animation
with open('/home/cody/RAZ-RE/XD0011/flash_binaries/onboard_flash_dumpF2.binF', 'rb') as f:
    data = f.read()

# Known good from binF.js: Flame at 0x1E4B0, 120x32
offset = 0x1E4B0
width = 120
height = 32

print(f"Testing known good image: Flame at 0x{offset:05X}, {width}x{height}")

# Test RGB565 validation
img_data = data[offset:offset + width*height*2]
is_valid = is_valid_rgb565_data(img_data)
print(f"RGB565 validation: {'PASS' if is_valid else 'FAIL'}")

# Test extraction
score, img = try_extract_image(data, offset, width, height)
print(f"Extraction score: {score:.3f}")
print(f"Image extracted: {'YES' if img is not None else 'NO'}")

if score < 0.12:
    print(f"⚠️  Score {score:.3f} is below threshold 0.12")
