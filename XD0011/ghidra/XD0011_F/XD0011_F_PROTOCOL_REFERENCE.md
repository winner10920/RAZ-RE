/**
 * XD0011_F_PROTOCOL_REFERENCE.md
 * 
 * Complete protocol specification and register operations documentation
 * for the N32G031K8 light measurement device firmware.
 * 
 * This document describes:
 * 1. SPI Flash Memory Protocol (Winbond W25Q16)
 * 2. LCD Display Protocol (ST7735R/NV3029)
 * 3. ADC Measurement Protocol
 * 4. GPIO Control Sequences
 * 5. DMA Transfer Configuration
 * 6. Register Bit Definitions
 * 7. Timing Specifications
 * 8. Error Handling
 */

# XD0011_F FIRMWARE PROTOCOL REFERENCE

## Table of Contents
- SPI Flash Memory Protocol
- LCD Display Protocol  
- ADC Measurement Protocol
- GPIO Control Sequences
- DMA Configuration
- Register Definitions
- Timing Specifications
- Troubleshooting Guide

---

## 1. SPI FLASH MEMORY PROTOCOL

### Overview
- **Device**: Winbond W25Q16 (2MB SPI Flash)
- **Interface**: SPI1 at 4MHz (N32G031K8 APB clock / 8)
- **Addressing**: 3-byte address (MSB first)
- **Data Width**: 8-bit (bytes)
- **Page Size**: 256 bytes
- **Sector Size**: 4KB
- **Endianness**: Big-endian (MSB first)

### SPI Communication Frame Structure

```
FLASH_CS (GPIO)   ─┐
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│ COMMAND │ ADDR_H │ ADDR_M │ ADDR_L │ DATA_0 │ DATA_1 │ ... │
│  [1B]   │ [1B]   │ [1B]   │ [1B]   │ [1B]   │ [1B]   │     │
└─────────────────────────────────────────────────────────────┘
                   ▲
                   │
             FLASH_CS (GPIO)
```

**Timing**:
- Command transmission: 1 byte × 2 µs = 2 µs
- Address transmission: 3 bytes × 2 µs = 6 µs  
- Data rate: ~1 byte per 2 µs at 4MHz SPI clock
- CS setup/hold: ≥1 µs before/after SPI transfer

### Flash Commands

#### 0x03 - READ DATA
**Purpose**: Read arbitrary length data from flash address
**Format**: `0x03 + ADDR_H + ADDR_M + ADDR_L + DATA_BYTES...`
**Response**: Data bytes at specified address
**Duration**: ~3 µs (command + address) + transfer_time
**Max Length**: 16MB (3-byte address)

**Register Sequence**:
```c
flash_cs_low();
spi_write_byte(0x03);              // Read command
spi_write_byte((addr >> 16) & 0xFF); // Address high
spi_write_byte((addr >> 8) & 0xFF);  // Address mid
spi_write_byte(addr & 0xFF);         // Address low
for (i = 0; i < length; i++) {
    data[i] = spi_write_byte(0xFF); // Dummy write reads data
}
flash_cs_high();
```

#### 0x02 - PAGE WRITE
**Purpose**: Write up to 256 bytes to single page
**Format**: `0x02 + ADDR_H + ADDR_M + ADDR_L + DATA_BYTES...`
**Constraints**: 
- Must not cross 256-byte page boundary
- Requires write enable (0x06) first
**Duration**: ~3 µs (command) + ~2ms (internal programming)
**Auto-increment**: Address auto-increments within page; wraps at page boundary

**Timing Diagram**:
```
T0: CS ─┐           ┌─ Command/Address transmitted (~3µs)
        │           │
T1: TX  ├──────────┤├──────────────────────┤ Data bytes
        │           │  (Page program)      │
T2+2ms: TX Done     └─ CS ─┐               ┌─
                           │ Poll status   │
T2+2ms: Status   ┌─────────┴───────────────┘
```

