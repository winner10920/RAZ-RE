#!/usr/bin/env python3
"""
Automatically extract RGB565 images from binary files and create GIFs from sequences.
Uses entropy analysis and image similarity to detect and group images.
"""

import os
import argparse
import numpy as np
from PIL import Image
from pathlib import Path
from collections import defaultdict
import hashlib
import warnings
warnings.filterwarnings('ignore')

def rgb565_to_rgb888(data):
    """Convert RGB565 bytes to RGB888 numpy array"""
    # Ensure even number of bytes
    if len(data) % 2 != 0:
        data = data[:-1]
    
    # RGB565 is stored as big-endian: first byte is high byte, second is low byte
    rgb565 = np.frombuffer(data, dtype='>u2')  # big-endian uint16
    
    # Extract RGB components
    r = ((rgb565 >> 11) & 0x1F) * 255 // 31
    g = ((rgb565 >> 5) & 0x3F) * 255 // 63
    b = (rgb565 & 0x1F) * 255 // 31
    
    return np.stack([r, g, b], axis=-1).astype(np.uint8)

def calculate_entropy(data):
    """Calculate Shannon entropy of data"""
    if len(data) == 0:
        return 0
    
    # Get byte value frequencies
    _, counts = np.unique(data, return_counts=True)
    probabilities = counts / len(data)
    
    # Calculate entropy
    entropy = -np.sum(probabilities * np.log2(probabilities + 1e-10))
    return entropy

def detect_edge_lines(img_array):
    """
    Detect horizontal and vertical edge lines using gradient analysis.
    Returns line strength score (0-1) where higher means stronger edge lines.
    """
    if img_array.shape[0] < 10 or img_array.shape[1] < 10:
        return 0.0
    
    # Convert to grayscale for edge detection
    if len(img_array.shape) == 3:
        gray = np.mean(img_array, axis=2)
    else:
        gray = img_array
    
    # Calculate gradients
    grad_y = np.abs(np.diff(gray, axis=0))
    grad_x = np.abs(np.diff(gray, axis=1))
    
    # Look for strong horizontal lines (high gradient in vertical direction)
    # Check top and bottom edges
    top_edge_strength = np.mean(grad_y[0:2, :]) if grad_y.shape[0] > 2 else 0
    bottom_edge_strength = np.mean(grad_y[-2:, :]) if grad_y.shape[0] > 2 else 0
    
    # Look for strong vertical lines (high gradient in horizontal direction)
    # Check left and right edges
    left_edge_strength = np.mean(grad_x[:, 0:2]) if grad_x.shape[1] > 2 else 0
    right_edge_strength = np.mean(grad_x[:, -2:]) if grad_x.shape[1] > 2 else 0
    
    # Average edge strength
    avg_edge_strength = np.mean([top_edge_strength, bottom_edge_strength, 
                                  left_edge_strength, right_edge_strength])
    
    # Normalize to 0-1 range (typical edge strength is 0-50 for good borders)
    edge_score = np.clip(avg_edge_strength / 30.0, 0, 1)
    
    return edge_score

def detect_borders(img_array):
    """
    Detect if image has distinct borders (black or solid color lines).
    Returns border score (0-1) where higher means clearer borders.
    """
    if img_array.shape[0] < 4 or img_array.shape[1] < 4:
        return 0.0
    
    # Check top and bottom rows for low variance (solid lines)
    top_row_var = np.var(img_array[0, :])
    bottom_row_var = np.var(img_array[-1, :])
    
    # Check left and right columns
    left_col_var = np.var(img_array[:, 0])
    right_col_var = np.var(img_array[:, -1])
    
    # Border pixels should be more uniform than image content
    content_var = np.var(img_array[2:-2, 2:-2]) if img_array.shape[0] > 4 and img_array.shape[1] > 4 else np.var(img_array)
    
    # Score based on how much more uniform borders are vs content
    border_vars = [top_row_var, bottom_row_var, left_col_var, right_col_var]
    avg_border_var = np.mean(border_vars)
    
    if content_var < 10:  # Image too uniform overall
        return 0.0
    
    # Good borders are much more uniform than content
    uniformity_score = np.clip(1.0 - (avg_border_var / (content_var + 1.0)), 0, 1)
    
    # Combine with edge line detection
    edge_score = detect_edge_lines(img_array)
    
    # Weight both uniformity and edge strength
    border_score = uniformity_score * 0.6 + edge_score * 0.4
    
    return border_score

def detect_dimension_errors(img_array):
    """
    Detect if image has wrong dimensions by looking for artifacts.
    Wrong width causes horizontal banding/"venetian blind" effect.
    Returns (error_score, correlation) where error_score 0-1 (higher=worse), correlation is avg row correlation.
    """
    if img_array.shape[0] < 10 or img_array.shape[1] < 10:
        return 0.0, 1.0
    
    # Calculate correlation between adjacent rows
    # Correct dimensions: high correlation between adjacent rows
    # Wrong dimensions: low correlation (venetian blind effect)
    row_correlations = []
    for i in range(0, min(img_array.shape[0] - 1, 20)):  # Check first 20 rows
        row1 = img_array[i, :].flatten().astype(float)
        row2 = img_array[i + 1, :].flatten().astype(float)
        std1, std2 = np.std(row1), np.std(row2)
        
        if std1 > 5 and std2 > 5:  # Skip uniform rows
            try:
                corr = np.corrcoef(row1, row2)[0, 1]
                if not np.isnan(corr):
                    row_correlations.append(corr)
            except:
                continue
    
    if len(row_correlations) == 0:
        return 0.0, 1.0
    
    avg_correlation = np.mean(row_correlations)
    
    # Low correlation suggests wrong dimensions
    # Good images: correlation > 0.5
    # Wrong dimensions: correlation < 0.3
    if avg_correlation < 0.3:
        return 1.0 - avg_correlation / 0.3, avg_correlation
    elif avg_correlation < 0.5:
        return 0.5 - avg_correlation * 0.5, avg_correlation
    
    return 0.0, avg_correlation

