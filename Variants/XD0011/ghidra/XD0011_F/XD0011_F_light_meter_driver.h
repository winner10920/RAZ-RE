/**
 * XD0011_F_light_meter_driver.h
 * 
 * Refactored firmware for N32G031K8 MCU-based Light Measurement Device
 * 
 * This header defines the complete interface for a light/energy measurement device with:
 * - LCD display driver (SPI-based, 128x160 resolution)
 * - Flash memory interface (SPI-based, page write/sector erase)
 * - ADC-based light sensor with temperature compensation
 * - Floating-point math library (IEEE 754 emulation)
 * - GPIO and external interrupt management
 * - DMA-accelerated data transfers
 * - Power management and button debouncing
 * 
 * HARDWARE PLATFORM:
 * - MCU: N32G031K8 (ARM Cortex-M0+, 64KB FLASH, 8KB SRAM)
 * - Display: ST7735R/NV3029 compatible (128x160, 16-bit RGB565)
 * - Flash: Winbond W25Q16 or compatible (2MB, 4KB sectors, 256-byte pages)
 * - Sensor: Ambient light sensor (ADC input)
 * - Power: Button input with debouncing
 * 
 * COMMUNICATION PROTOCOLS:
 * 1. SPI (Flash & LCD): 4MHz, MSB-first, CPOL=0, CPHA=0
 * 2. GPIO: 8-pin ports (GPIOA, GPIOB, GPIOC) with EXTI support
 * 3. ADC: 12-bit, 1MHz conversion rate, internal Vref
 * 4. DMA: 5 channels, used for LCD bulk transfers
 * 
 * CALIBRATION:
 * - Stored in flash at fixed location
 * - Contains sensor offset and scaling coefficients
 * - Temperature compensation applied in real-time
 */

#ifndef XD0011_F_LIGHT_METER_DRIVER_H
#define XD0011_F_LIGHT_METER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * TYPE DEFINITIONS
 * ============================================================================ */

typedef unsigned char       uint8_t_custom;
typedef unsigned short      uint16_t_custom;
typedef unsigned int        uint32_t_custom;
typedef unsigned long long  uint64_t_custom;
typedef float               fp32_t;
typedef double              fp64_t;

/* Display dimensions (from window commands 0x2A/0x2B) */
#define LCD_WIDTH           128
#define LCD_HEIGHT          160
#define LCD_PIXEL_FORMAT    0x65    /* 16-bit RGB565 */

/* SPI Flash parameters */
#define FLASH_PAGE_SIZE     256     /* Bytes per page (write granularity) */
#define FLASH_SECTOR_SIZE   4096    /* Bytes per sector (erase granularity) */
#define FLASH_ADDR_BITS     3       /* 3-byte addressing (supports up to 16MB) */

/* SPI Flash commands (Winbond W25Q16 compatible) */
#define FLASH_CMD_READ_DATA         0x03    /* Read data from flash at address */
#define FLASH_CMD_PAGE_WRITE        0x02    /* Write up to 256 bytes at page boundary */
#define FLASH_CMD_ERASE_SECTOR      0x20    /* Erase 4KB sector */
#define FLASH_CMD_READ_STATUS       0x05    /* Read status register (bits: WIP, WEL, etc) */
#define FLASH_CMD_WRITE_ENABLE      0x06    /* Set WEL bit to allow write/erase */
#define FLASH_STATUS_WRITE_IN_PROGRESS 0x01 /* Bit 0 of status register */
#define FLASH_STATUS_VERIFY_PATTERN 0xA5    /* Expected pattern after checksum verify */

/* SPI configurations */
#define SPI_TRANSFER_BLOCK_SIZE     512     /* Max bytes per DMA transfer */

/* ADC configuration */
#define ADC_SAMPLE_DELAY_US         110     /* Microseconds between ADC samples */
#define ADC_TEMP_CALIBRATION_DELAY  500     /* Initial calibration delay */
#define ADC_INTERNAL_VREF_MV        3300    /* 3.3V reference voltage */

/* GPIO pin control */
#define GPIO_MODE_INPUT             0x0     /* 2-bit field */
#define GPIO_MODE_OUTPUT            0x1
#define GPIO_MODE_ALTERNATE         0x2
#define GPIO_MODE_ANALOG            0x3

#define GPIO_PULL_NONE              0x0     /* 2-bit field */
#define GPIO_PULL_UP                0x1
#define GPIO_PULL_DOWN              0x2

#define GPIO_SPEED_LOW              0x0     /* 2-bit field */
#define GPIO_SPEED_MEDIUM           0x1
#define GPIO_SPEED_HIGH             0x3

/* External interrupt triggers */
#define EXTI_TRIGGER_RISING         0x10000  /* Rising edge flag */
#define EXTI_TRIGGER_FALLING        0x20000  /* Falling edge flag */

/* DMA channel configuration */
#define DMA_PRIORITY_LOW            0x0
#define DMA_PRIORITY_MEDIUM         0x1
#define DMA_PRIORITY_HIGH           0x2
#define DMA_PRIORITY_VERY_HIGH      0x3

/* Power management */
#define POWER_BUTTON_DEBOUNCE_CYCLES  1000  /* ~110ms at ~9kHz check rate */
#define ACTIVITY_CHECK_INTERVAL        500  /* Milliseconds between activity checks */
#define AUTO_SLEEP_TIMEOUT            6000  /* Milliseconds of inactivity before sleep */

/* Display timeout and refresh */
#define DISPLAY_REFRESH_INTERVAL_MS    200  /* Update display every 200ms */
#define DISPLAY_SLIDESHOW_INTERVAL_S     5  /* Auto-advance screens every 5 seconds */