**Register Sequence**:
```c
flash_write_enable();           // Send 0x06 first
flash_cs_low();
spi_write_byte(0x02);           // Write command
spi_write_byte(addr >> 16);
spi_write_byte(addr >> 8);
spi_write_byte(addr & 0xFF);
for (i = 0; i < length; i++) {
    spi_write_byte(data[i]);
}
flash_cs_high();
while (flash_read_status() & 0x01) {  // Poll WIP bit
    // Wait for write complete (~2ms)
}
```

#### 0x20 - ERASE 4KB SECTOR
**Purpose**: Erase 4KB sector (sets all bits to 1)
**Format**: `0x20 + ADDR_H + ADDR_M + ADDR_L`
**Constraints**: Address must be 4KB-aligned (bits 11:0 = 0x000)
**Duration**: ~400ms (typical 200-400ms)
**Auto-increment**: Erases entire 16 pages (4096 bytes)

**Valid Addresses**:
- 0x000000, 0x001000, 0x002000, ..., 0x1FF000

**Invalid Addresses** (will erase incorrect sector):
- 0x000100, 0x001234, 0x002FFF (not 4KB aligned)

**Register Sequence**:
```c
flash_write_enable();           // 0x06 first
flash_cs_low();
spi_write_byte(0x20);           // Erase command
spi_write_byte(addr >> 16);
spi_write_byte(addr >> 8);
spi_write_byte(addr & 0xFF);    // Lower 12 bits ignored
flash_cs_high();
// Poll for ~400ms
while (flash_read_status() & 0x01) {
    delay_ms(1);
}
```

#### 0x05 - READ STATUS REGISTER
**Purpose**: Read status register (WIP, WEL bits)
**Format**: `0x05`
**Response**: 1 status byte
**Duration**: ~2 µs
**Usage**: Poll to detect write/erase completion

**Status Register Bits**:
```
Bit 0: WIP (Write In Progress) - 1=write/erase in progress, 0=complete
Bit 1: WEL (Write Enable Latch) - 1=write enabled, 0=write disabled
Bit 2-3: BP (Block Protect) - 0=no protection
Bit 4-5: TB (Top/Bottom) - 0=top block protected
Bit 6: SEC (Sector Protect) - 0=no sector protect
Bit 7: SUS (Suspend) - 0=normal operation
```

**Polling Sequence**:
```c
// Check if write complete
while (flash_read_status() & 0x01) {
    // WIP bit set = still writing
}
// Check if write enabled (should be 0 after write completes)
if ((flash_read_status() & 0x02) == 0) {
    // Write complete, WEL auto-cleared
}
```

#### 0x06 - WRITE ENABLE
**Purpose**: Set Write Enable Latch (WEL) before write/erase
**Format**: `0x06`
**Duration**: <1 µs
**Auto-clear**: WEL cleared automatically after write/erase completes
**Required**: Must send before every write or erase operation

**Register Sequence**:
```c
flash_cs_low();
spi_write_byte(0x06);
flash_cs_high();
// WEL now set, can write or erase
```

### Flash Memory Map

```
Address      Size      Purpose
────────────────────────────────────────
0x000000     1KB       Reserved / Boot
0x000400     3.75KB    
0x001000     4KB       Calibration Data (1 sector)
0x002000     2MB-8KB   User Application Data / Images
0x200000     (end)
```

### Flash Protocol Validation

**Checksum Verification** (command 0x05):
- After write, send 0x05 and compare returned status
- Status should be 0xA5 pattern for successful write
- If different, write failed and should be retried

**Error Handling**:
1. Write to protected sector → Returns 0xFF in response
2. Erase without write enable → Silently fails (0x01 WIP never clears)
3. Invalid address → May erase wrong sector (address wraparound)

---

## 2. LCD DISPLAY PROTOCOL

