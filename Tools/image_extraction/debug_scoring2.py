#!/usr/bin/env python3
"""Debug later checks that might be failing"""
import numpy as np
import sys
sys.path.insert(0, '/home/cody/RAZ-RE/XD0011/flash_binaries')
from extract_images import (rgb565_to_rgb888, detect_borders, detect_dimension_errors, 
                             detect_fft_discontinuity)

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

# Border detection
border_score = detect_borders(img_array)
print(f"Border score: {border_score:.3f}")

# Dimension errors
dim_error, row_corr = detect_dimension_errors(img_array)
print(f"Dimension error: {dim_error:.3f}, Row correlation: {row_corr:.3f}")
if dim_error > 0.5:
    print(f"  ❌ FAIL: Dimension error {dim_error:.3f} > 0.5")

# FFT discontinuity
has_disc, disc_strength, axis = detect_fft_discontinuity(img_array)
print(f"FFT discontinuity: {has_disc}, strength={disc_strength:.2f}, axis={axis}")
if has_disc and disc_strength > 35.0:
    print(f"  ❌ FAIL: FFT discontinuity {disc_strength:.2f} > 35.0")

# Calculate final score components
entropy_score = np.clip((3.729 - 1.5) / 7.5, 0, 1) if 3.729 > 0.5 else 0.01
variance_score = np.clip(8308.2 / 500.0, 0, 1)
dimension_quality = 1.0 - dim_error
coherence = 1.0 / (1.0 + 14.5 / 10.0)

img_size = width * height
border_weight = 0.7 if img_size < 2000 else 0.5

final_score = entropy_score * coherence * variance_score * (1.0 - border_weight + border_weight * border_score) * dimension_quality

print(f"\nScore components:")
print(f"  Entropy score: {entropy_score:.3f}")
print(f"  Coherence: {coherence:.3f}")
print(f"  Variance score: {variance_score:.3f}")
print(f"  Border component: {(1.0 - border_weight + border_weight * border_score):.3f}")
print(f"  Dimension quality: {dimension_quality:.3f}")
print(f"  FINAL SCORE: {final_score:.3f}")