/* Measurement state machine */
typedef enum {
    STATE_POWER_ON_TEST = 0,      /* Initial power-on self-test */
    STATE_IDLE = 1,               /* Waiting for sensor data */
    STATE_MEASURING = 2,          /* ADC conversion in progress */
    STATE_CALCULATING = 3,        /* Applying calibration and lux conversion */
    STATE_DISPLAY = 4,            /* Updating LCD with measurement */
    STATE_CALIBRATION = 5,        /* Calibration mode active */
    STATE_SLEEP = 6               /* Low power sleep mode */
} device_state_t;

/* LCD command packet structure (from decompiled code analysis) */
typedef struct {
    uint8_t cmd_code;             /* LCD command byte */
    uint8_t param_count;          /* Number of data bytes following */
    uint8_t params[16];           /* Command parameters */
} lcd_cmd_packet_t;

/* Sensor measurement data structure */
typedef struct {
    uint16_t adc_raw_light;       /* Raw ADC reading from light sensor (12-bit) */
    uint16_t adc_raw_temp;        /* Raw ADC reading from temperature sensor */
    uint16_t adc_ref_voltage;     /* Internal reference voltage measurement */
    fp32_t lux_value;             /* Calculated illuminance in lux */
    fp32_t temp_celsius;          /* Temperature in degrees Celsius */
    uint32_t timestamp;           /* Measurement timestamp */
} sensor_data_t;

/* Calibration coefficients stored in flash */
typedef struct {
    fp32_t light_offset;          /* Subtract from raw ADC reading */
    fp32_t light_scale;           /* Multiply after offset subtraction */
    fp32_t temp_offset;           /* Temperature calibration offset */
    uint16_t checksum;            /* Simple sum checksum for validation */
} calibration_data_t;

/* ============================================================================
 * FLOATING-POINT MATH LIBRARY (IEEE 754 Emulation)
 * 
 * These functions provide software floating-point arithmetic for the ARM
 * Cortex-M0+ which lacks hardware FPU. Each function follows IEEE 754
 * specification for rounding and special values (NaN, Inf).
 * ============================================================================ */

/**
 * fp32_add() - Add two single-precision floating-point numbers
 * @a: First operand (32-bit IEEE 754 format)
 * @b: Second operand (32-bit IEEE 754 format)
 * @return: Sum (a + b) with normalization
 * 
 * Implements IEEE 754 addition with:
 * - Exponent alignment
 * - Mantissa addition
 * - Rounding to nearest
 * - Denormalization detection
 */
void fp32_add(uint32_t param_a, uint32_t param_b);

/**
 * fp32_subtract() - Subtract two single-precision floating-point numbers
 * @a: Minuend operand
 * @b: Subtrahend operand
 * @return: Difference (a - b)
 * 
 * Implemented as: a + (-b) by negating b's sign bit
 */
void fp32_subtract(uint32_t param_a, uint32_t param_b);

/**
 * fp32_multiply() - Multiply two single-precision floating-point numbers
 * @a: First factor
 * @b: Second factor
 * @return: Product (a * b)
 * 
 * Complex mantissa multiplication with:
 * - 32x32 multiply generating 64-bit result
 * - Exponent addition with bias compensation
 * - Rounding and normalization
 */
uint32_t fp32_multiply(uint32_t param_a, uint32_t param_b);

/**
 * fp32_divide() - Divide two single-precision floating-point numbers
 * @a: Dividend
 * @b: Divisor
 * @return: Quotient (a / b)
 * 
 * Uses long division algorithm for mantissa division
 * - Exponent subtraction with bias correction
 * - 32-bit precision mantissa division
 * - Denormalization and rounding
 */
uint32_t fp32_divide(uint32_t param_a, uint32_t param_b);

/**
 * fp32_to_int() - Convert 32-bit float to signed 32-bit integer
 * @value: IEEE 754 float value
 * @return: Integer truncation (towards zero)
 * 
 * Extracts exponent to determine right-shift amount
 * Saturates to INT_MAX/INT_MIN if out of range
 */
uint32_t fp32_to_int(uint32_t param_value);

/**
 * fp32_to_fp64() - Convert 32-bit float to 64-bit double
 * @value: Single-precision IEEE 754
 * @return: Double-precision IEEE 754 (64-bit)
 * 
 * Expands precision from 23-bit to 52-bit mantissa
 * Maintains exponent with 11-bit format
 */
uint64_t fp32_to_fp64(uint32_t param_value);

/**
 * fp64_add() - Add two 64-bit double-precision floating-point numbers
 * @a_hi: High 32 bits of first operand
 * @a_lo: Low 32 bits of first operand
 * @b_hi: High 32 bits of second operand
 * @b_lo: Low 32 bits of second operand
 * @return: 64-bit sum
 * 
 * Handles full 52-bit mantissa precision
 * Exponent range: -1022 to +1023
 */
uint64_t fp64_add(uint32_t a_hi, uint32_t a_lo, uint32_t b_hi, uint32_t b_lo);

/**
 * fp64_multiply() - Multiply two 64-bit double numbers
 * Uses Karatsuba algorithm for efficient 64x64 multiply
 * Result: 128-bit intermediate, normalized to 64-bit
 */
uint64_t fp64_multiply(int32_t a_hi, uint32_t a_lo, int32_t b_hi, uint32_t b_lo);

/**
 * fp64_divide() - Divide two 64-bit double numbers
 * Implements long division with 64-bit operands
 * Precision: 52-bit mantissa
 */
uint64_t fp64_divide(uint32_t a_hi, uint32_t a_lo, uint32_t b_hi, uint32_t b_lo);

/**
 * fp64_to_int() - Convert 64-bit double to signed 32-bit integer
 * Bit manipulation based on exponent field
 * Handles denormalized numbers and saturation
 */
uint32_t fp64_to_int(uint32_t param_value);

