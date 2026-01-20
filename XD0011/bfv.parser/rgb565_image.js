/**
 * RGB565 Binary Image Viewer
 * Displays RGB565 binary files as images with configurable width
 */
registerFileType((fileExt, filePath, fileData) => {
    if (fileExt == 'rgb565') {
        return true;
    }
    return false;
});

registerParser(() => {
    addStandardHeader();
    read();
    const fileBytes = getData(1);
    const totalBytes = fileBytes.length;

    // Default width for 128x160 display, with preset options
    let currentWidth = 128;
    
    function hex(v, pad) { return '0x' + v.toString(16).toUpperCase().padStart(pad||1,'0'); }
    
    function calculateDimensions(width) {
        // Each pixel is 2 bytes (RGB565)
        const totalPixels = Math.floor(totalBytes / 2);
        const height = Math.ceil(totalPixels / width);
        return { width, height, pixelsTotal: totalPixels, bytesUsed: totalPixels * 2 };
    }
    
    function renderImage(width) {
        const dims = calculateDimensions(width);
        const { height, pixelsTotal } = dims;
        
        const ctx = addCanvas(width, height, `RGB565 Image (${width}×${height})`);
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
    

    
    // Render image
    addRow('Current Rendering', `Width: ${currentWidth}px`);
    const dims = renderImage(currentWidth);
    addRow('Image Dimensions', `${dims.width}×${dims.height} pixels`);
    addRow('Bytes Used', `${dims.bytesUsed} of ${totalBytes} bytes`);
    addRow('', '');
    
    // Info
    addRow('Format', 'RGB565 16-bit little-endian');
    addRow('Conversion', 'Generated from image_to_rgb565.py converter');
    addRow('', '');
    addRow('Notes', 'Adjust width preset above if image doesn\'t display correctly. The file contains one complete RGB565 image.');
});