### Overview
- **Device**: ST7735R or compatible (NV3029 variant)
- **Resolution**: 128×160 pixels
- **Color Format**: RGB565 (16-bit per pixel)
- **Interface**: SPI1 at 4MHz + 2 GPIO pins (CS, DC)
- **Command Protocol**: 8-bit bytes via SPI
- **Initialization**: 100+ register writes required

### GPIO Control Pins

```
Pin         Function        Active Level    Purpose
──────────────────────────────────────────────────────
PORTA_8     CS (Chip Sel)   Low            Enable display SPI
PORTA_9     DC (Cmd/Data)   Low=Cmd        High=Data
PORTA_10    RESET           Low            Reset display
```

**Timing Constraints**:
- CS setup: ≥1 µs before SPI
- DC setup: Change before/after CS, ≥1 µs timing
- Reset: Low ≥10ms, then High, wait ≥120ms before commands

### Display Command Format

**Command Byte Structure**:
```
Every command is 8-bit hex value sent with DC=LOW

Format: DC=LOW → spi_write(COMMAND) → DC=HIGH → spi_write(DATA_BYTES...)

Example (0x3A = Color Mode):
  dc_command();
  cs_low();
  spi_write(0x3A);      // Color mode command
  cs_high();
  dc_data();
  cs_low();
  spi_write(0x65);      // RGB565 format
  cs_high();
```

### Critical Display Commands

#### 0xFF - Unlock Command Set
**Purpose**: Unlock extended command register bank
**Format**: `0xFF 0xA5` (0xFF = command, 0xA5 = parameter)
**Effect**: Enables access to gamma and extended commands
**Must precede**: All 0x70-0xC3 register writes

#### 0x3E - Display Mode
**Format**: `0x3E [mode_byte]`
**Typical**: `0x3E 0x08`
**Effect**: Sets display mode (partial vs full)

#### 0x3A - Color Format
**Format**: `0x3A [format_byte]`
**Value 0x65**: RGB565 (our setting, 16-bit color)
**Value 0x55**: RGB444 (12-bit color)
**Value 0x66**: RGB666 (18-bit color)
**Critical**: Must match image data format

#### 0x2A / 0x2B - Window Address
**Purpose**: Set drawable area (must set before drawing)
**Format**: 
```
0x2A [col_start] [col_end]    // Column address
0x2B [row_start] [row_end]    // Row address
```
**Example** (full screen 128×160):
```
0x2A 0x00 0x7F    // Columns 0 to 127
0x2B 0x00 0x9F    // Rows 0 to 159
```

#### 0x2C - Memory Write
**Purpose**: Begin writing pixel data to display RAM
**Format**: `0x2C` (followed by DC=HIGH, then pixel data bytes)
**Data Format**: 16-bit RGB565 pixels (MSB first)
**Duration**: 20ms to transfer full 128×160 image at 4MHz SPI

**Pixel Data Format**:
```
Each pixel = 16 bits = 2 bytes
┌─────────────────────┬─────────────────────┐
│ Byte 0 (MSB)        │ Byte 1 (LSB)        │
├─────────────────────┼─────────────────────┤
│ R7 R6 R5 R4 R3 G7 G6 G5 │ G4 G3 G2 B7 B6 B5 B4 B3 │
└─────────────────────┴─────────────────────┘

Example: Bright Red (R=255, G=0, B=0)
  Byte 0: 1111 1000 = 0xF8
  Byte 1: 0000 0000 = 0x00
```

### Initialization Sequence (Detailed)

**Phase 1: Hardware Reset**
```
1. lcd_reset_low();
2. delay_ms(10);
3. lcd_reset_high();
4. delay_ms(120);  // Wait for internal stabilization
```

**Phase 2: Unlock & Mode Configuration**
```
0xFF 0xA5  // Unlock extended commands
0x3E 0x08  // Display mode
0x3A 0x65  // Color format = RGB565 (16-bit)
```

**Phase 3: Frame Rate Control**
```
0xB1 0x0F  // Frame rate 1 (normal mode)
0xB2 0x0C  // Frame rate 2 (idle mode)
0xB3 0x00  // Frame rate 3 (partial mode)
```