/* ============================================================================
 * GPIO AND EXTERNAL INTERRUPT INTERFACE
 * 
 * Manages GPIO pins for:
 * - LCD control signals (CS, DC, RESET)
 * - Flash control signals (CS)
 * - Button input with debouncing
 * - Status LEDs (if present)
 * 
 * REGISTER LAYOUT (ARM Cortex-M0+ GPIOA/B/C):
 * +0x00: MODER   - Pin mode (2 bits per pin: 00=input, 01=output, 10=alt, 11=analog)
 * +0x04: OTYPER  - Output type (0=push-pull, 1=open-drain)
 * +0x08: OSPEEDR - Output speed (2 bits per pin)
 * +0x0C: PUPDR   - Pull-up/pull-down (2 bits per pin)
 * +0x10: IDR     - Input data register (read-only)
 * +0x14: ODR     - Output data register
 * +0x18: BSRR    - Bit set/reset register (write-only)
 * +0x1C: LCKR    - Lock register
 * +0x20: AFRL    - Alternate function for pins 0-7
 * +0x24: AFRH    - Alternate function for pins 8-15
 * ============================================================================ */

/**
 * gpio_configure_pins() - Configure GPIO port pins for input/output/alternate function
 * @port_base: GPIO port base address (0x50000000 + offset)
 * @pin_mask: Bitmask of pins to configure (bit N = pin N)
 * @mode: GPIO_MODE_INPUT/OUTPUT/ALTERNATE/ANALOG
 * @pull_config: GPIO_PULL_NONE/UP/DOWN
 * @speed: GPIO_SPEED_LOW/MEDIUM/HIGH
 * @alt_function: If mode=ALTERNATE, alternate function number (0-15)
 * @exti_config: Bit-field for EXTI setup:
 *   - Bit 16: Rising edge trigger enable
 *   - Bit 17: Falling edge trigger enable
 * 
 * Register modifications:
 * 1. MODER[pin*2:pin*2+1] = mode (2 bits)
 * 2. PUPDR[pin*2:pin*2+1] = pull (2 bits)
 * 3. OSPEEDR[pin*2:pin*2+1] = speed (2 bits)
 * 4. If ALTERNATE: AFRL/AFRH[pin*4:pin*4+3] = alt_function (4 bits)
 * 5. If EXTI: Configure EXTI controller for external interrupt
 */
void gpio_configure_pins(
    uint32_t *port_base,
    uint32_t pin_mask,
    uint32_t mode,
    uint32_t pull_config,
    uint32_t speed,
    uint32_t alt_function,
    uint32_t exti_config
);

/**
 * gpio_set_output_pins() - Set (drive high) GPIO output pins
 * @port_base: GPIO port base address
 * @pin_mask: Pins to set (bit N = set pin N to 1)
 * 
 * Register: ODR |= pin_mask (atomic via BSRR[0:15])
 */
void gpio_set_output_pins(uint32_t *port_base, uint32_t pin_mask);

/**
 * gpio_clear_output_pins() - Clear (drive low) GPIO output pins
 * @port_base: GPIO port base address
 * @pin_mask: Pins to clear (bit N = clear pin N to 0)
 * 
 * Register: ODR &= ~pin_mask (atomic via BSRR[16:31])
 */
void gpio_clear_output_pins(uint32_t *port_base, uint32_t pin_mask);

/**
 * gpio_read_input_pins() - Read GPIO input pin states
 * @port_base: GPIO port base address
 * @return: Input data register value (bit N = pin N state)
 * 
 * Register: return IDR (read-only input snapshot)
 */
uint32_t gpio_read_input_pins(uint32_t *port_base);

/**
 * gpio_read_specific_pin() - Read single GPIO pin state
 * @port_base: GPIO port base address
 * @pin_number: Pin index (0-15)
 * @return: 1 if pin high, 0 if pin low
 */
bool gpio_read_specific_pin(uint32_t *port_base, uint32_t pin_number);

/* ============================================================================
 * EXTERNAL INTERRUPT (EXTI) CONTROLLER
 * 
 * Manages external interrupt routing and edge triggering for GPIO pins.
 * EXTI allows GPIO pins to generate interrupts on rising/falling edges.
 * ============================================================================ */

/**
 * exti_setup_interrupt() - Configure external interrupt for GPIO pin
 * @exti_base: EXTI controller base address
 * @pin_number: GPIO pin number (0-15, maps to EXTI line)
 * @trigger_type: EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING | (both for both edges)
 * 
 * Configuration:
 * - Sets IMASK[pin] = 1 (enable interrupt)
 * - Sets EMASK[pin] = 0 (disable event)
 * - Sets RTCFG[pin] or FTCFG[pin] based on trigger_type
 */
void exti_setup_interrupt(
    uint32_t *exti_base,
    uint32_t pin_number,
    uint32_t trigger_type
);

/* ============================================================================
 * SPI INTERFACE (Hardware LVLSHIFT, controlled via GPIO)
 * 
 * SPI communication for:
 * 1. Flash Memory (SPI1): Read, write, erase operations
 * 2. LCD Display (SPI1): Command and data transfers
 * 
 * HARDWARE SPI REGISTERS (N32G031K8 SPI1 at 0x40013000):
 * +0x00: CTRL1 - Enable, clock divider, master/slave mode
 * +0x04: CTRL2 - Frame format, DMA enable
 * +0x08: STS   - Status flags (TXE, RXE, BSY, etc.)
 * +0x0C: DAT   - Data register (shift register)
 * +0x10: CRCPOLY - CRC polynomial
 * +0x14: CRCTDAT - CRC transmit data
 * +0x18: CRCRDAT - CRC receive data
 * +0x1C: I2SPRE - I2S prescaler (not used in this design)
 * 
 * PROTOCOL:
 * - Master mode, 4 MHz clock (APB clock / 8)
 * - CPOL=0, CPHA=0 (leading edge capture)
 * - 8-bit data frames
 * - MSB-first bit order
 * ============================================================================ */

