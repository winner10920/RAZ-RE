/*****************************************************************************
 * @file spi_flash_sfdp.h
 * @brief SFDP (Serial Flash Discoverable Parameters) support for SPI flash
 * 
 * Implements JESD216 SFDP standard for automatic flash parameter discovery
 ****************************************************************************/

#ifndef __SPI_FLASH_SFDP_H__
#define __SPI_FLASH_SFDP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "spi_flash.h"
#include <stdbool.h>

/** @addtogroup SPI_FLASH_SFDP
 * @{
 */

/**
 * @brief SFDP Command and Header Definitions
 */
#define SFDP_CMD_READ           0x5A    /*!< Read SFDP command */
#define SFDP_SIGNATURE          0x50444653  /*!< "SFDP" signature in little-endian */
#define SFDP_HEADER_ADDR        0x000000    /*!< SFDP header address */

/**
 * @brief SFDP Parameter IDs
 */
#define SFDP_PARAM_ID_BASIC     0xFF00  /*!< Basic Flash Parameter Table ID */
#define SFDP_PARAM_ID_4BYTE     0xFF84  /*!< 4-Byte Address Instruction Table */
#define SFDP_PARAM_ID_XSPI_V1   0xFF05  /*!< xSPI Profile 1.0 Table */

/**
 * @brief SFDP Erase Type Structure
 */
typedef struct {
    uint8_t  size_exp;      /*!< Erase size as 2^N bytes (0 = not supported) */
    uint8_t  opcode;        /*!< Erase opcode */
    uint32_t size_bytes;    /*!< Calculated erase size in bytes */
    uint32_t typ_time_ms;   /*!< Typical erase time in milliseconds */
    uint32_t max_time_ms;   /*!< Maximum erase time in milliseconds */
} SFDP_EraseType_t;

/**
 * @brief SFDP Flash Parameters Structure
 * Contains parsed parameters from SFDP Basic Flash Parameter Table
 */
typedef struct {
    /* SFDP Header Info */
    uint8_t  sfdp_major;        /*!< SFDP major version */
    uint8_t  sfdp_minor;        /*!< SFDP minor version */
    uint8_t  param_count;       /*!< Number of parameter headers */
    
    /* Flash Density */
    uint64_t density_bits;      /*!< Flash density in bits */
    uint32_t density_bytes;     /*!< Flash density in bytes */
    
    /* Addressing */
    uint8_t  address_bytes;     /*!< Number of address bytes (3 or 4) */
    bool     supports_3byte;    /*!< Supports 3-byte addressing */
    bool     supports_4byte;    /*!< Supports 4-byte addressing */
    
    /* Page Programming */
    uint16_t page_size;         /*!< Page programming size in bytes */
    uint32_t page_prog_typ_us;  /*!< Typical page program time in microseconds */
    uint32_t page_prog_max_us;  /*!< Maximum page program time in microseconds */
    
    /* Erase Operations */
    SFDP_EraseType_t erase_type[4];  /*!< Up to 4 erase types supported */
    uint8_t  erase_type_count;       /*!< Number of erase types available */
    
    /* Sector Information */
    uint32_t sector_size;       /*!< Typical sector size (4KB default) */
    uint8_t  sector_erase_opcode; /*!< Sector erase opcode */
    
    /* Block Information */
    uint32_t block_size;        /*!< Typical block size (64KB default) */
    uint8_t  block_erase_opcode; /*!< Block erase opcode */
    
    /* Fast Read Support */
    bool     supports_fast_read_112; /*!< 1-1-2 Fast Read */
    bool     supports_fast_read_122; /*!< 1-2-2 Fast Read */
    bool     supports_fast_read_114; /*!< 1-1-4 Fast Read */
    bool     supports_fast_read_144; /*!< 1-4-4 Fast Read */
    uint8_t  fast_read_dummy_clocks; /*!< Number of dummy clocks for fast read */
    
    /* Status Register */
    bool     volatile_sr_write;      /*!< Volatile status register write enable */
    bool     write_enable_required;  /*!< Write enable instruction required */
    
    /* Chip Erase */
    uint32_t chip_erase_typ_ms;  /*!< Typical chip erase time in milliseconds */
    uint32_t chip_erase_max_ms;  /*!< Maximum chip erase time in milliseconds */
    
    /* Raw BFPT Data (for advanced usage) */
    uint32_t bfpt_dwords[20];    /*!< Raw Basic Flash Parameter Table (up to 20 DWORDs) */
    uint8_t  bfpt_length;        /*!< Actual BFPT length in DWORDs */
    
    /* Validity */
    bool     valid;              /*!< True if SFDP was successfully read and parsed */
    
} SFDP_Params_t;

/**
 * @brief SFDP Header Structure (internal use)
 */
typedef struct {
    uint32_t signature;         /*!< Should be 0x50444653 "SFDP" */
    uint8_t  minor_rev;         /*!< SFDP minor revision */
    uint8_t  major_rev;         /*!< SFDP major revision */
    uint8_t  nph;               /*!< Number of parameter headers (0-based) */
    uint8_t  access_protocol;   /*!< SFDP access protocol */
} SFDP_Header_t;