**Phase 4: Power Control** (5 commands = 0xC0-0xC5)
- 0xC0: GVDD voltage (0x0C typical)
- 0xC1: GVDD negaive voltage
- 0xC2: Operating voltage (amplitude)
- 0xC3: Booster frequency
- 0xC4: Booster ratio
- 0xC5: Contrast control

**Phase 5: Gamma Correction** (30+ commands = 0x70-0xAF)
- 0x70-0x7F: Red gamma curve (16 values)
- 0x80-0x8F: Green gamma curve (16 values)
- 0x90-0x9F: Blue gamma curve (16 values)

**Phase 6: Additional Gamma** (0x82-0xC3)
- Extended gamma lookup table
- Temperature compensation

**Phase 7: Final Setup & Enable**
```
0x36 0x00  // Memory access direction
0x35 0x00  // Tearing effect OFF (no sync)
0x11       // Exit sleep mode
[wait 120ms]
0x29       // Display ON
```

**Phase 8: Lock Command Set**
```
0xFF 0x00  // Lock extended commands (return to normal)
```

### Display Drawing Sequence

**Full Screen Image Transfer**:
```c
// 1. Set window to entire display
lcd_set_window(0, 159, 0, 127);

// 2. Send memory write command
dc_command();
cs_low();
spi_write(0x2C);
cs_high();

// 3. Switch to data mode and send pixel stream
dc_data();
cs_low();
for (int y = 0; y < 160; y++) {
    for (int x = 0; x < 128; x++) {
        uint16_t pixel = image[y * 128 + x];  // RGB565
        spi_write((pixel >> 8) & 0xFF);        // MSB
        spi_write(pixel & 0xFF);               // LSB
    }
}
cs_high();
```

**Total Transfer Time**:
- Pixels: 128 × 160 = 20,480 pixels
- Bytes: 20,480 × 2 = 40,960 bytes
- Time: 40,960 bytes × 2 µs = 81.92 ms ≈ 20ms @ 4MHz

### Color Palette (RGB565)

```
Color       R     G     B     HEX Code   Binary
──────────────────────────────────────────────────
Black       0     0     0     0x0000     0000000000000000
White       255   255   255   0xFFFF     1111111111111111
Red         255   0     0     0xF800     1111100000000000
Green       0     255   0     0x07E0     0000011111100000
Blue        0     0     255   0x001F     0000000000011111
Yellow      255   255   0     0xFFE0     1111111111100000
Cyan        0     255   255   0x07FF     0000011111111111
Magenta     255   0     255   0xF81F     1111100000011111
Gray        128   128   128   0x8410     1000010000010000
Orange      255   165   0     0xFD20     1111110100100000
```

### Display Sleep Modes

**Enter Sleep Mode (0x10)**:
- Display OFF but memory retained
- Power consumption: <1mW
- Command 0x28 (display off) recommended first

**Exit Sleep Mode (0x11)**:
- Resume from sleep
- Wait 120ms before sending commands
- Send 0x29 (display on) to make visible

---

## 3. ADC MEASUREMENT PROTOCOL

### ADC Hardware Configuration

**ADC Registers** (N32G031K8):
```
Address     Offset  Name      Purpose
──────────────────────────────────────────────
0x40012400  +0x00   CTRL1     Enable, interrupts, resolution
0x40012400  +0x04   CTRL2     Trigger, sequence selection
0x40012400  +0x08   SMPT1     Sample time (channels 0-9)
0x40012400  +0x0C   SMPT2     Sample time (channels 10-17)
0x40012400  +0x28   RSEQ1     Regular sequence register
0x40012400  +0x30   DAT       12-bit conversion result
```

### ADC Channels

```
Channel   Input           Typical Range
───────────────────────────────────────────────
0         Light Sensor    0-4095 ADC counts (0-3.3V)
16        Temp Sensor     ~1500-2500 (25°C reference)
17        Vref (1.2V)     ~1024 @ 3.3V supply
```