/**
 * spi_configure() - Initialize SPI controller for flash and LCD communication
 * @spi_base: SPI base address (0x40013000 for SPI1)
 * @clock_divider: Clock division factor (1-8, resulting in fSPI = fAPB / (2^divider))
 * @enable: true to enable SPI, false to disable
 * 
 * Configuration:
 * - CTRL1: Enable bit, MSB-first, master mode
 * - CTRL2: 8-bit frames, DMA enabled for bulk transfers
 * - Clock divider sets communication speed
 */
void spi_configure(uint32_t *spi_base, uint32_t clock_divider, bool enable);

/**
 * spi_write_byte() - Send single byte via SPI and receive response
 * @spi_base: SPI base address
 * @data_byte: Byte to transmit
 * @return: Byte received during transmission
 * 
 * Polling sequence:
 * 1. Wait until TXE flag set (transmit buffer empty)
 * 2. Write data to DAT register
 * 3. Wait until RXE flag set (receive buffer has data)
 * 4. Read and return data from DAT register
 * 
 * Total time: ~2-3 microseconds at 4MHz SPI clock
 */
uint8_t spi_write_byte(uint32_t *spi_base, uint8_t data_byte);

/**
 * spi_read_status() - Poll SPI status register
 * @spi_base: SPI base address
 * @status_flag: Bit mask for status flag (e.g., TXE=2, RXE=1, BSY=0x80)
 * @return: true if flag is set, false otherwise
 * 
 * Used to poll:
 * - TXE (bit 1): Transmit buffer empty, ready to send
 * - RXE (bit 0): Receive buffer full, data ready
 * - BSY (bit 7): SPI busy in transaction
 */
bool spi_read_status(uint32_t *spi_base, uint32_t status_flag);

/**
 * spi_dma_setup() - Configure SPI for DMA transfers (used for LCD bulk image)
 * @spi_base: SPI base address
 * @enable: true to enable DMA mode, false to disable
 * 
 * Enables TXDMAEN bit in CTRL2 for DMA transmit mode
 * Used for LCD drawing: transfers 512-byte chunks to display
 */
void spi_dma_setup(uint32_t *spi_base, bool enable);

/**
 * spi_write_large_block() - Transfer large data block via SPI (used for flash writes)
 * @spi_base: SPI base address
 * @data_buffer: Pointer to data bytes to send
 * @length: Number of bytes to transfer
 * 
 * Sends data byte-by-byte, typically used for:
 * - Flash page writes (256 bytes at a time)
 * - Large LCD image transfers
 * 
 * Implements polling loop with TXE checks
 */
void spi_write_large_block(uint32_t *spi_base, const uint8_t *data_buffer, uint32_t length);

/**
 * spi_read_large_block() - Read large data block via SPI (used for flash reads)
 * @spi_base: SPI base address
 * @data_buffer: Pointer to receive buffer
 * @length: Number of bytes to read
 * 
 * Reads data byte-by-byte from SPI (typically flash memory)
 */
void spi_read_large_block(uint32_t *spi_base, uint8_t *data_buffer, uint32_t length);

/* ============================================================================
 * FLASH MEMORY INTERFACE (Winbond W25Q16 or compatible)
 * 
 * Manages SPI flash memory for:
 * 1. Calibration coefficient storage (read on startup)
 * 2. Saved measurement data (optional)
 * 3. Display images/patterns (for slideshow)
 * 
 * FLASH MEMORY MAP:
 * - 0x000000-0x000FFF: Reserved/boot
 * - 0x001000-0x001FFF: Calibration data (1 sector)
 * - 0x002000-0x1FFFFF: Application data/images
 * 
 * COMMUNICATION PROTOCOL:
 * 1. Set CS low (GPIO)
 * 2. Send command byte (0x03=read, 0x02=write, 0x20=erase)
 * 3. Send 3-byte address (MSB first)
 * 4. Send/receive data (255 bytes max for write)
 * 5. Poll status register until WIP=0
 * 6. Set CS high (GPIO)
 * ============================================================================ */

/**
 * flash_write_enable() - Set Write Enable Latch (WEL) bit
 * 
 * Sends command 0x06 to flash
 * Must be called before every write or erase operation
 * WEL bit is automatically cleared after write/erase completes
 * 
 * Sequence:
 * 1. CS low
 * 2. Send 0x06
 * 3. CS high
 */
void flash_write_enable(void);

/**
 * flash_read_status() - Read flash status register via command 0x05
 * @return: Status register value (bit 0=WIP, bit 1=WEL, bit 2-7=reserved)
 * 
 * Polls status to determine:
 * - WIP (bit 0): Write In Progress, wait if set
 * - WEL (bit 1): Write Enable Latch, confirms write enabled
 */
uint8_t flash_read_status(void);

/**
 * flash_verify_checksum() - Verify flash operation with checksum
 * 
 * Sends command 0x05 and checks for status pattern 0xA5
 * Used after write operations to confirm success
 * 
 * @return: true if checksum matches, false if verification failed
 */
bool flash_verify_checksum(void);

/**
 * flash_erase_sector() - Erase 4KB sector
 * @address: Start address of 4KB sector (must be 4KB aligned: 0x001000, 0x002000, etc.)
 * 
 * Erases entire 4KB sector (sets all bits to 1)
 * Takes ~400ms to complete
 * 
 * Sequence:
 * 1. flash_write_enable()
 * 2. CS low
 * 3. Send 0x20 (erase command)
 * 4. Send 3-byte address
 * 5. CS high
 * 6. Poll status register until WIP=0
 */
void flash_erase_sector(uint32_t address);

/**
 * flash_page_write() - Write up to 256 bytes to flash page
 * @address: Start address (must be page-aligned: 0xXXXX00, 0xXXXX100, etc.)
 * @data_buffer: Pointer to data bytes
 * @length: Number of bytes to write (0-256)
 * 
 * Writes data within single 256-byte page (does not wrap)
 * Takes ~2ms per page
 * 
 * Sequence:
 * 1. flash_write_enable()
 * 2. CS low
 * 3. Send 0x02 (page program command)
 * 4. Send 3-byte address
 * 5. Send data_length bytes
 * 6. CS high
 * 7. Poll status until WIP=0
 */
