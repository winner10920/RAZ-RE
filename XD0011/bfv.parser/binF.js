/**
 * Select the files for which the parser should work.
 */
registerFileType((fileExt, filePath, fileData) => {
    // Check for the right file extension
    if (fileExt == 'binF') {
        return true;
    }
    return false;
});


/**
 * The parser to decode the file.
 */
registerParser(() => {
    addStandardHeader();

    // Read entire file into memory once (file sizes here are modest)
    read();
    const fileBytes = getData(1); // array of bytes

    // External flash memory map (from repository readme)
    const map = [
        { idx:0,  offset:0x00000,  length:0x9600,  w:120,  h:160, category:'Background' },
        { idx:1,  offset:0x0A000,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:2,  offset:0x0ABE0,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:3,  offset:0x0B7C0,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:4,  offset:0x0C3A0,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:5,  offset:0x0DB60,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:6,  offset:0x0E740,  length:0x0BE0,  w:40,  h:40,  category:'Battery Icon' },
        { idx:7,  offset:0x0F320,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:8,  offset:0x0FF00,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:9,  offset:0x10AE0,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:10, offset:0x116C0,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:11, offset:0x122A0,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:12, offset:0x12E80,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:13, offset:0x13A60,  length:0x0BE0,  w:40,  h:40,  category:'Juice Icon' },
        { idx:14, offset:0x14642,  length:0x2EE0,  w:75,  h:80,  category:'Normal Mode Icon' },
        { idx:15, offset:0x17522,  length:0x2EE0,  w:75,  h:80,  category:'Boost Mode Icon' },
        { idx:16, offset:0x1A400,  length:0x0C18,  w:43,  h:36,  category:'Moon Icon' },
        { idx:17, offset:0x1B018,  length:0x0C18,  w:43,  h:36,  category:'Sun Icon' },
        { idx:18, offset:0x1BC30,  length:0x1440,  w:72,  h:36,  category:'12W Icon' },
        { idx:19, offset:0x1D070,  length:0x1440,  w:72,  h:36,  category:'24W Icon' },
        { idx:20, offset:0x1E4B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:21, offset:0x202B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:22, offset:0x220B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:23, offset:0x23EB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:24, offset:0x25CB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:25, offset:0x27AB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:26, offset:0x298B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:27, offset:0x2B6B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:28, offset:0x2D4B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:29, offset:0x2F2B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:30, offset:0x310B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:31, offset:0x32EB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:32, offset:0x34CB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:33, offset:0x36AB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:34, offset:0x388B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:35, offset:0x3A6B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:36, offset:0x3C4B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:37, offset:0x3E2B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:38, offset:0x400B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:39, offset:0x41EB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:40, offset:0x43CB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:41, offset:0x45AB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:42, offset:0x478B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:43, offset:0x496B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:44, offset:0x4B4B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:45, offset:0x4D2B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:46, offset:0x4F0B0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:47, offset:0x50EB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:48, offset:0x52CB0,  length:0x1E00,  w:120,  h:32, category:'Flame Animation' },
        { idx:49, offset:0x54AB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:50, offset:0x568B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:51, offset:0x586B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:52, offset:0x5A4B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:53, offset:0x5C2B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:54, offset:0x5E0B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:55, offset:0x5FEB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:56, offset:0x61CB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:57, offset:0x63AB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:58, offset:0x658B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:59, offset:0x676B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:60, offset:0x694B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:61, offset:0x6B2B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:62, offset:0x6D0B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:63, offset:0x6EEB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:64, offset:0x70CB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:65, offset:0x72AB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:66, offset:0x748B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:67, offset:0x766B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:68, offset:0x784B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:69, offset:0x7A2B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:70, offset:0x7C0B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:71, offset:0x7DEB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:72, offset:0x7FCB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:73, offset:0x81AB0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:74, offset:0x838B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:75, offset:0x856B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:76, offset:0x874B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:77, offset:0x892B0,  length:0x1E00,  w:120,  h:32, category:'Flame2 Animation' },
        { idx:78, offset:0x8B0B0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:79, offset:0x8C700,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:80, offset:0x8DD50,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:81, offset:0x8F3A0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:82, offset:0x909F0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:83, offset:0x92040,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:84, offset:0x93690,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:85, offset:0x93690,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:86, offset:0x94CE0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:87, offset:0x96330,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:88, offset:0x97980,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:89, offset:0x98FD0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:90, offset:0x9A620,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:91, offset:0x9BC70,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:92, offset:0x9D2C0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:93, offset:0x9E910,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:94, offset:0x9FF60,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:95, offset:0xA15B0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:96, offset:0xA2C00,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:97, offset:0xA4250,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:98, offset:0xA58A0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:99, offset:0xA6EF0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:100, offset:0xA8540,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:101, offset:0xA9B90,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:102, offset:0xAB1E0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:103, offset:0xAC830,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:104, offset:0xADE80,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:105, offset:0xAF4D0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:106, offset:0xB0B20,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:107, offset:0xB2170,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:108, offset:0xB37C0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:109, offset:0xB4E10,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:110, offset:0xB6460,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:111, offset:0xB7AB0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:112, offset:0xB9100,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:113, offset:0xBA750,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:114, offset:0xBBDA0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:115, offset:0xBD3F0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:116, offset:0xBEA40,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:117, offset:0xC0090,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:118, offset:0xC16E0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:119, offset:0xC2D30,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:120, offset:0xC4380,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:121, offset:0xC59D0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:122, offset:0xC7020,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:123, offset:0xC8670,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:124, offset:0xC9CC0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:125, offset:0xCB310,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:126, offset:0xCC960,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:127, offset:0xCDFB0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:128, offset:0xCF600,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:129, offset:0xD0C50,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:130, offset:0xD22A0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:131, offset:0xD38F0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:132, offset:0xD4F40,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:133, offset:0xD6590,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:134, offset:0xD7BE0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:135, offset:0xD9230,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:136, offset:0xDA880,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:137, offset:0xDBED0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:138, offset:0xDD520,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:139, offset:0xDEB70,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:140, offset:0xE01C0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:141, offset:0xE1810,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:142, offset:0xE2E60,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:143, offset:0xE44B0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:144, offset:0xE5B00,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:145, offset:0xE7150,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:146, offset:0xE87A0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:147, offset:0xE9DF0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:148, offset:0xECA90,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:149, offset:0xEE0E0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:150, offset:0xEF730,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:151, offset:0xF0D80,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:152, offset:0xF23D0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:153, offset:0xF3A20,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:154, offset:0xF5070,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:155, offset:0xF66C0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:156, offset:0xF7D10,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:157, offset:0xF9360,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:158, offset:0xFA9B0,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:159, offset:0xFC000,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:160, offset:0xFD650,  length:0x1650,  w:42,  h:68, category:'Unknown Background' },
        { idx:161, offset:0xFECA0,  length:0x1360,  w:42,  h:68, category:'Unknown Background' },
        { idx:161, offset:0xF8000,  length:0x000f,  w:null,  h:null, category:'Unknown ' }


    ];

    // helper: format hex
    function hex(v, pad) { return '0x' + v.toString(16).toUpperCase().padStart(pad||1,'0'); }

    // Iterate map and add rows + preview canvas if possible
    for (let e of map) {
        const label = `Index ${e.idx} - ${e.category}`;
        const desc = `Offset: ${hex(e.offset,5)}, Length: ${hex(e.length,4)}${e.w? ', Size: ' + e.w + 'x' + e.h : ''}`;
        addRow(label, desc);

        if (e.w && e.h) {
            addDetails(() => {
                try {
                    // slice bytes from the full file buffer
                    const start = e.offset;
                    const len = e.length;
                    if (!fileBytes || fileBytes.length < start + len) {
                        addRow('Preview', 'Not enough data for preview');
                        return;
                    }
                    const raw = fileBytes.slice(start, start + len);

                    const ctx = addCanvas(e.w, e.h, `Preview ${e.idx}`);
                    // create ImageData
                    const imgData = ctx.createImageData(e.w, e.h);
                    const pixels = imgData.data;
                    for (let i = 0; i < e.w * e.h; ++i) {
                        const hi = raw[i*2];
                        const lo = raw[i*2 + 1];
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
                } catch (err) {
                    addRow('Preview Error', String(err));
                }
            }, true);
        } else {
            try {
                    // slice bytes from the full file buffer
                    const start = e.offset;
                    const len = e.length;
                    if (!fileBytes || fileBytes.length < start + len) {
                        addRow('Preview', 'Not enough data for preview');
                        return;
                    }
                    const raw = fileBytes.slice(start, start + len);

                    addRow('Data', raw);
                    } catch (err) {
                    addRow('Preview Error', String(err));
                }
        }


    }

    addRow('Notes', 'Images are raw RGB565 bitmaps (16-bit little-endian). Previews rendered using addCanvas.');
});