### ADC Conversion Timing

```
Timeline for single conversion:
T0: ADON bit set (start conversion)
T1-T12: Sample phase (12 cycles @ 1MHz = 12µs)
T12-T24: Conversion phase (12 cycles × ~2 = ~24µs)
T24: EOC flag set, result in DAT register

Total: ~36µs per conversion

For robust measurement with debounce:
  1. Trigger conversion
  2. Wait 110µs (3x conversion time for stability)
  3. Read result
```

### ADC Read Sequence

```c
// 1. Select channel via RSEQ1
ADC_RSEQ1 = (channel & 0x1F);  // 5-bit channel select

// 2. Start conversion
ADC_CTRL2 |= 0x0001;            // ADON = 1

// 3. Poll for completion
uint32_t timeout = 1000;        // 1000µs max wait
while ((ADC_CTRL2 & 0x0001) && timeout--) {
    delay_us(1);
}

// 4. Read result (lower 12 bits only)
uint16_t adc_value = DAT & 0x0FFF;
```

### Temperature Sensor Calibration

**Factory Calibration Values** (in ROM):
- Temp @ 25°C: ~1500 ADC count (stored in 0x1FFFF814)
- Temp slope: ~4.3 mV/°C or 0.43 ADC count/°C

**Temperature Calculation**:
```
T(°C) = (ADC_VALUE - CALIB_25) / SLOPE + 25.0

Where:
  CALIB_25 ≈ 1500 (factory calibration point)
  SLOPE ≈ 6.0 counts/°C (depends on ADC/trim calibration)

Example: ADC reads 1560
  T = (1560 - 1500) / 6.0 + 25.0 = 35°C
```

### ADC Sample Timing for Stability

**Debouncing Strategy**:
```
1. First read (remove glitch):    T0
2. Wait 110µs:                     T0 + 110µs
3. Second read (stabilized):       T0 + 110µs
4. Wait 110µs:                     T0 + 220µs
5. Third read (confirmation):      T0 + 220µs

Use average of 3 reads for final value
```

**Why 110µs?**:
- 110µs ≈ 3× ADC conversion time (36µs)
- Allows capacitive debouncing on sensor input
- Filters out transient noise spikes
- Typical RC time constant: ~100µs

---

## 4. GPIO CONTROL SEQUENCES

### Port Configuration Register (MODER)

```
GPIOx_MODER register layout:
Bits [2×PIN+1 : 2×PIN]
  00 = Input mode
  01 = Output mode
  10 = Alternate function mode
  11 = Analog mode (for ADC inputs)

Example: Configure PIN 8 as output
  GPIOX_MODER &= ~(0x3 << 16);    // Clear bits 17:16
  GPIOX_MODER |=  (0x1 << 16);    // Set to 01 (output)
```

### Output Control (ODR/BSRR)

```
Method 1: Using ODR register (read-modify-write)
  ⚠️  NOT atomic, can have race conditions
  
  GPIOX_ODR |=  (1 << 8);         // Set pin 8
  GPIOX_ODR &= ~(1 << 8);         // Clear pin 8

Method 2: Using BSRR register (atomic, recommended)
  ✓ Atomic operation, no race condition
  
  Bits [15:0]   = Set bits (if written with 1)
  Bits [31:16]  = Reset bits (if written with 1)
  
  GPIOX_BSRR = (1 << 8);          // Set pin 8 (atomic)
  GPIOX_BSRR = (1 << 24);         // Reset pin 8 (atomic)
```

### LCD GPIO Sequence

