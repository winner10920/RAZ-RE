#!/usr/bin/env python3
"""
Analyze known good images to understand their signatures for better detection.
Uses the binF.js structure as ground truth.
"""

import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
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

def analyze_image_characteristics(img_array, name):
    """Analyze and print characteristics of an image"""
    print(f"\n=== {name} ===")
    print(f"Dimensions: {img_array.shape}")
    
    # Entropy
    unique_pixels = len(np.unique(img_array.reshape(-1, 3), axis=0))
    total_pixels = img_array.shape[0] * img_array.shape[1]
    color_diversity = unique_pixels / total_pixels
    print(f"Color diversity: {color_diversity:.3f} ({unique_pixels}/{total_pixels} unique)")
    
    # Variance
    variance = np.var(img_array)
    print(f"Variance: {variance:.1f}")
    
    # Row correlation
    if img_array.shape[0] > 1:
        correlations = []
        for i in range(min(10, img_array.shape[0] - 1)):
            row1 = img_array[i, :].flatten().astype(float)
            row2 = img_array[i + 1, :].flatten().astype(float)
            if np.std(row1) > 0 and np.std(row2) > 0:
                corr = np.corrcoef(row1, row2)[0, 1]
                if not np.isnan(corr):
                    correlations.append(corr)
        if correlations:
            print(f"Avg row correlation: {np.mean(correlations):.3f} (n={len(correlations)})")
    
    # Border analysis
    if img_array.shape[0] > 2 and img_array.shape[1] > 2:
        top_var = np.var(img_array[0, :])
        bottom_var = np.var(img_array[-1, :])
        left_var = np.var(img_array[:, 0])
        right_var = np.var(img_array[:, -1])
        content_var = np.var(img_array[1:-1, 1:-1])
        
        print(f"Border variance: T={top_var:.1f}, B={bottom_var:.1f}, L={left_var:.1f}, R={right_var:.1f}")
        print(f"Content variance: {content_var:.1f}")
        print(f"Border/Content ratio: {np.mean([top_var, bottom_var, left_var, right_var]) / (content_var + 1):.3f}")
    
    # Edge strength (gradient)
    grad_y = np.abs(np.diff(np.mean(img_array, axis=2), axis=0))
    grad_x = np.abs(np.diff(np.mean(img_array, axis=2), axis=1))
    
    if img_array.shape[0] > 2:
        edge_top = np.mean(grad_y[0:2, :])
        edge_bottom = np.mean(grad_y[-2:, :])
    else:
        edge_top = edge_bottom = 0
        
    if img_array.shape[1] > 2:
        edge_left = np.mean(grad_x[:, 0:2])
        edge_right = np.mean(grad_x[:, -2:])
    else:
        edge_left = edge_right = 0
    
    print(f"Edge gradients: T={edge_top:.1f}, B={edge_bottom:.1f}, L={edge_left:.1f}, R={edge_right:.1f}")
    
    # Spatial coherence
    avg_grad = (np.mean(grad_y) + np.mean(grad_x)) / 2
    print(f"Avg spatial gradient: {avg_grad:.1f}")
    
    # FFT analysis
    gray = np.mean(img_array, axis=2)
    fft = np.fft.fft2(gray)
    fft_shift = np.fft.fftshift(fft)
    magnitude = np.abs(fft_shift)
    
    cy, cx = magnitude.shape[0] // 2, magnitude.shape[1] // 2
    center_energy = magnitude[cy-2:cy+3, cx-2:cx+3].sum()
    total_energy = magnitude.sum()
    center_ratio = center_energy / total_energy
    
    print(f"FFT center energy ratio: {center_ratio:.4f}")
    
    # Check for discontinuity
    vertical_line = magnitude[:, cx]
    horizontal_line = magnitude[cy, :]
    v_strength = np.mean(vertical_line) / (np.mean(magnitude) + 1e-6)
    h_strength = np.mean(horizontal_line) / (np.mean(magnitude) + 1e-6)
    print(f"FFT discontinuity: V={v_strength:.2f}, H={h_strength:.2f}")
    
    return {
        'name': name,
        'shape': img_array.shape,
        'color_diversity': color_diversity,
        'variance': variance,
        'row_correlation': np.mean(correlations) if correlations else None,
        'border_content_ratio': np.mean([top_var, bottom_var, left_var, right_var]) / (content_var + 1) if content_var > 0 else None,
        'edge_strength': np.mean([edge_top, edge_bottom, edge_left, edge_right]),
        'spatial_gradient': avg_grad,
        'fft_center_ratio': center_ratio,
        'fft_discontinuity': max(v_strength, h_strength)
    }

