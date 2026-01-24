#!/usr/bin/env python3
"""
Brute-force find the best offset and dimensions for an image by trying many combinations.
"""

import argparse
import numpy as np
from PIL import Image
from pathlib import Path

def rgb565_to_rgb888(data):
    """Convert RGB565 bytes to RGB888 numpy array"""
    if len(data) % 2 != 0:
        data = data[:-1]
    
    rgb565 = np.frombuffer(data, dtype='>u2')
    r = ((rgb565 >> 11) & 0x1F) * 255 // 31
    g = ((rgb565 >> 5) & 0x3F) * 255 // 63
    b = (rgb565 & 0x1F) * 255 // 31
    
    return np.stack([r, g, b], axis=-1).astype(np.uint8)

def score_image(img_array):
    """Score image quality - higher is better"""
    if img_array.shape[0] < 5 or img_array.shape[1] < 5:
        return 0.0
    
    # Row correlation (adjacent rows should be similar)
    correlations = []
    for i in range(min(img_array.shape[0] - 1, 20)):
        row1 = img_array[i, :].flatten().astype(float)
        row2 = img_array[i + 1, :].flatten().astype(float)
        
        if np.std(row1) > 5 and np.std(row2) > 5:
            corr = np.corrcoef(row1, row2)[0, 1]
            if not np.isnan(corr):
                correlations.append(corr)
    
    if len(correlations) == 0:
        return 0.0
    
    row_corr = np.mean(correlations)
    
    # Spatial coherence (smooth gradients)
    diff_h = np.abs(np.diff(img_array.astype(np.int16), axis=0)).mean()
    diff_v = np.abs(np.diff(img_array.astype(np.int16), axis=1)).mean()
    coherence = 1.0 / (1.0 + (diff_h + diff_v) / 20.0)
    
    # Variance (not too uniform)
    variance = np.var(img_array)
    var_score = np.clip(variance / 500.0, 0, 1)
    
    # Combined score
    return row_corr * 0.7 + coherence * 0.2 + var_score * 0.1

def try_extract(data, offset, width, height):
    """Try to extract image at given parameters"""
    bytes_needed = width * height * 2
    
    if offset + bytes_needed > len(data) or offset < 0:
        return None, 0.0
    
    try:
        img_data = data[offset:offset + bytes_needed]
        rgb888 = rgb565_to_rgb888(img_data)
        
        if len(rgb888) != width * height:
            return None, 0.0
        
        img_array = rgb888.reshape(height, width, 3)
        score = score_image(img_array)
        
        if score > 0:
            return Image.fromarray(img_array, 'RGB'), score
    except Exception:
        pass
    
    return None, 0.0

def main():
    parser = argparse.ArgumentParser(description='Find best offset and dimensions for an image')
    parser.add_argument('input', help='Input binary file')
    parser.add_argument('offset', help='Approximate hex offset (e.g., 0x26600)')
    parser.add_argument('width', type=int, help='Approximate width')
    parser.add_argument('height', type=int, help='Approximate height')
    parser.add_argument('-r', '--range', type=int, default=512,
                       help='Offset search range in bytes (default: 512)')
    parser.add_argument('-o', '--output', help='Output image file')
    
    args = parser.parse_args()
    
    # Parse offset
    base_offset = int(args.offset, 16) if args.offset.startswith('0x') else int(args.offset)
    
    # Read binary file
    with open(args.input, 'rb') as f:
        data = f.read()
    
    print(f"Searching around offset 0x{base_offset:05X}")
    print(f"Base dimensions: {args.width}x{args.height}")
    print(f"Offset range: ±{args.range} bytes")
    print(f"Dimension range: ±20 pixels\n")
    
    best_score = 0
    best_params = None
    best_img = None
    
    candidates = []
    
    # Try offsets (align to 2-byte boundaries)
    for offset_delta in range(-args.range, args.range + 1, 2):
        test_offset = base_offset + offset_delta
        
        # Try dimensions
        for dw in range(-20, 21):
            test_w = args.width + dw
            if test_w < 10 or test_w > 200:
                continue
            
            for dh in range(-10, 11):
                test_h = args.height + dh
                if test_h < 10 or test_h > 200:
                    continue
                
                img, score = try_extract(data, test_offset, test_w, test_h)
                
                if score > best_score:
                    best_score = score
                    best_params = (test_offset, test_w, test_h)
                    best_img = img
                
                if score > 0.7:  # Good candidate
                    candidates.append((test_offset, test_w, test_h, score))
    
    if not best_img:
        print("No valid image found!")
        return 1
    
    # Show results
    print("Top 10 candidates:")
    candidates.sort(key=lambda x: x[3], reverse=True)
    for i, (off, w, h, score) in enumerate(candidates[:10]):
        offset_diff = off - base_offset
        marker = " <-- BEST" if (off, w, h) == best_params else ""
        print(f"  {i+1}. Offset: 0x{off:05X} ({offset_diff:+d} bytes), "
              f"Size: {w}x{h}, Score: {score:.3f}{marker}")
    
    if best_params:
        off, w, h = best_params
        offset_diff = off - base_offset
        print(f"\nBest match:")
        print(f"  Offset: 0x{off:05X} ({offset_diff:+d} bytes from base)")
        print(f"  Dimensions: {w}x{h}")
        print(f"  Score: {best_score:.3f}")
        
        output_path = args.output or f"best_{w}x{h}_0x{off:05X}.png"
        best_img.save(output_path)
        print(f"\nSaved to: {output_path}")
    
    return 0

if __name__ == "__main__":
    exit(main())