def calculate_image_score(img_array):
    """
    Score how likely data is to be a real image.
    Real images have moderate entropy, spatial coherence, and often have borders.
    """
    # Calculate entropy
    entropy = calculate_entropy(img_array.flatten())
    
    # Real images typically have entropy between 2-9 bits (relaxed based on analysis)
    # Note: Background image has very low entropy (0.0), others range 4-8
    if entropy < 1.5 or entropy > 9.0:
        return 0.0
    
    # Calculate spatial coherence (neighboring pixels should be similar)
    diff_h = np.abs(np.diff(img_array.astype(np.int16), axis=0)).mean()
    diff_v = np.abs(np.diff(img_array.astype(np.int16), axis=1)).mean()
    avg_diff = (diff_h + diff_v) / 2
    
    # Real images have gradients up to ~25 (based on analysis showing max 18.3)
    if avg_diff > 30:  # Too noisy
        return 0.0
    
    coherence = 1.0 / (1.0 + avg_diff / 10.0)
    
    # Check if image has reasonable color distribution (not all one color)
    color_variance = np.var(img_array)
    # Analysis shows min variance of 64 for real images (except solid background)
    if color_variance < 50:  # Too uniform (but allow near-solid backgrounds)
        return 0.0
    
    # Check for repeating patterns (common in garbage data)
    # Sample first and last rows
    if img_array.shape[0] > 10:
        first_rows = img_array[:5].flatten()
        last_rows = img_array[-5:].flatten()
        if len(first_rows) == len(last_rows):
            try:
                pattern_similarity = np.corrcoef(first_rows, last_rows)[0, 1]
                if not np.isnan(pattern_similarity) and pattern_similarity > 0.95:
                    return 0.0
            except:
                pass
    
    # Detect borders (includes edge line detection)
    border_score = detect_borders(img_array)
    
    # Detect dimension errors (wrong width/height causing skew)
    dimension_error, row_corr = detect_dimension_errors(img_array)
    
    # Check FFT for discontinuity (indicates wrong offset/split image)
    has_discontinuity, discontinuity_strength, _ = detect_fft_discontinuity(img_array)
    
    # Heavy penalty for dimension errors or FFT discontinuities
    # Analysis shows real images have FFT discontinuity up to 20 (flames), icons 6-11
    # Only reject if extremely high (> 30) indicating actual split
    if dimension_error > 0.5 or (has_discontinuity and discontinuity_strength > 35.0):
        return 0.0
    
    # Combined score
    # Adjusted entropy normalization for wider range
    entropy_score = np.clip((entropy - 1.5) / 7.5, 0, 1) if entropy > 0.5 else 0.01
    variance_score = np.clip(color_variance / 500.0, 0, 1)
    dimension_quality = 1.0 - dimension_error
    
    # For smaller images, weight borders more heavily (they're more critical for detection)
    img_size = img_array.shape[0] * img_array.shape[1]
    if img_size < 2000:  # Small images (< ~45x45)
        border_weight = 0.7
    else:
        border_weight = 0.5
    
    # Weight border detection and dimension quality heavily
    return entropy_score * coherence * variance_score * (1.0 - border_weight + border_weight * border_score) * dimension_quality