void flash_page_write(uint32_t address, const uint8_t *data_buffer, uint32_t length);

/**
 * flash_read_data() - Read arbitrary length data from flash
 * @address: Start address
 * @data_buffer: Pointer to receive buffer
 * @length: Number of bytes to read
 * 
 * Reads data sequentially from flash with no page boundaries
 * @1MHz SPI clock: reads ~1 byte per microsecond
 * 
 * Sequence:
 * 1. CS low
 * 2. Send 0x03 (read command)
 * 3. Send 3-byte address
 * 4. Read length bytes from SPI
 * 5. CS high
 */
void flash_read_data(uint32_t address, uint8_t *data_buffer, uint32_t length);

/**
 * flash_write_large_block() - Write data larger than 256 bytes
 * @start_address: Start address (must be 256-byte aligned)
 * @data_buffer: Pointer to data
 * @total_bytes: Total bytes to write (can be > 256)
 * 
 * Automatically handles page boundaries
 * Sends multiple flash_page_write() commands
 * 
 * Example: Writing 512 bytes sends 2x page_write calls
 */
void flash_write_large_block(uint32_t start_address, const uint8_t *data_buffer, uint32_t total_bytes);

/* ============================================================================
 * LCD DISPLAY DRIVER (ST7735R/NV3029 Compatible)
 * 
 * 128x160 color TFT display controlled via SPI and GPIO signals
 * 16-bit RGB565 color format
 * 
 * GPIO CONTROL SIGNALS:
 * - CS (Chip Select): GPIO, active low
 * - DC (Data/Command): GPIO, low=command, high=data
 * - RESET: GPIO, active low
 * 
 * SPI COMMUNICATION:
 * - Master sends 8-bit bytes at 4MHz
 * - No response expected from display
 * - Command/Data distinction via DC pin
 * 
 * COMMAND STRUCTURE:
 * Most commands follow pattern:
 * 1. DC=low, CS=low
 * 2. Send command byte (0xFF, 0x3E, 0x3A, etc.)
 * 3. Wait timing
 * 4. DC=high, send parameter bytes
 * 5. CS=high
 * 
 * COLOR FORMAT:
 * RGB565: [R7-R3][G7-G2][B7-B3] = 16-bit word
 * Example: Pure red = 0xF800, green = 0x07E0, blue = 0x001F
 * ============================================================================ */

/**
 * lcd_initialize() - Complete LCD initialization sequence (100+ commands)
 * 
 * Sends full manufacturer initialization:
 * 1. Hardware reset via RESET pin
 * 2. Unlock command set (0xFF, 0xA5)
 * 3. Display mode configuration (0x3E)
 * 4. Color format: 16-bit RGB565 (0x3A, 0x65)
 * 5. Frame rate control (0xB1, 0xB2, 0xB3)
 * 6. Power control (0xC3, 0xC4, 0xC5)
 * 7. Gamma correction curves (0xE0-0xE2): 30+ register writes
 * 8. Gamma correction (0x82-0xC3): 10+ values
 * 9. Additional vendor-specific (0x70-0xAF)
 * 10. Lock command set (0xFF, 0x00)
 * 11. Exit sleep mode (0x11)
 * 12. Display on (0x29)
 * 
 * Total initialization time: ~100ms
 * Critical timing: Must delay after reset, after sleep exit
 */
void lcd_initialize(void);

/**
 * lcd_send_command() - Send single LCD command byte
 * @cmd: Command byte (e.g., 0xFF for unlock, 0x3A for color format)
 * 
 * Protocol:
 * 1. DC=low (command mode)
 * 2. CS=low
 * 3. SPI send 8-bit command
 * 4. CS=high
 * 5. Small delay (~1-2 microseconds)
 * 
 * Note: Does not set DC=high or send parameters
 */
void lcd_send_command(uint8_t cmd);

/**
 * lcd_send_data() - Send single LCD data byte (parameter)
 * @data: Data byte (parameter for previous command)
 * 
 * Protocol:
 * 1. DC=high (data mode)
 * 2. CS=low
 * 3. SPI send 8-bit data
 * 4. CS=high
 * 
 * Must be called after lcd_send_command() for parameters
 */
void lcd_send_data(uint8_t data);

/**
 * lcd_set_window() - Configure display window (active drawing area)
 * @row_start: Start row (0-159)
 * @row_end: End row (0-159)
 * @col_start: Start column (0-127)
 * @col_end: End column (0-127)
 * 
 * Sets:
 * - Command 0x2A (Column Address Set): col_start, col_end
 * - Command 0x2B (Row Address Set): row_start, row_end
 * - Ready for Memory Write (0x2C)
 * 
 * Typical use: Set full screen 0,159,0,127 before drawing
 * Time: ~10 microseconds
 */
void lcd_set_window(uint8_t row_start, uint8_t row_end, uint8_t col_start, uint8_t col_end);

/**
 * lcd_draw_image() - Send image data to display via DMA
 * @image_data: Pointer to RGB565 pixel data
 * @pixel_count: Number of 16-bit pixels (width * height)
 * 
 * Transfers image via DMA for speed:
 * 1. Configure DMA channel for SPI transmit
 * 2. Set window to full screen (0,159,0,127)
 * 3. Send command 0x2C (Memory Write)
 * 4. Initiate DMA transfer from image_data
 * 5. Poll DMA completion flag
 * 
 * Time for 128x160 image: ~20ms at 4MHz SPI
 * Uses DMA channel to avoid CPU blocking
 * 
 * Image data format:
 * - Row-major (top-to-bottom)
 * - Each pixel: RGB565 (MSB=R, LSB=B)
 * - Total size: 128*160*2 = 40,960 bytes
 */
