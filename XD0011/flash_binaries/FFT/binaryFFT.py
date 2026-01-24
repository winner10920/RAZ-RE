#!/usr/bin/env python3
"""
Binary FFT Analysis Tool
Reads a binary file, performs FFT analysis, and generates visualizations.
"""

import argparse
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import sys


def read_binary_file(filepath, bit_width=8, endian='little'):
    """Read binary file and convert to numpy array with specified bit width.
    
    Args:
        filepath: Path to binary file
        bit_width: Bit width (8, 16, 24, or 32)
        endian: Byte order ('little' or 'big')
    """
    try:
        with open(filepath, 'rb') as f:
            raw_data = f.read()
        
        if bit_width == 8:
            data = np.frombuffer(raw_data, dtype=np.uint8)
        elif bit_width == 16:
            dtype = np.dtype(np.uint16)
            dtype = dtype.newbyteorder('<' if endian == 'little' else '>')
            data = np.frombuffer(raw_data, dtype=dtype)
        elif bit_width == 24:
            # 24-bit has no native numpy type, need to process manually
            byte_array = np.frombuffer(raw_data, dtype=np.uint8)
            # Ensure length is multiple of 3
            length = (len(byte_array) // 3) * 3
            byte_array = byte_array[:length]
            
            if endian == 'little':
                # Combine 3 bytes: byte0 + byte1*256 + byte2*65536
                data = (byte_array[0::3].astype(np.uint32) + 
                        byte_array[1::3].astype(np.uint32) * 256 + 
                        byte_array[2::3].astype(np.uint32) * 65536)
            else:
                # Big endian: byte0*65536 + byte1*256 + byte2
                data = (byte_array[0::3].astype(np.uint32) * 65536 + 
                        byte_array[1::3].astype(np.uint32) * 256 + 
                        byte_array[2::3].astype(np.uint32))
        elif bit_width == 32:
            dtype = np.dtype(np.uint32)
            dtype = dtype.newbyteorder('<' if endian == 'little' else '>')
            data = np.frombuffer(raw_data, dtype=dtype)
        else:
            print(f"Error: Unsupported bit width {bit_width}")
            sys.exit(1)
        
        return data
    except Exception as e:
        print(f"Error reading file: {e}")
        sys.exit(1)


def perform_fft(data):
    """Perform FFT on the data."""
    # Convert to float and normalize
    data_float = data.astype(float) - np.mean(data)
    
    # Perform FFT
    fft_result = np.fft.fft(data_float)
    fft_freq = np.fft.fftfreq(len(data_float))
    
    # Get magnitude (power spectrum)
    fft_magnitude = np.abs(fft_result)
    
    return fft_freq, fft_magnitude, fft_result


def create_frequency_plot(fft_freq, fft_magnitude, output_path):
    """Create and save frequency domain plot."""
    plt.figure(figsize=(12, 6))
    
    # Only plot positive frequencies
    positive_freq_idx = fft_freq >= 0
    
    plt.subplot(1, 2, 1)
    plt.plot(fft_freq[positive_freq_idx], fft_magnitude[positive_freq_idx])
    plt.xlabel('Normalized Frequency')
    plt.ylabel('Magnitude')
    plt.title('FFT Magnitude Spectrum (Linear)')
    plt.grid(True)
    
    plt.subplot(1, 2, 2)
    plt.semilogy(fft_freq[positive_freq_idx], fft_magnitude[positive_freq_idx])
    plt.xlabel('Normalized Frequency')
    plt.ylabel('Magnitude (log scale)')
    plt.title('FFT Magnitude Spectrum (Log)')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Frequency plot saved to: {output_path}")
    plt.close()


def create_spectrogram(data, output_path, window_size=256):
    """Create and save spectrogram image."""
    plt.figure(figsize=(12, 8))
    
    # Create spectrogram
    plt.subplot(2, 1, 1)
    plt.specgram(data.astype(float) - np.mean(data), 
                 NFFT=window_size, 
                 Fs=1.0, 
                 noverlap=window_size//2,
                 cmap='viridis')
    plt.colorbar(label='Power (dB)')
    plt.xlabel('Sample Index')
    plt.ylabel('Normalized Frequency')
    plt.title('Spectrogram')
    
    # Also show raw data for reference
    plt.subplot(2, 1, 2)
    plt.plot(data[:min(1000, len(data))], linewidth=0.5)
    plt.xlabel('Sample Index')
    plt.ylabel('Value')
    plt.title('Raw Data (first 1000 samples)')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Spectrogram saved to: {output_path}")
    plt.close()


def create_2d_visualization(fft_magnitude, output_path, rows=256):
    """Create a 2D image visualization of FFT magnitude with multiple enhancement options."""
    # Use only positive frequencies (first half) and skip DC component
    # DC component (index 0) is often huge and dominates the visualization
    positive_half = fft_magnitude[1:len(fft_magnitude)//2]
    
    data_len = len(positive_half)
    cols = data_len // rows
    
    if cols > 0:
        # Trim to fit rectangle
        trimmed_data = positive_half[:rows * cols]
        magnitude_2d = trimmed_data.reshape(rows, cols)
        
        # Create figure with multiple subplots for different visualizations
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # 1. Linear scale with percentile clipping for contrast
        ax1 = axes[0, 0]
        vmin, vmax = np.percentile(magnitude_2d, [5, 95])
        im1 = ax1.imshow(magnitude_2d, aspect='auto', cmap='hot', 
                         interpolation='nearest', vmin=vmin, vmax=vmax)
        plt.colorbar(im1, ax=ax1, label='Magnitude')
        ax1.set_xlabel('Column Index')
        ax1.set_ylabel('Row Index')
        ax1.set_title('FFT Magnitude (Linear, 5-95 percentile clipped)')
        
        # 2. Log scale
        ax2 = axes[0, 1]
        magnitude_log = np.log10(magnitude_2d + 1)  # +1 to avoid log(0)
        im2 = ax2.imshow(magnitude_log, aspect='auto', cmap='viridis', 
                         interpolation='nearest')
        plt.colorbar(im2, ax=ax2, label='Log10(Magnitude + 1)')
        ax2.set_xlabel('Column Index')
        ax2.set_ylabel('Row Index')
        ax2.set_title('FFT Magnitude (Log Scale)')
        
        # 3. Normalized (0-1 range) with hot colormap
        ax3 = axes[1, 0]
        magnitude_norm = (magnitude_2d - magnitude_2d.min()) / (magnitude_2d.max() - magnitude_2d.min())
        im3 = ax3.imshow(magnitude_norm, aspect='auto', cmap='inferno', 
                         interpolation='nearest')
        plt.colorbar(im3, ax=ax3, label='Normalized Magnitude')
        ax3.set_xlabel('Column Index')
        ax3.set_ylabel('Row Index')
        ax3.set_title('FFT Magnitude (Normalized 0-1)')
        
        # 4. High contrast with adaptive histogram equalization approximation
        ax4 = axes[1, 1]
        # Use percentile-based adaptive scaling
        magnitude_sorted = np.sort(magnitude_2d.flatten())
        magnitude_enhanced = np.zeros_like(magnitude_2d)
        for i in range(rows):
            for j in range(cols):
                # Map value to its percentile rank
                val = magnitude_2d[i, j]
                percentile = np.searchsorted(magnitude_sorted, val) / len(magnitude_sorted)
                magnitude_enhanced[i, j] = percentile
        
        im4 = ax4.imshow(magnitude_enhanced, aspect='auto', cmap='plasma', 
                         interpolation='nearest')
        plt.colorbar(im4, ax=ax4, label='Percentile Rank')
        ax4.set_xlabel('Column Index')
        ax4.set_ylabel('Row Index')
        ax4.set_title('FFT Magnitude (Histogram Equalized)')
        
        plt.tight_layout()
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"2D visualization saved to: {output_path}")
        print(f"  Magnitude range: {magnitude_2d.min():.2f} - {magnitude_2d.max():.2f}")
        print(f"  Magnitude mean: {magnitude_2d.mean():.2f}, std: {magnitude_2d.std():.2f}")
        plt.close()
    else:
        print(f"Warning: Not enough data to create 2D visualization with {rows} rows")


def main():
    parser = argparse.ArgumentParser(
        description='Perform FFT analysis on binary file and generate visualizations'
    )
    parser.add_argument('input_file', 
                       type=str, 
                       help='Path to binary file')
    parser.add_argument('-o', '--output-prefix', 
                       type=str, 
                       default=None,
                       help='Output prefix for generated files (default: input filename)')
    parser.add_argument('-w', '--window-size', 
                       type=int, 
                       default=256,
                       help='Window size for spectrogram (default: 256)')
    parser.add_argument('-b', '--bit-width', 
                       type=int, 
                       choices=[8, 16, 24, 32],
                       default=8,
                       help='Bit width for interpreting binary data (default: 8)')
    parser.add_argument('-e', '--endian', 
                       type=str, 
                       choices=['little', 'big'],
                       default='little',
                       help='Byte order for multi-byte values (default: little)')
    parser.add_argument('--no-display', 
                       action='store_true',
                       help='Do not display plots, only save them')
    
    args = parser.parse_args()
    
    # Validate input file
    input_path = Path(args.input_file)
    if not input_path.exists():
        print(f"Error: File not found: {args.input_file}")
        sys.exit(1)
    
    # Set output prefix
    if args.output_prefix:
        output_prefix = args.output_prefix
    else:
        output_prefix = input_path.stem
    
    output_dir = input_path.parent
    
    print(f"Reading binary file: {input_path}")
    print(f"Bit width: {args.bit_width}-bit, Endianness: {args.endian}")
    data = read_binary_file(input_path, args.bit_width, args.endian)
    print(f"Data length: {len(data)} samples ({len(data) * args.bit_width // 8} bytes)")
    print(f"Data range: {data.min()} - {data.max()}")
    print(f"Data mean: {data.mean():.2f}")
    
    print("\nPerforming FFT analysis...")
    fft_freq, fft_magnitude, fft_result = perform_fft(data)
    
    # Generate outputs
    freq_plot_path = output_dir / f"{output_prefix}_fft_frequency.png"
    spectrogram_path = output_dir / f"{output_prefix}_fft_spectrogram.png"
    heatmap_path = output_dir / f"{output_prefix}_fft_heatmap.png"
    
    print("\nGenerating visualizations...")
    create_frequency_plot(fft_freq, fft_magnitude, freq_plot_path)
    create_spectrogram(data, spectrogram_path, args.window_size)
    create_2d_visualization(fft_magnitude, heatmap_path)
    
    print("\nAnalysis complete!")
    print(f"Generated files:")
    print(f"  - {freq_plot_path}")
    print(f"  - {spectrogram_path}")
    print(f"  - {heatmap_path}")


if __name__ == '__main__':
    main()