def is_valid_rgb565_data(data, min_sample=50):
    """
    Quick check if data looks like valid RGB565 image data (not garbage).
    Returns True if data passes basic validity checks.
    """
    if len(data) < min_sample * 2:
        return False
    
    # Sample multiple regions (start, middle, end) for better validation
    sample_size = min(min_sample * 2, len(data) // 3)
    sample1 = data[:sample_size]
    sample2 = data[len(data)//2 - sample_size//2:len(data)//2 + sample_size//2] if len(data) > sample_size*2 else data[:sample_size]
    sample3 = data[-sample_size:] if len(data) > sample_size else data[:sample_size]
    
    for sample in [sample1, sample2, sample3]:
        if len(sample) < 20:
            continue
            
        rgb565 = np.frombuffer(sample, dtype='>u2')
        
        # Check 1: Not all zeros or all 0xFFFF
        if np.all(rgb565 == 0) or np.all(rgb565 == 0xFFFF):
            return False
        
        # Check 2: Unique values ratio (real images: 0.20-0.78 for battery icons)
        unique_ratio = len(np.unique(rgb565)) / len(rgb565)
        if unique_ratio < 0.15:  # Strengthened threshold
            return False
        
        # Check 3: Extract RGB components
        r_vals = (rgb565 >> 11) & 0x1F
        g_vals = (rgb565 >> 5) & 0x3F  
        b_vals = rgb565 & 0x1F
        
        # Check 4: Each channel needs reasonable diversity
        # Analysis shows: R 0.08-0.18, G 0.14-0.64, B 0.12-0.30
        r_unique_ratio = len(np.unique(r_vals)) / len(r_vals)
        g_unique_ratio = len(np.unique(g_vals)) / len(g_vals)
        b_unique_ratio = len(np.unique(b_vals)) / len(b_vals)
        
        # At least GREEN channel must have good diversity (it has highest in real images)
        if g_unique_ratio < 0.10:
            return False
        
        # At least 2 of 3 channels must have some diversity
        diversity_count = sum([r_unique_ratio >= 0.06, g_unique_ratio >= 0.06, b_unique_ratio >= 0.06])
        if diversity_count < 2:
            return False
    
    # Check 5: Not sequential data (like memory addresses or counters)
    rgb565_full = np.frombuffer(data[:min(200, len(data))], dtype='>u2')
    diffs = np.abs(np.diff(rgb565_full.astype(np.int32)))
    if len(diffs) > 10:
        avg_diff = np.mean(diffs)
        std_diff = np.std(diffs)
        # If differences are very uniform, it's probably not image data
        if std_diff < 10 and 0 < avg_diff < 20:
            return False
    
    return True

def try_extract_image(data, offset, width, height, create_image=False):
    """
    Try to extract an image at given offset with given dimensions.
    Returns (score, image) or (0, None) if invalid.
    
    Args:
        create_image: If False, only returns score without creating PIL Image (saves memory)
    """
    bytes_needed = width * height * 2  # RGB565 = 2 bytes per pixel
    
    if offset + bytes_needed > len(data):
        return 0, None
    
    try:
        img_data = data[offset:offset + bytes_needed]
        
        # Quick pre-filter: check if data looks like valid RGB565 before expensive scoring
        if not is_valid_rgb565_data(img_data, min_sample=min(50, width * height // 10)):
            return 0, None
        
        rgb888 = rgb565_to_rgb888(img_data)
        
        if len(rgb888) != width * height:
            return 0, None
        
        img_array = rgb888.reshape(height, width, 3)
        score = calculate_image_score(img_array)
        
        if score < 0.06:  # With strong RGB565 validation, we can use lower threshold
            return 0, None
        
        # Only create PIL Image if requested (Pass 2), not during Pass 1 scan
        img = Image.fromarray(img_array, 'RGB') if create_image else None
        return score, img
    except Exception:
        return 0, None

def find_animation_sequences(found_images, max_gap=0x2000):
    """
    Find potential animation sequences by looking for regularly-spaced similar images.
    Returns list of sequences, where each sequence is a list of image indices.
    """
    sequences = []
    used = set()
    
    for i, img_info in enumerate(found_images):
        if i in used:
            continue
        
        sequence = [i]
        expected_stride = None
        last_offset = img_info['offset']
        
        # Look for images with same dimensions at regular intervals
        for j in range(i + 1, len(found_images)):
            if j in used:
                continue
            
            next_img = found_images[j]
            
            # Must have same dimensions
            if (img_info['width'] != next_img['width'] or 
                img_info['height'] != next_img['height']):
                continue
            
            offset_diff = next_img['offset'] - last_offset
            
            # Skip if gap is too large
            if offset_diff > max_gap:
                break
            
            # First frame pair - establish stride
            if expected_stride is None:
                expected_stride = offset_diff
                sequence.append(j)
                used.add(j)
                last_offset = next_img['offset']
            # Subsequent frames - check if stride matches (allow 10% tolerance)
            elif abs(offset_diff - expected_stride) < expected_stride * 0.1:
                sequence.append(j)
                used.add(j)
                last_offset = next_img['offset']
            else:
                # Stride changed, stop sequence
                break
        
        # Mark first image as used if it's part of a sequence
        if len(sequence) > 1:
            used.add(i)
        
        sequences.append(sequence)
    
    return sequences

def refine_dimensions_from_sequence(data, base_offset, width, height, count, stride):
    """
    Try to refine image dimensions by testing variations around the initial guess.
    Uses all frames in sequence to validate dimensions.
    Also tries nearby offsets in case the image doesn't start exactly where detected.
    """
    best_score = 0
    best_dims = (width, height)
    best_offset = base_offset
    
    # Limit search space for very large images to prevent memory issues
    img_size = width * height
    if img_size >= 10000:
        # For large images (>= 100x100), use minimal search space to prevent segfaults
        width_variations = [0, -1, 1]
        height_variations = [0, -1, 1]
        offset_variations = [0, -2, 2]
    else:
        # Normal search space for smaller images
        width_variations = [0, -1, 1, -2, 2, -3, 3, -4, 4, -5, 5, -6, 6, -8, 8, -10, 10, -12, 12, -16, 16]
        height_variations = [0, -1, 1, -2, 2, -3, 3, -4, 4, -6, 6]
        offset_variations = [0, -2, 2, -4, 4, -8, 8, -16, 16, -32, 32]
    
    test_count = 0
    max_tests = 500  # Hard limit to prevent infinite loops
    
    for offset_delta in offset_variations:
        test_offset = base_offset + offset_delta
        
        if test_offset < 0:
            continue
        
        for dw in width_variations:
            for dh in height_variations:
                if test_count >= max_tests:
                    print(f"    Reached test limit ({max_tests}), stopping refinement")
                    return best_offset, best_dims, best_score
                
                test_w = width + dw
                test_h = height + dh
                
                # Enforce size limits: min 20x20, max 128x160
                if test_w < 20 or test_h < 20 or test_w > 128 or test_h > 160:
                    continue
                
                # Test first 3 frames with these dimensions
                scores = []
                dimension_errors = []
                row_correlations = []
                
                frames_to_test = min(3, count)
                for i in range(frames_to_test):
                    frame_offset = test_offset + (i * stride)
                    
                    try:
                        score, img = try_extract_image(data, frame_offset, test_w, test_h, create_image=True)
                    except Exception as e:
                        # Skip this combination on error
                        break
                    
                    if score > 0 and img is not None:
                        try:
                            # Check for dimension errors
                            img_array = np.array(img)
                            dim_error, row_corr = detect_dimension_errors(img_array)
                            
                            # Check for FFT discontinuity
                            has_disc, disc_strength, _ = detect_fft_discontinuity(img_array)
                            
                            # Penalize discontinuities
                            discontinuity_penalty = disc_strength / 5.0 if has_disc else 0.0
                            
                            scores.append(score)
                            dimension_errors.append(dim_error + discontinuity_penalty)
                            row_correlations.append(row_corr)
                        except Exception as e:
                            # Skip frame on error
                            continue
                
                # Calculate averages for this dimension combination
                if len(scores) == 0:
                    continue
                    
                avg_score = np.mean(scores)
                avg_dim_error = np.mean(dimension_errors)
                avg_row_corr = np.mean(row_correlations)
                
                # Heavily penalize dimension errors, reward high row correlation
                adjusted_score = avg_score * (1.0 - avg_dim_error) * (0.3 + 0.7 * avg_row_corr)
                
                if adjusted_score > best_score:
                    best_score = adjusted_score
                    best_dims = (test_w, test_h)
                    best_offset = test_offset
                    
                    # If we found dimensions with great correlation, we can stop early
                    if avg_row_corr > 0.9 and avg_dim_error < 0.1 and best_score > 0.15:
                        print(f"    Found excellent match: {test_w}x{test_h} at offset +{offset_delta}, "
                              f"row_corr={avg_row_corr:.3f}, score={adjusted_score:.3f}")
                        return best_offset, best_dims, best_score
                
                test_count += 1
                # Show progress for long searches
                if test_count % 500 == 0:
                    print(f"    Tested {test_count} combinations, best so far: {best_dims[0]}x{best_dims[1]} "
                          f"(score={best_score:.3f})")
    
    # Report if we found a better offset
    if best_offset != base_offset:
        offset_diff = best_offset - base_offset
        print(f"    Adjusted offset by {offset_diff:+d} bytes")
    
    return best_offset, best_dims, best_score

def scan_for_images(data, max_width=128, max_height=160, min_score=0.06):
    """
    Scan binary data for images at various offsets and sizes.
    Returns list of (offset, width, height, score, image).
    Max dimensions: 128x160 (nothing larger exists in flash).
    Min dimensions: 20x20 (nothing smaller exists).
    """
    common_sizes = [
        (128, 160), (120, 160), (128, 128),  # Full screen sizes
        (120, 32), (72, 36), (75, 80),        # Common UI elements
        (43, 36), (40, 40), (42, 68),         # Icons
        (36, 36), (32, 32), (30, 30),         # Medium icons
        (24, 24), (20, 20),                   # Small icons
    ]
    
    found_images = []
    checked_regions = set()
    
    print(f"Scanning {len(data)} bytes for images (Pass 1: Coarse detection)...")
    
    # Pass 1: Try common sizes at aligned offsets
    # Use 0x100 (256) byte alignment, typical for flash storage
    # Also try 0x1000 (4KB) boundaries which are more common for image starts
    scan_offsets = []
    
    # Priority 1: 4KB boundaries (typical for image starts)
    for offset in range(0, len(data), 0x1000):
        scan_offsets.append(offset)
    
    # Priority 2: 256-byte boundaries between 4KB marks (for smaller images)
    for offset in range(0, len(data), 0x100):
        if offset % 0x1000 != 0:  # Don't duplicate 4KB boundaries
            scan_offsets.append(offset)
    
    scan_offsets.sort()
    
    for scan_idx, offset in enumerate(scan_offsets):
        if scan_idx % 64 == 0:  # Update progress less frequently
            print(f"  Progress: {offset / len(data) * 100:.1f}%")
        
        for width, height in common_sizes:
            bytes_needed = width * height * 2
            img_size = width * height
            
            # Skip very large images during Pass 1 to prevent numpy segfaults
            if img_size > 15000:  # Skip images > ~120x125 pixels during initial scan
                continue
            
            # Skip if we've already found an image overlapping this region
            region_key = (offset // 2048, width, height)
            #if region_key in checked_regions:
                #continue
            
            # Don't create PIL Image during Pass 1 scan to avoid memory issues
            score, img = try_extract_image(data, offset, width, height, create_image=False)
            
            if score >= min_score:
                # Don't store PIL Image during scan to avoid memory issues/segfaults
                # We'll re-extract when needed
                found_images.append({
                    'offset': offset,
                    'width': width,
                    'height': height,
                    'score': score,
                    'image': None,  # Don't keep image in memory during scan
                    'size': bytes_needed
                })
                checked_regions.add(region_key)
                print(f"  Found image at 0x{offset:05X}: {width}x{height}, score={score:.3f}")
                break
    
    print(f"\nPass 1 complete. Found {len(found_images)} candidates.")
    
    # Sort by offset
    try:
        found_images.sort(key=lambda x: x['offset'])
        print(f"Sorted images by offset.")
    except Exception as e:
        print(f"Warning: Sort failed: {e}")
        return []
    
    # Deduplication: Remove images that are too close together (likely false positives)
    # Real images are spaced at least 0x800 (2KB) apart, animations have regular strides
    print(f"\nDeduplicating {len(found_images)} candidates...")
    
    try:
        deduplicated = []
        min_spacing = 0x800  # Minimum 2KB between different images
        
        for img in found_images:
            # Check if this overlaps or is too close to any already accepted image
            too_close = False
            for accepted in deduplicated:
                offset_diff = abs(img['offset'] - accepted['offset'])
                
                # If images overlap or are suspiciously close
                if offset_diff < min_spacing:
                    # Keep the one with higher score
                    if img['score'] > accepted['score'] + 0.02:  # Significant improvement
                        deduplicated.remove(accepted)
                    else:
                        too_close = True
                        break
            
            if not too_close:
                deduplicated.append(img)
        
        print(f"  Kept {len(deduplicated)} after deduplication")
        found_images = deduplicated
    except Exception as e:
        print(f"Warning: Deduplication failed: {e}")
        # Continue with all images
    
    # Pass 2: Find animation sequences and refine dimensions
    print("\nPass 2: Finding animation sequences...")
    
    try:
        sequences = find_animation_sequences(found_images)
    except Exception as e:
        print(f"  Warning: Animation sequence detection failed: {e}")
        print(f"  Falling back to treating all images as static...")
        sequences = [[i] for i in range(len(found_images))]
    
    refined_images = []
    for seq_idx, seq in enumerate(sequences):
        try:
            if len(seq) <= 1:
                # Single image - re-extract it (wasn't stored during Pass 1)
                img_info = found_images[seq[0]]
                score, img = try_extract_image(data, img_info['offset'], img_info['width'], img_info['height'], create_image=True)
                if img:
                    img_info['image'] = img  # Store the extracted image
                    img_info['score'] = max(score, img_info['score'])
                refined_images.append(img_info)
                continue
            
            # Multiple images - this is likely an animation sequence
            first_img = found_images[seq[0]]
            
            # Skip very large images that might cause segfault during PIL operations
            img_size = first_img['width'] * first_img['height']
            if img_size > 15000:  # Skip sequences with images > ~120x125 pixels
                print(f"  Sequence {seq_idx}: Skipping large sequence ({first_img['width']}x{first_img['height']}, {img_size} pixels) to prevent segfault")
                sys.stdout.flush()
                # Process as individual images instead
                for i in seq:
                    img_info = found_images[i]
                    if img_info['image'] is None:
                        try:
                            score, img = try_extract_image(data, img_info['offset'], img_info['width'], img_info['height'], create_image=True)
                            if img:
                                img_info['image'] = img
                        except:
                            pass
                    if img_info['image']:
                        refined_images.append(img_info)
                continue
            
            # Calculate stride
            if len(seq) > 1:
                stride = found_images[seq[1]]['offset'] - first_img['offset']
            else:
                stride = first_img['size']
            
            print(f"  Sequence {seq_idx}: {len(seq)} frames at 0x{first_img['offset']:05X}, "
                  f"{first_img['width']}x{first_img['height']}, stride=0x{stride:X}")
            sys.stdout.flush()  # Force output before potential crash
            
            # Analyze color patterns to validate animation sequence
            print(f"    Starting color pattern analysis...")
            sys.stdout.flush()
            try:
                is_animation, pattern_score, suggested_stride = analyze_color_pattern_repetition(
                    data, first_img['offset'], first_img['width'], first_img['height'], 
                    len(seq), stride
                )
                print(f"    Color pattern analysis complete")
                sys.stdout.flush()
                
                if is_animation:
                    print(f"    Color pattern analysis: animation confirmed (score={pattern_score:.3f})")
                    if suggested_stride != stride:
                        print(f"    Suggested stride adjustment: 0x{stride:X} -> 0x{suggested_stride:X}")
                        stride = suggested_stride
            except Exception as e:
                print(f"    Warning: Color pattern analysis failed: {e}")
                sys.stdout.flush()
            
            # Try to refine dimensions using the sequence (includes offset adjustment)
            # Skip refinement for medium/large images to avoid memory issues and segfaults
            img_size = first_img['width'] * first_img['height']
            print(f"    Checking if refinement needed (size={img_size})...")
            sys.stdout.flush()
            if img_size >= 6500:  # Skip refinement for images >= ~80x80 to prevent numpy segfaults
                print(f"    Skipping refinement (image size {img_size} pixels may cause segfault)")
                sys.stdout.flush()
                refined_offset = first_img['offset']
                refined_dims = (first_img['width'], first_img['height'])
                refined_score = first_img['score']
            else:
                print(f"    Starting refinement...")
                sys.stdout.flush()
                try:
                    refined_offset, refined_dims, refined_score = refine_dimensions_from_sequence(
                        data, first_img['offset'], first_img['width'], first_img['height'], 
                        len(seq), stride
                    )
                except Exception as e:
                    print(f"    Warning: Refinement failed: {e}")
                    refined_offset = first_img['offset']
                    refined_dims = (first_img['width'], first_img['height'])
                    refined_score = first_img['score']
            
            if refined_dims != (first_img['width'], first_img['height']) or refined_offset != first_img['offset']:
                print(f"    Refined to {refined_dims[0]}x{refined_dims[1]} "
                      f"at 0x{refined_offset:05X} (score: {refined_score:.3f})")
            
            # Re-extract all frames with refined dimensions and offset
            print(f"    Re-extracting {len(seq)} frames...")
            sys.stdout.flush()
            offset_delta = refined_offset - first_img['offset']
            
            for i in seq:
                try:
                    img_info = found_images[i]
                    adjusted_offset = img_info['offset'] + offset_delta
                    print(f"      Extracting frame {i} at 0x{adjusted_offset:05X}...")
                    sys.stdout.flush()
                    new_score, new_img = try_extract_image(
                        data, adjusted_offset, refined_dims[0], refined_dims[1], create_image=True
                    )
                    print(f"      Frame {i} extracted successfully")
                    sys.stdout.flush()
                    
                    if new_img:
                        refined_images.append({
                            'offset': adjusted_offset,
                            'width': refined_dims[0],
                            'height': refined_dims[1],
                            'score': max(new_score, img_info['score']),
                            'image': new_img,
                            'size': refined_dims[0] * refined_dims[1] * 2
                        })
                    else:
                        # Fallback to original - but need to extract image first
                        if img_info['image'] is None:
                            score, img = try_extract_image(data, img_info['offset'], img_info['width'], img_info['height'], create_image=True)
                            if img:
                                img_info['image'] = img
                                img_info['score'] = max(score, img_info['score'])
                        refined_images.append(img_info)
                except Exception as e:
                    print(f"      Warning: Failed to extract frame {i}: {e}")
                    sys.stdout.flush()
                    # Try fallback
                    img_info = found_images[i]
                    if img_info['image'] is None:
                        try:
                            score, img = try_extract_image(data, img_info['offset'], img_info['width'], img_info['height'], create_image=True)
                            if img:
                                img_info['image'] = img
                        except:
                            pass
                    if img_info['image']:
                        refined_images.append(img_info)
        except Exception as e:
            print(f"  Warning: Failed to process sequence {seq_idx}: {e}")
            # Add original images from this sequence as fallback (extract if needed)
            for i in seq:
                img_info = found_images[i]
                if img_info['image'] is None:
                    score, img = try_extract_image(data, img_info['offset'], img_info['width'], img_info['height'], create_image=True)
                    if img:
                        img_info['image'] = img
                        img_info['score'] = max(score, img_info['score'])
                refined_images.append(img_info)
    
    # Remove overlapping images (keep highest scoring one)
    filtered = []
    for img_info in refined_images:
        overlap = False
        for existing in filtered:
            # Check if regions overlap
            start1, end1 = img_info['offset'], img_info['offset'] + img_info['size']
            start2, end2 = existing['offset'], existing['offset'] + existing['size']
            
            if not (end1 <= start2 or start1 >= end2):
                # Overlapping - keep higher score
                if img_info['score'] > existing['score']:
                    filtered.remove(existing)
                    break
                else:
                    overlap = True
                    break
        
        if not overlap:
            filtered.append(img_info)
    
    print(f"\nFound {len(filtered)} non-overlapping images")
    return filtered

def calculate_image_similarity(img1, img2):
    """Calculate similarity between two PIL images (0=different, 1=identical)"""
    if img1.size != img2.size:
        return 0.0
    
    arr1 = np.array(img1)
    arr2 = np.array(img2)
    
    # Calculate normalized mean squared error
    mse = np.mean((arr1.astype(float) - arr2.astype(float)) ** 2)
    max_mse = 255.0 ** 2
    
    # Convert to similarity (1 = identical, 0 = completely different)
    similarity = 1.0 - (mse / max_mse)
    return similarity

def group_similar_images(images, similarity_threshold=0.80):
    """
    Group images by similarity to create animation sequences.
    Uses stride-based grouping for more reliable sequence detection.
    Returns list of groups, where each group is a list of image indices.
    """
    groups = []
    used = set()
    
    for i, img_info in enumerate(images):
        if i in used:
            continue
        
        # Start new group
        group = [i]
        used.add(i)
        
        # Find similar consecutive images with consistent stride
        last_offset = img_info['offset']
        expected_stride = None
        
        for j in range(i + 1, len(images)):
            if j in used:
                continue
            
            next_img = images[j]
            
            # Only group if same dimensions
            if (img_info['width'] != next_img['width'] or 
                img_info['height'] != next_img['height']):
                continue
            
            # Check offset stride
            offset_diff = next_img['offset'] - last_offset
            
            if expected_stride is None:
                # Allow reasonable animation frame spacing (up to 0x3000 bytes)
                if offset_diff < 0x3000:
                    expected_stride = offset_diff
                    group.append(j)
                    used.add(j)
                    last_offset = next_img['offset']
            elif abs(offset_diff - expected_stride) < max(expected_stride * 0.15, 256):
                # Allow 15% stride variation or 256 bytes, whichever is larger
                group.append(j)
                used.add(j)
                last_offset = next_img['offset']
            else:
                # Stride changed significantly, stop sequence
                break
        
        groups.append(group)
    
    return groups
    
    return groups

def create_gif(images, output_path, duration=100):
    """Create GIF from list of PIL images"""
    if len(images) == 0:
        return
    
    images[0].save(
        output_path,
        save_all=True,
        append_images=images[1:],
        duration=duration,
        loop=0
    )

def analyze_color_pattern_repetition(data, offset, width, height, num_frames=5, stride=None):
    """
    Analyze RGB565 color patterns to detect animation sequences.
    Animations have repeating color patterns across frames.
    Returns (is_animation, pattern_score, suggested_stride)
    """
    try:
        bytes_per_frame = width * height * 2
        
        if stride is None:
            # Try common strides
            test_strides = [bytes_per_frame, bytes_per_frame + 0x100, bytes_per_frame + 0x200]
        else:
            test_strides = [stride]
        
        best_pattern_score = 0
        best_stride = bytes_per_frame
        
        for test_stride in test_strides:
            # Extract color histograms from multiple frames
            histograms = []
            
            for i in range(min(num_frames, 3)):
                try:
                    frame_offset = offset + (i * test_stride)
                    if frame_offset + bytes_per_frame > len(data):
                        break
                    
                    frame_data = data[frame_offset:frame_offset + bytes_per_frame]
                    
                    # Parse as RGB565 (2 bytes per pixel)
                    if len(frame_data) % 2 == 0:
                        rgb565 = np.frombuffer(frame_data, dtype='>u2')
                        
                        # Create histogram of color values (smaller bins to reduce memory)
                        hist, _ = np.histogram(rgb565, bins=128, range=(0, 65536))
                        histograms.append(hist)
                except Exception:
                    continue
            
            if len(histograms) < 2:
                continue
            
            # Calculate correlation between histograms
            try:
                correlations = []
                for i in range(len(histograms) - 1):
                    corr = np.corrcoef(histograms[i], histograms[i+1])[0, 1]
                    if not np.isnan(corr):
                        correlations.append(corr)
                
                if correlations:
                    pattern_score = np.mean(correlations)
                    if pattern_score > best_pattern_score:
                        best_pattern_score = pattern_score
                        best_stride = test_stride
            except Exception:
                continue
        
        # High correlation (>0.7) suggests animation sequence
        is_animation = best_pattern_score > 0.7
        return is_animation, best_pattern_score, best_stride
    except Exception as e:
        # Return safe defaults on any error
        return False, 0.0, width * height * 2
    is_animation = best_pattern_score > 0.7
    
    return is_animation, best_pattern_score, best_stride

def detect_fft_discontinuity(img_array):
    """
    Detect FFT signature of split/misaligned image.
    Strong vertical or horizontal line through center indicates discontinuity.
    Returns (has_discontinuity, strength, axis) where axis is 'vertical' or 'horizontal'
    """
    try:
        if len(img_array.shape) == 3:
            gray = np.mean(img_array, axis=2)
        else:
            gray = img_array
        
        # Compute 2D FFT
        fft = np.fft.fft2(gray)
        fft_shift = np.fft.fftshift(fft)
        magnitude = np.abs(fft_shift)
        
        # Get center coordinates
        cy, cx = magnitude.shape[0] // 2, magnitude.shape[1] // 2
        
        # Check vertical line through center (indicates horizontal discontinuity)
        vertical_line = magnitude[:, cx]
        vertical_strength = np.mean(vertical_line) / (np.mean(magnitude) + 1e-6)
        
        # Check horizontal line through center (indicates vertical discontinuity)
        horizontal_line = magnitude[cy, :]
        horizontal_strength = np.mean(horizontal_line) / (np.mean(magnitude) + 1e-6)
        
        # Strong line indicates discontinuity (threshold > 2.0)
        vertical_discontinuity = vertical_strength > 2.0
        horizontal_discontinuity = horizontal_strength > 2.0
        
        if vertical_discontinuity or horizontal_discontinuity:
            if vertical_strength > horizontal_strength:
                return True, vertical_strength, 'vertical'
            else:
                return True, horizontal_strength, 'horizontal'
        
        return False, 0.0, None
    except:
        return False, 0.0, None

def generate_fft_visualization(img):
    """Generate FFT visualization of image to show frequency patterns"""
    try:
        # Convert to grayscale
        img_array = np.array(img)
        if len(img_array.shape) == 3:
            gray = np.mean(img_array, axis=2)
        else:
            gray = img_array
        
        # Compute 2D FFT
        fft = np.fft.fft2(gray)
        fft_shift = np.fft.fftshift(fft)
        magnitude = np.abs(fft_shift)
        
        # Log scale for better visualization
        magnitude_log = np.log(magnitude + 1)
        
        # Normalize to 0-255
        magnitude_norm = ((magnitude_log - magnitude_log.min()) / 
                         (magnitude_log.max() - magnitude_log.min()) * 255).astype(np.uint8)
        
        # Create image
        fft_img = Image.fromarray(magnitude_norm, mode='L')
        return fft_img
    except:
        return None

def generate_markdown_report(image_groups, output_dir, binary_filename):
    """Generate markdown file showcasing all images, GIFs, and FFT analysis"""
    md_path = output_dir / "report.md"
    
    with open(md_path, 'w') as f:
        f.write(f"# Image Extraction Report\n\n")
        f.write(f"**Source File:** `{binary_filename}`\n\n")
        f.write(f"**Total Images Found:** {sum(len(g['indices']) for g in image_groups)}\n")
        f.write(f"**Animation Sequences:** {sum(1 for g in image_groups if len(g['indices']) > 1)}\n")
        f.write(f"**Static Images:** {sum(1 for g in image_groups if len(g['indices']) == 1)}\n\n")
        f.write(f"**Note:** FFT (Fast Fourier Transform) visualizations show frequency patterns. ")
        f.write(f"Regular patterns indicate repeating structures, noise shows as scattered frequencies.\n\n")
        
        f.write("---\n\n")
        
        # Animation sequences
        animations = [g for g in image_groups if len(g['indices']) > 1]
        if animations:
            f.write(f"## Animation Sequences ({len(animations)})\n\n")
            
            for group_idx, group in enumerate(animations):
                info = group['info']
                f.write(f"### Animation {group_idx + 1}\n\n")
                f.write(f"- **Frames:** {len(group['indices'])}\n")
                f.write(f"- **Dimensions:** {info['width']}x{info['height']}\n")
                f.write(f"- **File Offsets:** 0x{info['offset']:05X} - 0x{group['last_offset']:05X}\n")
                f.write(f"- **Avg Score:** {group['avg_score']:.3f}\n")
                
                # Add discontinuity warning if detected
                if 'discontinuity_info' in group and group['discontinuity_info']:
                    f.write(f"- **⚠️ FFT Discontinuity Detected:** {group['discontinuity_info']}\n")
                
                f.write(f"\n")
                
                if group['gif_path']:
                    rel_path = os.path.relpath(group['gif_path'], output_dir)
                    f.write(f"![Animation {group_idx + 1}]({rel_path})\n\n")
                    
                    # Add FFT of first frame
                    if group['fft_paths'] and len(group['fft_paths']) > 0:
                        fft_rel_path = os.path.relpath(group['fft_paths'][0], output_dir)
                        f.write(f"**FFT Analysis (Frame 1):**\n\n")
                        f.write(f"![FFT]({fft_rel_path})\n\n")
                
                f.write("---\n\n")
        
        # Static images
        statics = [g for g in image_groups if len(g['indices']) == 1]
        if statics:
            f.write(f"## Static Images ({len(statics)})\n\n")
            
            for group_idx, group in enumerate(statics):
                info = group['info']
                f.write(f"### Image {group_idx + 1}\n\n")
                f.write(f"- **Dimensions:** {info['width']}x{info['height']}\n")
                f.write(f"- **File Offset:** 0x{info['offset']:05X}\n")
                f.write(f"- **Score:** {info['score']:.3f}\n\n")
                
                if group['image_paths']:
                    rel_path = os.path.relpath(group['image_paths'][0], output_dir)
                    f.write(f"![Image {group_idx + 1}]({rel_path})\n\n")
                    
                    # Add FFT
                    if group['fft_paths'] and len(group['fft_paths']) > 0:
                        fft_rel_path = os.path.relpath(group['fft_paths'][0], output_dir)
                        f.write(f"**FFT Analysis:**\n\n")
                        f.write(f"![FFT]({fft_rel_path})\n\n")
                
                f.write("---\n\n")
    
    print(f"\nMarkdown report saved to: {md_path}")

def main():
    parser = argparse.ArgumentParser(
        description='Extract RGB565 images from binary files and create GIFs from sequences. '
                    'Automatically detects and fixes skewed/misaligned images.'
    )
    parser.add_argument('input', help='Input binary file')
    parser.add_argument('-o', '--output', help='Output directory (default: input_name_extracted)')
    parser.add_argument('-s', '--min-score', type=float, default=0.06,
                       help='Minimum image quality score (0-1, default: 0.06)')
    parser.add_argument('-t', '--similarity', type=float, default=0.85,
                       help='Similarity threshold for grouping (0-1, default: 0.85)')
    parser.add_argument('-d', '--duration', type=int, default=100,
                       help='GIF frame duration in ms (default: 100)')
    
    args = parser.parse_args()
    
    # Setup paths
    input_path = Path(args.input)
    if not input_path.exists():
        print(f"Error: File not found: {input_path}")
        return 1
    
    if args.output:
        output_dir = Path(args.output)
    else:
        output_dir = input_path.parent / f"{input_path.stem}_extracted"
    
    output_dir.mkdir(exist_ok=True)
    
    # Read binary file
    print(f"Reading {input_path}...")
    with open(input_path, 'rb') as f:
        data = f.read()
    
    print(f"File size: {len(data)} bytes")
    
    # Scan for images (includes automatic dimension and offset refinement)
    images = scan_for_images(data, min_score=args.min_score)
    
    if len(images) == 0:
        print("No images found!")
        return 1
    
    # Group similar images
    print("\nGrouping similar images...")
    groups = group_similar_images(images, args.similarity)
    
    # Process groups
    image_groups = []
    
    for group_idx, group_indices in enumerate(groups):
        try:
            group_images = [images[i] for i in group_indices]
            first_img = group_images[0]
            
            # Calculate image size
            img_size = first_img['width'] * first_img['height']
            
            # Check for discontinuities in first image
            try:
                first_img_array = np.array(group_images[0]['image'])
                has_disc, disc_strength, disc_axis = detect_fft_discontinuity(first_img_array)
                discontinuity_info = None
                if has_disc:
                    discontinuity_info = f"{disc_axis} discontinuity (strength={disc_strength:.2f}), possible offset error"
            except Exception as e:
                print(f"  Warning: FFT discontinuity check failed: {e}")
                discontinuity_info = None
            
            group_info = {
                'indices': group_indices,
                'info': first_img,
                'avg_score': np.mean([img['score'] for img in group_images]),
                'last_offset': group_images[-1]['offset'],
                'gif_path': None,
                'image_paths': [],
                'fft_paths': [],
                'discontinuity_info': discontinuity_info
            }
            
            if len(group_indices) > 1:
            # Animation sequence
                # Skip GIF creation for very large images to prevent segfault
                if img_size > 10000:  # Skip GIF for images > 10000 pixels
                    print(f"  Skipping GIF creation for large sequence: {first_img['width']}x{first_img['height']} ({img_size} pixels)")
                    print(f"  Saving {len(group_images)} individual frames instead...")
                    
                    # Save individual frames instead
                    for frame_idx, img in enumerate(group_images):
                        try:
                            png_filename = f"animation_{group_idx:03d}_frame{frame_idx:02d}_{img['width']}x{img['height']}_0x{img['offset']:05X}.png"
                            png_path = output_dir / png_filename
                            img['image'].save(png_path)
                            group_info['image_paths'].append(png_path)
                        except Exception as e:
                            print(f"  Warning: Failed to save frame {frame_idx}: {e}")
                    
                    if group_info['image_paths']:
                        print(f"  Saved {len(group_info['image_paths'])} frames")
                else:
                    # Create GIF for smaller images
                    try:
                        gif_filename = f"animation_{group_idx:03d}_{first_img['width']}x{first_img['height']}.gif"
                        gif_path = output_dir / gif_filename
                        
                        pil_images = [img['image'] for img in group_images]
                        create_gif(pil_images, gif_path, duration=args.duration)
                        
                        group_info['gif_path'] = gif_path
                        print(f"Created GIF: {gif_filename} ({len(pil_images)} frames)")
                    except Exception as e:
                        print(f"  Warning: GIF creation failed: {e}")
                
                # Generate FFT for first frame
                try:
                    if img_size <= 10000:  # Only generate FFT for smaller images
                        fft_img = generate_fft_visualization(group_images[0]['image'])
                        if fft_img:
                            fft_filename = f"animation_{group_idx:03d}_fft.png"
                            fft_path = output_dir / fft_filename
                            fft_img.save(fft_path)
                            group_info['fft_paths'].append(fft_path)
                except Exception as e:
                    print(f"  Warning: FFT generation failed: {e}")
            else:
                # Single image - save as PNG
                try:
                    png_filename = f"image_{group_idx:03d}_{first_img['width']}x{first_img['height']}_0x{first_img['offset']:05X}.png"
                    png_path = output_dir / png_filename
                    
                    first_img['image'].save(png_path)
                    group_info['image_paths'] = [png_path]
                    print(f"Saved image: {png_filename}")
                    
                    # Generate FFT (skip for very large images)
                    if img_size <= 10000:
                        try:
                            fft_img = generate_fft_visualization(first_img['image'])
                            if fft_img:
                                fft_filename = f"image_{group_idx:03d}_fft.png"
                                fft_path = output_dir / fft_filename
                                fft_img.save(fft_path)
                                group_info['fft_paths'].append(fft_path)
                        except Exception as e:
                            print(f"  Warning: FFT generation failed: {e}")
                except Exception as e:
                    print(f"  Warning: Failed to save image: {e}")
            
            image_groups.append(group_info)
        except Exception as e:
            print(f"  Warning: Failed to process group {group_idx}: {e}")
    
    # Generate markdown report
    generate_markdown_report(image_groups, output_dir, input_path.name)
    
    print(f"\nExtraction complete! Output directory: {output_dir}")
    return 0

if __name__ == "__main__":
    exit(main())