void lcd_draw_image(const uint16_t *image_data, uint32_t pixel_count);

/**
 * lcd_fill_color() - Fill entire display with solid color
 * @color: RGB565 color value
 * 
 * Fast clear operation:
 * 1. Set window to full screen
 * 2. Send 20,480 copies of same RGB565 color value
 * 3. Uses SPI repeated writes (not DMA)
 * 
 * Time: ~50ms at 4MHz SPI
 */
void lcd_fill_color(uint16_t color);

/**
 * lcd_display_on() - Power on display and set visible
 * Sends command 0x29 (Display ON)
 */
void lcd_display_on(void);

/**
 * lcd_display_off() - Power off display (black screen)
 * Sends command 0x28 (Display OFF)
 */
void lcd_display_off(void);

/**
 * lcd_enter_sleep() - Enter display sleep mode (minimal power)
 * Sends command 0x10 (Enter Sleep Mode)
 * Must wait 120ms before sending commands again
 */
void lcd_enter_sleep(void);

/**
 * lcd_exit_sleep() - Exit sleep mode and resume normal operation
 * Sends command 0x11 (Sleep Out)
 * Must wait 120ms before display is fully active
 */
void lcd_exit_sleep(void);

/* ============================================================================
 * DMA CONTROLLER (Direct Memory Access)
 * 
 * Accelerates memory-to-peripheral transfers for:
 * 1. LCD image transfers (memory to SPI)
 * 2. ADC data capture (ADC to memory)
 * 3. Reduces CPU load for large transfers
 * 
 * N32G031K8 has 5 DMA channels:
 * - Channel 1: ADC
 * - Channel 2: SPI1 RX (not used here)
 * - Channel 3: SPI1 TX (LCD transfers)
 * - Channel 4: USART
 * - Channel 5: Timer
 * 
 * DMA REGISTERS (at 0x40020000):
 * +0x00: ISR   - Interrupt status flags (4 bits per channel)
 * +0x04: IFCR  - Interrupt flag clear (write 1 to clear)
 * +0x08+n*14: CCRx  - Channel control register
 *   - Bit 0: EN (enable)
 *   - Bit 1: TCIE (transfer complete interrupt)
 *   - Bit 2: HTIE (half-transfer interrupt)
 *   - Bits 4-6: PL (priority: 00=low, 11=very high)
 *   - Bits 8-9: MSIZE (memory size: 00=8-bit, 01=16-bit, 10=32-bit)
 *   - Bits 10-11: PSIZE (peripheral size)
 *   - Bit 7: MINC (memory increment)
 *   - Bit 12: PINC (peripheral increment)
 *   - Bit 14: DIR (direction: 0=peripheral←memory, 1=memory←peripheral)
 * ============================================================================ */

/**
 * dma_configure_channel() - Set up DMA channel for transfer
 * @dma_base: DMA controller base address
 * @channel_num: Channel number (1-5)
 * @src_addr: Source address
 * @dst_addr: Destination address
 * @transfer_count: Number of transfer units
 * @config_flags: Control register settings (enable bit separate)
 * 
 * Configures:
 * - CCRx: Control register with size, increment, priority
 * - CMARx: Current memory address
 * - CPARx: Current peripheral address
 * - CNDTRx: Number of data items to transfer
 * 
 * Does NOT enable DMA (call dma_enable_channel() after)
 */
void dma_configure_channel(
    uint32_t *dma_base,
    uint32_t channel_num,
    uint32_t src_addr,
    uint32_t dst_addr,
    uint32_t transfer_count,
    uint32_t config_flags
);

/**
 * dma_enable_channel() - Start DMA transfer
 * @dma_base: DMA controller base address
 * @channel_num: Channel number (1-5)
 * @enable: true to start, false to stop
 * 
 * Sets or clears EN bit in CCRx register
 * Transfer starts immediately when enabled
 */
void dma_enable_channel(uint32_t *dma_base, uint32_t channel_num, bool enable);

/**
 * dma_is_transfer_complete() - Poll DMA transfer completion
 * @dma_base: DMA controller base address
 * @channel_num: Channel number (1-5)
 * @out_remaining: Pointer to store remaining transfer count
 * @return: true if transfer complete, false if in progress
 * 
 * Checks TCIF bit in ISR register
 * Also returns remaining count from CNDTR register
 * Used in polling loop to wait for transfer finish
 */
bool dma_is_transfer_complete(uint32_t *dma_base, uint32_t channel_num, uint32_t *out_remaining);

/**
 * dma_setup_interrupt() - Enable DMA interrupt generation
 * @dma_base: DMA controller base address
 * @channel_num: Channel number (1-5)
 * @enable_tc: true to enable transfer complete interrupt
 * @enable_ht: true to enable half-transfer interrupt
 * @enable_err: true to enable error interrupt
 * 
 * Allows CPU to be interrupted when DMA completes
 * Not used in this implementation (polling instead)
 */
void dma_setup_interrupt(
    uint32_t *dma_base,
    uint32_t channel_num,
    bool enable_tc,
    bool enable_ht,
    bool enable_err
);

/* ============================================================================
 * ADC INTERFACE (Analog-to-Digital Converter)
 * 
 * Measures:
 * 1. Ambient light sensor (ALS) on channel 0
 * 2. Internal temperature sensor
 * 3. Internal Vref (3.3V reference for calibration)
 * 
 * ADC CONFIGURATION:
 * - 12-bit resolution
 * - Single conversion mode (not continuous)
 * - Conversion rate: ~1MHz (12 cycles per conversion)
 * - Voltage reference: 3.3V (internal)
 * 
 * REGISTER LAYOUT (N32G031K8 ADC1 at 0x40012400):
 * +0x00: CTRL1 - Enable, interrupt enable, resolution
 * +0x04: CTRL2 - Start conversion, trigger selection
 * +0x08: SAMPT1 - Sample time for channels 0-9
 * +0x0C: SAMPT2 - Sample time for channels 10-17
 * +0x10: IOOFFSET1 - Input channel offset
 * +0x28: RSEQ1 - Regular sequence 1 (channels)
 * +0x30: DAT - Conversion result (12-bit)
 * ============================================================================ */