def main():
    # Known images from binF.js structure
    known_images = [
        {'offset': 0x00000, 'width': 128, 'height': 160, 'name': 'Background'},
        {'offset': 0x0A000, 'width': 40, 'height': 40, 'name': 'Battery_Icon_1'},
        {'offset': 0x0ABE0, 'width': 40, 'height': 40, 'name': 'Battery_Icon_2'},
        {'offset': 0x0F320, 'width': 40, 'height': 40, 'name': 'Juice_Icon_1'},
        {'offset': 0x14642, 'width': 75, 'height': 80, 'name': 'Normal_Mode'},
        {'offset': 0x17522, 'width': 75, 'height': 80, 'name': 'Boost_Mode'},
        {'offset': 0x1A400, 'width': 43, 'height': 36, 'name': 'Moon_Icon'},
        {'offset': 0x1B018, 'width': 43, 'height': 36, 'name': 'Sun_Icon'},
        {'offset': 0x1BC30, 'width': 72, 'height': 36, 'name': '12W_Icon'},
        {'offset': 0x1D070, 'width': 72, 'height': 36, 'name': '24W_Icon'},
        {'offset': 0x1E4B0, 'width': 120, 'height': 32, 'name': 'Flame_Frame_1'},
        {'offset': 0x202B0, 'width': 120, 'height': 32, 'name': 'Flame_Frame_2'},
        {'offset': 0x54AB0, 'width': 120, 'height': 32, 'name': 'Flame2_Frame_1'},
        {'offset': 0x568B0, 'width': 120, 'height': 32, 'name': 'Flame2_Frame_2'},
        {'offset': 0x8B0B0, 'width': 42, 'height': 68, 'name': 'Unknown_Bg_Frame_1'},
        {'offset': 0x8C700, 'width': 42, 'height': 68, 'name': 'Unknown_Bg_Frame_2'},
    ]
    
    file_path = Path('onboard_flash_dumpF2.binF')
    if not file_path.exists():
        file_path = Path('onboard_flash_dump.binF')
    
    if not file_path.exists():
        print("Error: Could not find binF file")
        return 1
    
    print(f"Analyzing known good images from {file_path}")
    print("=" * 60)
    
    with open(file_path, 'rb') as f:
        data = f.read()
    
    all_stats = []
    
    for img_info in known_images:
        offset = img_info['offset']
        width = img_info['width']
        height = img_info['height']
        name = img_info['name']
        
        bytes_needed = width * height * 2
        
        if offset + bytes_needed > len(data):
            print(f"\nSkipping {name}: extends beyond file")
            continue
        
        img_data = data[offset:offset + bytes_needed]
        rgb888 = rgb565_to_rgb888(img_data)
        
        if len(rgb888) != width * height:
            print(f"\nSkipping {name}: wrong size")
            continue
        
        img_array = rgb888.reshape(height, width, 3)
        stats = analyze_image_characteristics(img_array, name)
        all_stats.append(stats)
    
    # Summary statistics
    print("\n" + "=" * 60)
    print("SUMMARY STATISTICS")
    print("=" * 60)
    
    print("\nColor Diversity:")
    diversities = [s['color_diversity'] for s in all_stats]
    print(f"  Range: {min(diversities):.3f} - {max(diversities):.3f}")
    print(f"  Mean: {np.mean(diversities):.3f}")
    
    print("\nVariance:")
    variances = [s['variance'] for s in all_stats]
    print(f"  Range: {min(variances):.1f} - {max(variances):.1f}")
    print(f"  Mean: {np.mean(variances):.1f}")
    
    print("\nRow Correlation:")
    corrs = [s['row_correlation'] for s in all_stats if s['row_correlation'] is not None]
    print(f"  Range: {min(corrs):.3f} - {max(corrs):.3f}")
    print(f"  Mean: {np.mean(corrs):.3f}")
    
    print("\nBorder/Content Ratio:")
    ratios = [s['border_content_ratio'] for s in all_stats if s['border_content_ratio'] is not None]
    print(f"  Range: {min(ratios):.3f} - {max(ratios):.3f}")
    print(f"  Mean: {np.mean(ratios):.3f}")
    
    print("\nEdge Strength:")
    edges = [s['edge_strength'] for s in all_stats]
    print(f"  Range: {min(edges):.1f} - {max(edges):.1f}")
    print(f"  Mean: {np.mean(edges):.1f}")
    
    print("\nSpatial Gradient:")
    grads = [s['spatial_gradient'] for s in all_stats]
    print(f"  Range: {min(grads):.1f} - {max(grads):.1f}")
    print(f"  Mean: {np.mean(grads):.1f}")
    
    print("\nFFT Center Energy Ratio:")
    fft_centers = [s['fft_center_ratio'] for s in all_stats]
    print(f"  Range: {min(fft_centers):.4f} - {max(fft_centers):.4f}")
    print(f"  Mean: {np.mean(fft_centers):.4f}")
    
    print("\nFFT Discontinuity:")
    fft_discs = [s['fft_discontinuity'] for s in all_stats]
    print(f"  Range: {min(fft_discs):.2f} - {max(fft_discs):.2f}")
    print(f"  Mean: {np.mean(fft_discs):.2f}")
    
    print("\n" + "=" * 60)
    print("RECOMMENDATIONS FOR DETECTION THRESHOLDS:")
    print("=" * 60)
    print(f"Variance: Accept > {min(variances) * 0.5:.1f}")
    print(f"Row Correlation: Accept > {min(corrs) * 0.9:.3f}")
    print(f"FFT Discontinuity: Reject if > {max(fft_discs) * 1.5:.2f}")
    print(f"Spatial Gradient: Accept < {max(grads) * 1.2:.1f}")
    
    return 0

if __name__ == "__main__":
    exit(main())