/**
 * @brief SFDP Parameter Header Structure (internal use)
 */
typedef struct {
    uint8_t  id_lsb;            /*!< Parameter ID LSB */
    uint8_t  minor_rev;         /*!< Parameter minor revision */
    uint8_t  major_rev;         /*!< Parameter major revision */
    uint8_t  length;            /*!< Parameter table length in DWORDs */
    uint32_t table_address;     /*!< Parameter table address (24-bit) */
    uint8_t  id_msb;            /*!< Parameter ID MSB */
} SFDP_ParamHeader_t;

/**
 * @addtogroup SPI_FLASH_SFDP_Functions
 * @{
 */

/**
 * @brief Read SFDP data from flash
 * @param address SFDP address to read from
 * @param buffer Buffer to store read data
 * @param length Number of bytes to read
 * @return true if read successful, false otherwise
 */
bool SFDP_Read(uint32_t address, uint8_t* buffer, uint16_t length);

/**
 * @brief Read and parse SFDP parameters from flash
 * @param params Pointer to SFDP_Params_t structure to fill
 * @return true if SFDP read and parse successful, false otherwise
 */
bool SFDP_ReadAndParse(SFDP_Params_t* params);

/**
 * @brief Check if flash supports SFDP
 * @return true if SFDP signature is valid, false otherwise
 */
bool SFDP_IsSupported(void);

/**
 * @brief Get flash memory density in bytes
 * @param params Pointer to SFDP parameters
 * @return Flash density in bytes (0 if invalid)
 */
uint32_t SFDP_GetDensityBytes(const SFDP_Params_t* params);

/**
 * @brief Get flash memory density in megabits
 * @param params Pointer to SFDP parameters
 * @return Flash density in megabits (0 if invalid)
 */
uint32_t SFDP_GetDensityMegabits(const SFDP_Params_t* params);

/**
 * @brief Get page size for programming
 * @param params Pointer to SFDP parameters
 * @return Page size in bytes (default 256 if not specified)
 */
uint16_t SFDP_GetPageSize(const SFDP_Params_t* params);

/**
 * @brief Get sector size (typically 4KB)
 * @param params Pointer to SFDP parameters
 * @return Sector size in bytes
 */
uint32_t SFDP_GetSectorSize(const SFDP_Params_t* params);

/**
 * @brief Get sector erase opcode
 * @param params Pointer to SFDP parameters
 * @return Sector erase opcode (0x20 by default)
 */
uint8_t SFDP_GetSectorEraseOpcode(const SFDP_Params_t* params);

/**
 * @brief Get block size (typically 64KB)
 * @param params Pointer to SFDP parameters
 * @return Block size in bytes
 */
uint32_t SFDP_GetBlockSize(const SFDP_Params_t* params);

/**
 * @brief Get block erase opcode
 * @param params Pointer to SFDP parameters
 * @return Block erase opcode (0xD8 by default)
 */
uint8_t SFDP_GetBlockEraseOpcode(const SFDP_Params_t* params);

/**
 * @brief Get number of address bytes required
 * @param params Pointer to SFDP parameters
 * @return Number of address bytes (3 or 4)
 */
uint8_t SFDP_GetAddressBytes(const SFDP_Params_t* params);

/**
 * @brief Check if flash supports fast read mode
 * @param params Pointer to SFDP parameters
 * @return true if any fast read mode is supported
 */
bool SFDP_SupportsFastRead(const SFDP_Params_t* params);

/**
 * @brief Get typical page program time in microseconds
 * @param params Pointer to SFDP parameters
 * @return Typical page program time in microseconds
 */
uint32_t SFDP_GetPageProgramTime(const SFDP_Params_t* params);

/**
 * @brief Get typical chip erase time in milliseconds
 * @param params Pointer to SFDP parameters
 * @return Typical chip erase time in milliseconds
 */
uint32_t SFDP_GetChipEraseTime(const SFDP_Params_t* params);

/**
 * @brief Get erase type information by index
 * @param params Pointer to SFDP parameters
 * @param index Erase type index (0-3)
 * @return Pointer to erase type structure, NULL if invalid index
 */
const SFDP_EraseType_t* SFDP_GetEraseType(const SFDP_Params_t* params, uint8_t index);

/**
 * @brief Find best erase type for a given size
 * @param params Pointer to SFDP parameters
 * @param size Desired erase size in bytes
 * @return Pointer to best matching erase type, NULL if none found
 */
const SFDP_EraseType_t* SFDP_FindBestEraseType(const SFDP_Params_t* params, uint32_t size);

/**
 * @brief Print SFDP parameters to debug output (if enabled)
 * @param params Pointer to SFDP parameters
 */
void SFDP_PrintParams(const SFDP_Params_t* params);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_SFDP_H__ */