```
Initialize LCD GPIO pins:

1. Configure as output, push-pull, high speed
   - MODER = 0x01 (output)
   - OTYPER = 0x00 (push-pull)
   - OSPEEDR = 0x03 (high speed)

2. LCD CS control (PORTA_8):
   GPIOA_BSRR = (1 << 8);         // CS HIGH (idle)
   // Before SPI transaction:
   GPIOA_BSRR = (1 << 24);        // CS LOW (enable)
   // After SPI transaction:
   GPIOA_BSRR = (1 << 8);         // CS HIGH (disable)

3. LCD DC control (PORTA_9):
   GPIOA_BSRR = (1 << 9);         // DC HIGH (data mode)
   spi_write(pixel_data);
   GPIOA_BSRR = (1 << 25);        // DC LOW (command mode)
   spi_write(cmd_byte);

4. LCD RESET (PORTA_10):
   GPIOA_BSRR = (1 << 26);        // RESET LOW
   delay_ms(10);
   GPIOA_BSRR = (1 << 10);        // RESET HIGH
   delay_ms(120);                  // Wait stabilization
```

### Input Reading (GPIO_IDR)

```
Read input pin state:

  uint32_t pin_state = GPIOX_IDR & (1 << pin_number);
  if (pin_state) {
      // Pin is HIGH
  } else {
      // Pin is LOW
  }

Debouncing input (e.g., button press):
  for (int i = 0; i < 10; i++) {
      if ((GPIOX_IDR & (1 << pin)) == 0) {
          count++;
      }
      delay_ms(10);  // 10ms per sample
  }
  if (count > 5) {
      // Button pressed (majority vote)
  }
```

---

## 5. DMA CONFIGURATION

### DMA Channel Registers (N32G031K8)

```
Channel 3 SPI1 TX (used for LCD bulk transfers):

Offset from DMA_BASE (0x40020000):
  +0x08:  CCR3    Channel 3 Control Register
  +0x0C:  CNDTR3  Data counter (remaining transfers)
  +0x10:  CPAR3   Peripheral address (SPI DAT = 0x4001300C)
  +0x14:  CMAR3   Memory address (image data buffer)
```

### DMA Control Register (CCRx) Bits

```
Bit 0:  EN       Enable DMA channel
Bit 1:  TCIE     Transfer complete interrupt enable
Bit 2:  HTIE     Half-transfer interrupt enable
Bit 3:  TEIE     Transfer error interrupt enable
Bit 4:  DIR      Direction: 0=peripheral←memory
Bit 5:  CIRC     Circular mode (auto-reload)
Bit 6:  PINC     Peripheral address increment
Bit 7:  MINC     Memory address increment (usually 1)
Bits 8-9:  PSIZE Peripheral size: 00=8-bit, 01=16-bit
Bits 10-11: MSIZE Memory size: 00=8-bit, 01=16-bit
Bits 12-14: PL   Priority level
```

### DMA Setup for LCD Image Transfer

```c
// 1. Configure DMA channel 3
DMA_CPAR3 = 0x4001300C;         // SPI1 data register
DMA_CMAR3 = (uint32_t)image_buffer;  // Pixel data
DMA_CNDTR3 = 40960;             // 20480 pixels × 2 bytes
DMA_CCR3 = 0x0081;              // DIR=0, MINC=1, PSIZE=0, MSIZE=0

// 2. Set SPI to DMA mode
SPI_CTRL2 |= 0x0002;            // TXDMAEN = 1

// 3. Enable DMA channel
DMA_CCR3 |= 0x0001;             // EN = 1

// 4. Wait for completion
while ((DMA_CNDTR3 & 0xFF) > 0) {  // Poll remaining count
    // Spin until transfer complete
}
```

---

## 6. TIMING SPECIFICATIONS

### Critical Timing Values