/**
 * adc_initialize() - Configure ADC for light and temperature measurements
 * 
 * Sets:
 * - 12-bit resolution
 * - Continuous conversion mode
 * - Vref enabled (internal 3.3V)
 * - Temperature sensor channel
 * - Sample time: 239.5 cycles (max precision)
 * 
 * Total conversion time: ~20 microseconds per sample
 */
void adc_initialize(void);

/**
 * adc_read_raw_value() - Get single ADC conversion result
 * @channel: ADC channel number (0-17)
 * @return: 12-bit ADC result (0-4095)
 * 
 * Reads result from DAT register
 * Does NOT trigger conversion (assumes running)
 * 
 * Raw value interpretation:
 * - Light sensor: Higher = brighter (typically 0-4000)
 * - Temperature: 1 LSB ≈ 0.043°C
 * - Vref: 1 LSB ≈ 0.8mV at 3.3V
 */
uint16_t adc_read_raw_value(uint32_t channel);

/**
 * adc_trigger_conversion() - Start ADC conversion on channel
 * @channel: ADC channel number (0-17)
 * 
 * Sets RSEQ1 to select channel and initiates conversion
 * Conversion takes ~20 microseconds
 * Result available via adc_read_raw_value()
 */
void adc_trigger_conversion(uint32_t channel);

/**
 * adc_measure_light_sensor() - Complete light measurement with calibration
 * @calibration: Pointer to calibration data structure
 * @out_lux: Pointer to store calculated lux value
 * @return: true on success, false on sensor error
 * 
 * Sequence:
 * 1. Trigger ADC conversion on light sensor channel
 * 2. Wait 110 microseconds for conversion
 * 3. Read raw ADC value
 * 4. Apply calibration offset and scaling
 * 5. Temperature compensate if temperature available
 * 6. Convert to lux using manufacturer lookup table
 * 
 * Time: ~150 microseconds total
 */
bool adc_measure_light_sensor(const calibration_data_t *calibration, float *out_lux);

/**
 * adc_measure_temperature() - Measure temperature via internal sensor
 * @out_temp_celsius: Pointer to store temperature in °C
 * @return: true on success, false on measurement error
 * 
 * Reads internal temperature sensor
 * Applies factory calibration points:
 * - 25°C reference point (from register)
 * - Temperature slope (1/T_SLOPE from register)
 * 
 * Accuracy: ±10°C typical
 */
bool adc_measure_temperature(float *out_temp_celsius);

/**
 * adc_measure_vref() - Measure internal 3.3V reference voltage
 * @out_voltage_mv: Pointer to store voltage in millivolts
 * @return: true on success
 * 
 * Reads Vref via dedicated ADC channel
 * Used to calibrate sensor measurements
 * Should be ~3300mV if power supply good
 */
bool adc_measure_vref(uint16_t *out_voltage_mv);

/**
 * adc_wait_conversion() - Busy-wait for conversion to complete
 * @timeout_us: Maximum wait time in microseconds
 * @return: true if conversion completed, false if timeout
 * 
 * Polls EOC (End Of Conversion) flag
 * Typical wait: ~20 microseconds
 */
bool adc_wait_conversion(uint32_t timeout_us);

/* ============================================================================
 * SENSOR MEASUREMENT AND CONVERSION
 * 
 * Converts raw ADC readings to physical units (lux, degrees Celsius)
 * Applies calibration coefficients stored in flash
 * ============================================================================ */

/**
 * sensor_adc_to_lux() - Convert raw ADC reading to illuminance in lux
 * @adc_raw: 12-bit ADC value from light sensor (0-4095)
 * @calibration: Pointer to calibration coefficients
 * @temperature_c: Optional temperature for compensation (-40 to +85°C)
 * @return: Illuminance in lux (0-100,000+)
 * 
 * Algorithm:
 * 1. Apply offset: adc_calibrated = adc_raw - calibration.light_offset
 * 2. Apply scale: adc_scaled = adc_calibrated * calibration.light_scale
 * 3. Temperature compensation: adjust scale by temperature coefficient
 * 4. Lux conversion: Use piecewise linear lookup or formula
 * 
 * Typical sensor response: ~0.5 lux per ADC count (varies by sensor)
 * Range: 0-100,000 lux (full range from darkness to bright sunlight)
 */
float sensor_adc_to_lux(uint16_t adc_raw, const calibration_data_t *calibration, float temperature_c);

/**
 * sensor_load_calibration() - Read calibration data from flash
 * @out_calibration: Pointer to structure to fill
 * @return: true if valid calibration loaded, false if checksum failed
 * 
 * Reads from fixed flash location (e.g., 0x001000)
 * Verifies checksum before accepting data
 * Falls back to default values if verification fails
 */
bool sensor_load_calibration(calibration_data_t *out_calibration);

/**
 * sensor_save_calibration() - Write calibration data to flash
 * @calibration: Pointer to calibration structure to save
 * @return: true on success, false on write error
 * 
 * Writes to fixed flash location with checksum
 * Erases sector first, then writes with verification
 */
bool sensor_save_calibration(const calibration_data_t *calibration);

/**
 * sensor_get_default_calibration() - Return factory default calibration
 * @return: Calibration structure with default values
 * 
 * Used at startup if stored calibration invalid
 * Values: offset=0, scale=0.5, temp_offset=0
 */
calibration_data_t sensor_get_default_calibration(void);

