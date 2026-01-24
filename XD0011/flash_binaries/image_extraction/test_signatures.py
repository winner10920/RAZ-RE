#!/usr/bin/env python3
"""
Test specific signatures from known good images against random data.
This helps distinguish real images from garbage.
"""

import numpy as np
from pathlib import Path

def rgb565_to_rgb888(data):
    if len(data) % 2 != 0:
        data = data[:-1]
    rgb565 = np.frombuffer(data, dtype='>u2')
    r = ((rgb565 >> 11) & 0x1F) * 255 // 31
    g = ((rgb565 >> 5) & 0x3F) * 255 // 63
    b = (rgb565 & 0x1F) * 255 // 31
    return np.stack([r, g, b], axis=-1).astype(np.uint8)

def test_rgb565_pattern(data):
    """Check if data looks like valid RGB565 values"""
    if len(data) < 100:
        return False
    
    # RGB565 values should have reasonable color distribution
    # Not all values will be 0xFFFF or 0x0000
    rgb565 = np.frombuffer(data[:100], dtype='>u2')
    
    # Check for diversity in values
    unique_ratio = len(np.unique(rgb565)) / len(rgb565)
    
    # Check if values look like colors (not sequential counters)
    max_val = rgb565.max()
    min_val = rgb565.min()
    
    # Real images have mixed RGB components, not just maxed out channels
    r_vals = (rgb565 >> 11) & 0x1F
    g_vals = (rgb565 >> 5) & 0x3F
    b_vals = rgb565 & 0x1F
    
    r_diversity = len(np.unique(r_vals)) / len(r_vals)
    g_diversity = len(np.unique(g_vals)) / len(g_vals)
    b_diversity = len(np.unique(b_vals)) / len(b_vals)
    
    print(f"  Unique ratio: {unique_ratio:.3f}")
    print(f"  Value range: {min_val:#06x} - {max_val:#06x}")
    print(f"  R diversity: {r_diversity:.3f}, G: {g_diversity:.3f}, B: {b_diversity:.3f}")
    
    # Real images should have at least some diversity in each channel
    if r_diversity < 0.05 or g_diversity < 0.05 or b_diversity < 0.05:
        return False
    
    return True

def main():
    file_path = Path('onboard_flash_dumpF2.binF')
    with open(file_path, 'rb') as f:
        data = f.read()
    
    # Test known good image
    print("Testing known good image (Battery Icon at 0x0A000):")
    good_data = data[0x0A000:0x0A000 + 40*40*2]
    test_rgb565_pattern(good_data)
    
    # Test garbage data
    print("\nTesting garbage data (0x20300 which was detected as false positive):")
    bad_data = data[0x20300:0x20300 + 40*40*2]
    test_rgb565_pattern(bad_data)
    
    # Test another garbage spot
    print("\nTesting garbage data (0x20400):")
    bad_data2 = data[0x20400:0x20400 + 40*40*2]
    test_rgb565_pattern(bad_data2)
    
    # Test actual flame animation (0x1E4B0)
    print("\nTesting known good animation (Flame at 0x1E4B0):")
    good_data2 = data[0x1E4B0:0x1E4B0 + 120*32*2]
    test_rgb565_pattern(good_data2)

if __name__ == "__main__":
    main()