```
Component          Timing          Tolerance
─────────────────────────────────────────────
SPI Clock          4MHz (250ns)    ±5%
SPI Byte           ~2µs            ±1µs
Flash Read         ~1µs/byte       ±0.5µs
Flash Write        ~2ms/page       ±500µs
Flash Erase        ~400ms/sector   ±100ms

LCD Reset Low      ≥10ms           Minimum
LCD Init Delay     ~100-200ms      After reset
LCD Sleep Wait     ≥120ms          Before/after sleep
LCD Memory Write   ~20ms/frame     128×160 pixels

ADC Conversion     ~36µs           ±10µs
ADC Sample Time    ≥110µs          Recommended
Button Debounce    ≥100ms          Total
Display Refresh    200ms (5Hz)     Typical
Auto-sleep Timeout 6 seconds       Configurable

Power-on Sequence  ~500ms          From reset to ready
```

### Measurement Timing Example

```
T0: Trigger ADC conversion
    adc_trigger_conversion(LIGHT_SENSOR_CHANNEL);

T0+36µs: Conversion complete (ADC sets EOC)

T0+110µs: First stable read
    value1 = adc_read();
    
T0+220µs: Second stable read
    value2 = adc_read();

T0+330µs: Third stable read  
    value3 = adc_read();

T0+400µs: Calculate average
    avg = (value1 + value2 + value3) / 3;
```

---

## 7. ERROR HANDLING

### Flash Memory Errors

| Error | Cause | Detection | Recovery |
|-------|-------|-----------|----------|
| Write fails | WEL not set | Status 0x01 never clears | Retry: Write enable + write |
| Erase fails | Sector protected | Status 0x01 never clears | Disable protection, retry |
| Data corrupted | Power loss during write | Checksum mismatch | Re-read, compare, re-write |
| SPI timeout | CS stuck low | SPI hangs >1ms | Reset SPI, retry |

### ADC Errors

| Error | Cause | Detection | Recovery |
|-------|-------|-----------|----------|
| No conversion | ADON never set | EOC never signals | Re-trigger, check clock |
| Bad reading | Noise spike | Outlier >20% from average | Use median filter, re-sample |
| Timeout | ADC stalled | Poll count exhausted | Reset ADC peripheral |

### LCD Errors

| Error | Cause | Detection | Recovery |
|-------|-------|-----------|----------|
| No display | Reset failed | Blank/white screen | Pulse reset pin, retry init |
| Garbled image | Wrong pixel format | Shifted colors | Send color format command |
| No SPI response | CS stuck | Transactions hang | Release GPIO, reset SPI |
| Wrong colors | Gamma tables corrupted | Color shift | Reinitialize with extended commands |

---

## 8. TROUBLESHOOTING GUIDE

### Device Won't Boot

**Symptom**: Power-on test fails, watchdog resets

**Checks**:
1. **Flash accessible?**
   - Send 0x05 command, should get valid status
   - Check SPI clock waveform with oscilloscope
   
2. **Button debouncing issue?**
   - Count transitions should be stable
   - Try resetting button circuit

3. **ADC not initializing?**
   - Check if CTRL1 bits properly set
   - Verify reference voltage present (3.3V)

### LCD Shows Garbage

**Symptom**: Random pixels, no recognizable image

**Checks**:
1. **Color format mismatch**
   - Verify 0x3A 0x65 sent correctly (RGB565)
   - Check if image data is correct format

2. **SPI timing wrong**
   - Verify 4MHz clock (250ns period)
   - Check DC pin transitions around SPI writes

3. **Initialization incomplete**
   - Confirm 0xFF 0xA5 unlock sent
   - Verify display on command (0x29) sent

### Light Sensor Reads Zero

**Symptom**: ADC always returns 0

**Checks**:
1. **ADC channel wrong**
   - Confirm using channel 0 for light sensor
   - Check if pin connected to ADC input

2. **Sample time too short**
   - Should use 239.5 cycles (max SMPT)
   - 36µs conversion time minimum

3. **Reference voltage issue**
   - Measure Vref input (should be 3.3V)
   - Check if Vref enable bit set

---

## End of Protocol Reference

This document should be used in conjunction with:
- N32G031K8 ARM Cortex-M0+ Datasheet
- ST7735R/NV3029 LCD Controller Datasheet
- Winbond W25Q16 Flash Memory Datasheet