/* ============================================================================
 * DEVICE STATE MACHINE AND MAIN LOOP
 * 
 * Coordinates overall device operation:
 * 1. Power-on self-test (startup checks)
 * 2. Measurement loop (periodic ADC reads)
 * 3. Display updates (LCD refresh)
 * 4. Button handling (user input)
 * 5. Power management (sleep/wake)
 * ============================================================================ */

/**
 * device_power_on_test() - Verify device is functional at startup
 * 
 * Checks:
 * 1. Power button state (debounce for stability)
 * 2. Count button transitions (detect accidental boot)
 * 3. Flash memory accessibility (read status register)
 * 4. LCD communication (send test pattern)
 * 5. ADC functionality (read temperature)
 * 
 * If any check fails: enters infinite loop (watchdog will reset)
 * Success: transitions to idle state
 */
void device_power_on_test(void);

/**
 * device_main_loop() - Core measurement and display update loop
 * 
 * Runs continuously after startup:
 * 1. Check for button press (state transitions)
 * 2. Trigger ADC light sensor measurement
 * 3. Wait for conversion (110μs)
 * 4. Apply calibration and convert to lux
 * 5. Update display if changed (or on timer)
 * 6. Check for inactivity (sleep after 6 seconds)
 * 7. Return to step 1
 * 
 * Loop time: ~200ms (display update rate)
 * Can be interrupted by button press
 */
void device_main_loop(void);

/**
 * device_handle_button_press() - Process user button input
 * 
 * Debounces button by reading multiple times
 * Triggers state transitions:
 * - First press: Switch to next display screen
 * - Hold 2+ seconds: Enter calibration mode
 * - Hold 5+ seconds: Reset to defaults
 */
void device_handle_button_press(void);

/**
 * device_update_display() - Refresh LCD with current measurement
 * 
 * Displays measurement data:
 * 1. Lux value (numerical display)
 * 2. Graph or bar chart (optional)
 * 3. Temperature (if available)
 * 4. Status indicators
 * 
 * Only updates if value changed significantly (>5%)
 * Reduces LCD wear and power consumption
 */
void device_update_display(const sensor_data_t *measurement);

/**
 * device_check_activity() - Detect user input or sensor activity
 * @return: true if activity detected (button, change in light level), false if idle
 * 
 * Used for auto-sleep: if idle >6 seconds, device enters low-power mode
 */
bool device_check_activity(void);

/**
 * device_enter_sleep_mode() - Transition to low-power sleep mode
 * 
 * Disables:
 * 1. LCD display (saves ~80mW)
 * 2. ADC continuous conversion
 * 3. Unused GPIO outputs
 * 
 * Leaves enabled:
 * 1. Button interrupt (wakes device)
 * 2. Watchdog timer
 * 3. RTC (if present)
 * 
 * Power consumption: <5mW in sleep
 * Wake-up time: <100ms (LCD re-init)
 */
void device_enter_sleep_mode(void);

/**
 * device_exit_sleep_mode() - Wake from sleep and resume operation
 * 
 * Re-enables:
 * 1. ADC measurement
 * 2. LCD display (initialize)
 * 3. GPIO outputs
 * 4. Main loop
 * 
 * Takes ~120ms for LCD initialization
 */
void device_exit_sleep_mode(void);

/**
 * device_select_display_screen() - Change displayed content
 * @screen_id: Screen number (0=lux display, 1=graph, 2=settings, etc.)
 * 
 * Prepares appropriate image/format for LCD display
 * Calls device_update_display() to render
 */
void device_select_display_screen(uint32_t screen_id);

/* ============================================================================
 * CALIBRATION AND CONFIGURATION
 * 
 * User-facing functions for adjusting device response to match reference
 * measurements. Calibration data stored in flash for persistence.
 * ============================================================================ */

/**
 * device_enter_calibration_mode() - Start two-point calibration procedure
 * 
 * Sequence:
 * 1. Display "Calibration Mode" on LCD
 * 2. Prompt user to place device at known light level 1 (e.g., 100 lux)
 * 3. Read ADC, calculate offset coefficient
 * 4. Prompt for light level 2 (e.g., 1000 lux)
 * 5. Read ADC, calculate scale coefficient
 * 6. Save to flash
 * 7. Return to measurement mode
 * 
 * Improves accuracy from ±20% to ±5% typical
 */
void device_enter_calibration_mode(void);

/**
 * device_run_test_pattern() - Display test pattern on LCD for verification
 * 
 * Shows:
 * 1. Full black screen
 * 2. Full white screen
 * 3. Primary colors (red, green, blue)
 * 4. Gradient patterns
 * 5. Text pattern (if font data available)
 * 
 * Used during manufacturing test and troubleshooting
 */
void device_run_test_pattern(void);

/* ============================================================================
 * SYSTEM UTILITIES
 * ============================================================================ */

/**
 * delay_microseconds() - Busy-wait delay
 * @microseconds: Number of microseconds to wait
 * 
 * Simple loop delay based on CPU clock (~9MHz for N32G031K8)
 * Each iteration ≈ 0.1 microseconds
 * Not suitable for timing-critical code (use timer instead)
 */
void delay_microseconds(uint32_t microseconds);

/**
 * delay_milliseconds() - Millisecond delay wrapper
 * @milliseconds: Number of milliseconds to wait
 */
void delay_milliseconds(uint32_t milliseconds);

/**
 * watchdog_pet() - Reset watchdog timer
 * 
 * Must be called periodically (typically every 1 second)
 * If not called, watchdog resets the device after ~8 seconds
 * 
 * Prevents hung system from persisting indefinitely
 */
void watchdog_pet(void);

/**
 * get_system_tick() - Get current system time in milliseconds
 * @return: Milliseconds since device startup
 * 
 * Based on timer/RTC counter
 * Used for timing measurements and auto-sleep timeout
 */
uint32_t get_system_tick(void);

#endif /* XD0011_F_LIGHT_METER_DRIVER_H */
