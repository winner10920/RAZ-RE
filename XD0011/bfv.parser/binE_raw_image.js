/**
 * Raw Binary Image Viewer for .binE files
 * Converts entire binary into RGB565 image with adjustable width
 * Allows measurement and extraction of image coordinates
 */
registerFileType((fileExt, filePath, fileData) => {
    if (fileExt == 'binE') {
        return true;
    }
    return false;
});

registerParser(() => {
    addStandardHeader();
    read();
    const fileBytes = getData(1);
    const totalBytes = fileBytes.length;

    // Default width and available widths for quick selection
    let currentWidth = 256;
    const widthPresets = [32, 64, 128, 160, 192, 256, 320, 512, 640, 1024];
    
    function hex(v, pad) { return '0x' + v.toString(16).toUpperCase().padStart(pad||1,'0'); }
    
    function calculateDimensions(width) {
        // Each pixel is 2 bytes (RGB565)
        const pixelsPerRow = width;
        const bytesPerRow = pixelsPerRow * 2;
        const totalPixels = Math.floor(totalBytes / 2);
        const height = Math.ceil(totalPixels / pixelsPerRow);
        return { width, height, pixelsTotal: totalPixels, bytesUsed: totalPixels * 2 };
    }
    
    function renderImage(width) {
        const dims = calculateDimensions(width);
        const { height, pixelsTotal } = dims;
        
        const ctx = addCanvas(width, height, `Binary Image (${width}×${height})`);
        const imgData = ctx.createImageData(width, height);
        const pixels = imgData.data;
        
        for (let i = 0; i < pixelsTotal && i < totalBytes / 2; ++i) {
            const hi = fileBytes[i*2];
            const lo = fileBytes[i*2 + 1];
            const rgb565 = (hi << 8) | lo;
            
            const r = Math.round(((rgb565 >> 11) & 0x1F) * 255 / 31);
            const g = Math.round(((rgb565 >> 5) & 0x3F) * 255 / 63);
            const b = Math.round((rgb565 & 0x1F) * 255 / 31);
            
            pixels[i*4 + 0] = r;
            pixels[i*4 + 1] = g;
            pixels[i*4 + 2] = b;
            pixels[i*4 + 3] = 255;
        }
        
        ctx.putImageData(imgData, 0, 0);
        return dims;
    }
    
    // Title and file info
    addRow('File Size', `${totalBytes} bytes (${hex(totalBytes, 6)})`);
    addRow('Total Pixels', `${Math.floor(totalBytes / 2)} (16-bit RGB565 values)`);
    addRow('', '');
    
    // Width selector buttons
    addRow('Quick Width Presets', '');
    for (let w of widthPresets) {
        const dims = calculateDimensions(w);
        addRow(`  → ${w}px wide`, `Height: ${dims.height}px (${dims.pixelsTotal} pixels)`);
    }
    addRow('', '');
    
    // Render current width image
    addRow('Current Rendering', `Width: ${currentWidth}px`);
    const dims = renderImage(currentWidth);
    addRow('Image Dimensions', `${dims.width}×${dims.height} pixels`);
    addRow('Bytes Used', `${dims.bytesUsed} of ${totalBytes} bytes`);
    addRow('', '');
    
    // Instructions for measurement
    addRow('Measurement Guide', 'Use Binary Ninja\'s region selection tools or extract using address/length below');
    addRow('To Extract an Image:', '');
    addRow('  1. Visual inspection:', 'Find your image in the rendered view');
    addRow('  2. Identify bounds:', 'Note the pixel column/row positions');
    addRow('  3. Calculate address:', 'pixel_offset × 2 = byte_offset');
    addRow('  4. Calculate size:', '(width × height) × 2 = bytes');
    addRow('', '');
    
    // Helper function for quick calculations
    addRow('Quick Calculator', '');
    
    function addCalculatorEntry(pixelX, pixelY, pixelW, pixelH) {
        const byteOffset = (pixelY * currentWidth + pixelX) * 2;
        const byteLength = (pixelW * pixelH) * 2;
        addRow(
            `  Image at (${pixelX},${pixelY}) ${pixelW}×${pixelH}`,
            `Offset: ${hex(byteOffset, 6)}, Length: ${hex(byteLength, 5)}`
        );
    }
    
    // Example calculations based on the original binE.js map entries
    // Users can add their own by modifying this
    addCalculatorEntry(0, 0, 128, 160);      // First image example
    addCalculatorEntry(128, 0, 128, 160);    // Second image example
    
    addRow('', '');
    addRow('Manual Calculation Reference:', '');
    addRow('  Byte Address = (PixelY × Width + PixelX) × 2');
    addRow('  Byte Length = (Width × Height) × 2');
    addRow('  PixelY = Byte Address ÷ (Width × 2)');
    addRow('  PixelX = (Byte Address mod (Width × 2)) ÷ 2');
    
    addRow('', '');
    addRow('Notes', 'Images are raw RGB565 bitmaps (16-bit little-endian). To find proper widths, adjust the width preset above and observe where image boundaries align.');
});
