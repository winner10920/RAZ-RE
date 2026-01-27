#!/usr/bin/env python3
"""Debug why known good image scores 0.0"""
import numpy as np
from pathlib import Path
import sys
sys.path.insert(0, '/home/cody/RAZ-RE/XD0011/flash_binaries')
from extract_images import rgb565_to_rgb888, calculate_entropy

# Test known good flame animation
with open('/home/cody/RAZ-RE/XD0011/flash_binaries/onboard_flash_dumpF2.binF', 'rb') as f:
    data = f.read()

offset = 0x1E4B0
width = 120
height = 32

img_data = data[offset:offset + width*height*2]
rgb888 = rgb565_to_rgb888(img_data)
img_array = rgb888.reshape(height, width, 3)

print(f"Testing Flame at 0x{offset:05X}, {width}x{height}")
print()

# Test each criterion from calculate_image_score
entropy = calculate_entropy(img_array.flatten())
print(f"1. Entropy: {entropy:.3f} (need: 1.5-9.0)")
if entropy < 1.5 or entropy > 9.0:
    print("   ❌ FAIL: Outside range")
else:
    print("   ✓ PASS")

diff_h = np.abs(np.diff(img_array.astype(np.int16), axis=0)).mean()
diff_v = np.abs(np.diff(img_array.astype(np.int16), axis=1)).mean()
avg_diff = (diff_h + diff_v) / 2
print(f"\n2. Spatial gradient: {avg_diff:.1f} (need: < 30)")
if avg_diff > 30:
    print("   ❌ FAIL: Too noisy")
else:
    print("   ✓ PASS")

color_variance = np.var(img_array)
print(f"\n3. Color variance: {color_variance:.1f} (need: >= 50)")
if color_variance < 50:
    print("   ❌ FAIL: Too uniform")
else:
    print("   ✓ PASS")

# Check pattern repetition
if img_array.shape[0] > 10:
    first_rows = img_array[:5].flatten()
    last_rows = img_array[-5:].flatten()
    if len(first_rows) == len(last_rows):
        try:
            pattern_similarity = np.corrcoef(first_rows, last_rows)[0, 1]
            print(f"\n4. Pattern repetition: {pattern_similarity:.3f} (need: < 0.95)")
            if not np.isnan(pattern_similarity) and pattern_similarity > 0.95:
                print("   ❌ FAIL: Too repetitive (first/last rows too similar)")
            else:
                print("   ✓ PASS")
        except:
            print("\n4. Pattern check: ERROR")
