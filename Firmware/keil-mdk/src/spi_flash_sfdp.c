/*****************************************************************************
 * @file spi_flash_sfdp.c
 * @brief SFDP (Serial Flash Discoverable Parameters) implementation
 * 
 * Implements JESD216 SFDP standard for automatic flash parameter discovery
 ****************************************************************************/

#include "spi_flash_sfdp.h"
#include <string.h>

/** @addtogroup SPI_FLASH_SFDP
 * @{
 */

/** @addtogroup SPI_FLASH_SFDP_Private_Functions
 * @{
 */

/**
 * @brief Read raw SFDP data from flash
 * @param address 24-bit SFDP address
 * @param buffer Buffer to store data
 * @param length Number of bytes to read
 * @return true on success
 */
bool SFDP_Read(uint32_t address, uint8_t* buffer, uint16_t length)
{
    if (buffer == NULL || length == 0)
        return false;
    
    /* Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    
    /* Send SFDP Read command */
    sFLASH_SendByte(SFDP_CMD_READ);
    
    /* Send 24-bit address */
    sFLASH_SendByte((address >> 16) & 0xFF);
    sFLASH_SendByte((address >> 8) & 0xFF);
    sFLASH_SendByte(address & 0xFF);
    
    /* Send dummy byte (required by SFDP spec) */
    sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    
    /* Read data bytes */
    for (uint16_t i = 0; i < length; i++)
    {
        buffer[i] = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
    }
    
    /* Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
    
    return true;
}

/**
 * @brief Check if flash supports SFDP
 * @return true if SFDP signature found
 */
bool SFDP_IsSupported(void)
{
    uint8_t header[4];
    uint32_t signature;
    
    if (!SFDP_Read(SFDP_HEADER_ADDR, header, 4))
        return false;
    
    /* Check for SFDP signature (little-endian: 0x50, 0x44, 0x46, 0x53 = "SFDP") */
    signature = header[0] | (header[1] << 8) | (header[2] << 16) | (header[3] << 24);
    
    return (signature == SFDP_SIGNATURE);
}

/**
 * @brief Parse SFDP header
 * @param header_data Raw header data (16 bytes)
 * @param header Pointer to header structure to fill
 */
static void SFDP_ParseHeader(const uint8_t* header_data, SFDP_Header_t* header)
{
    header->signature = header_data[0] | (header_data[1] << 8) | 
                       (header_data[2] << 16) | (header_data[3] << 24);
    header->minor_rev = header_data[4];
    header->major_rev = header_data[5];
    header->nph = header_data[6];
    header->access_protocol = header_data[7];
}

/**
 * @brief Parse SFDP parameter header
 * @param param_data Raw parameter header data (8 bytes)
 * @param param_header Pointer to parameter header structure to fill
 */
static void SFDP_ParseParamHeader(const uint8_t* param_data, SFDP_ParamHeader_t* param_header)
{
    param_header->id_lsb = param_data[0];
    param_header->minor_rev = param_data[1];
    param_header->major_rev = param_data[2];
    param_header->length = param_data[3];
    param_header->table_address = param_data[4] | (param_data[5] << 8) | (param_data[6] << 16);
    param_header->id_msb = param_data[7];
}

/**
 * @brief Parse Basic Flash Parameter Table
 * @param bfpt_data Raw BFPT data
 * @param length Length in DWORDs
 * @param params SFDP parameters structure to fill
 */
static void SFDP_ParseBFPT(const uint32_t* bfpt_data, uint8_t length, SFDP_Params_t* params)
{
    uint32_t dword;
    
    /* Store raw BFPT data */
    params->bfpt_length = (length > 20) ? 20 : length;
    memcpy(params->bfpt_dwords, bfpt_data, params->bfpt_length * 4);
    
    /* DWORD 1: Block/Sector Erase Sizes */
    if (length >= 1)
    {
        dword = bfpt_data[0];
        
        /* Erase Type 1 (typically 4KB sector) */
        params->erase_type[0].size_exp = dword & 0xFF;
        params->erase_type[0].opcode = (dword >> 8) & 0xFF;
        
        /* Erase Type 2 */
        params->erase_type[1].size_exp = (dword >> 16) & 0xFF;
        params->erase_type[1].opcode = (dword >> 24) & 0xFF;
    }
    
    /* DWORD 2: More erase sizes and density */
    if (length >= 2)
    {
        dword = bfpt_data[1];
        
        /* Flash Memory Density */
        if (dword & 0x80000000)
        {
            /* Density >= 4 Gbit, encoded as 2^N bits */
            uint32_t n = dword & 0x7FFFFFFF;
            params->density_bits = (1ULL << n);
        }
        else
        {
            /* Density < 4 Gbit, encoded as (N+1) bits */
            params->density_bits = (uint64_t)(dword & 0x7FFFFFFF) + 1;
        }
        params->density_bytes = params->density_bits / 8;
    }
    
    /* DWORD 3: Erase Types 3 & 4 */
    if (length >= 3)
    {
        dword = bfpt_data[2];
        
        /* Erase Type 3 */
        params->erase_type[2].size_exp = dword & 0xFF;
        params->erase_type[2].opcode = (dword >> 8) & 0xFF;
        
        /* Erase Type 4 */
        params->erase_type[3].size_exp = (dword >> 16) & 0xFF;
        params->erase_type[3].opcode = (dword >> 24) & 0xFF;
    }
    
    /* Calculate erase sizes and count types */
    params->erase_type_count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (params->erase_type[i].size_exp != 0)
        {
            params->erase_type[i].size_bytes = (1UL << params->erase_type[i].size_exp);
            params->erase_type_count++;
            
            /* Identify common sector/block sizes */
            if (params->erase_type[i].size_bytes == 4096)
            {
                params->sector_size = 4096;
                params->sector_erase_opcode = params->erase_type[i].opcode;
            }
            else if (params->erase_type[i].size_bytes == 65536)
            {
                params->block_size = 65536;
                params->block_erase_opcode = params->erase_type[i].opcode;
            }
        }
    }
    
    /* DWORD 4: Erase timing */
    if (length >= 4)
    {
        dword = bfpt_data[3];
        
        /* Erase type 1 timing (bits 0-10) */
        uint32_t count = (dword & 0x1F) + 1;
        uint32_t units = (dword >> 5) & 0x3;
        uint32_t multiplier = (units == 0) ? 1 : (units == 1) ? 16 : (units == 2) ? 128 : 1000;
        params->erase_type[0].typ_time_ms = count * multiplier;
        params->erase_type[0].max_time_ms = params->erase_type[0].typ_time_ms * 2;
    }
    
    /* DWORD 11: Page Size and Chip Erase Time */
    if (length >= 11)
    {
        dword = bfpt_data[10];
        
        /* Page size (bits 4-7) */
        uint8_t n = (dword >> 4) & 0x0F;
        if (n == 0)
            params->page_size = 256;  /* Default */
        else
            params->page_size = 1 << n;
        
        /* Chip erase time */
        uint32_t count = ((dword >> 24) & 0x1F) + 1;
        uint32_t units = (dword >> 29) & 0x3;
        uint32_t multiplier = (units == 0) ? 16 : (units == 1) ? 256 : (units == 2) ? 4000 : 64000;
        params->chip_erase_typ_ms = count * multiplier;
        params->chip_erase_max_ms = params->chip_erase_typ_ms * 2;
    }
    else
    {
        params->page_size = 256;  /* Default page size */
    }
    
    /* DWORD 12: Program Time */
    if (length >= 12)
    {
        dword = bfpt_data[11];
        
        /* Page program time (first byte) */
        uint32_t count = (dword & 0x1F) + 1;
        uint32_t units = (dword >> 5) & 0x1;
        params->page_prog_typ_us = count * (units ? 64 : 8);
        params->page_prog_max_us = params->page_prog_typ_us * 2;
    }
    
    /* DWORD 16: Address Bytes (JESD216A and later) */
    if (length >= 16)
    {
        dword = bfpt_data[15];
        
        /* Check for 3-byte and 4-byte address support */
        params->supports_3byte = true;  /* Always assume 3-byte support */
        params->supports_4byte = (dword & 0x01) || (dword & 0x02);
        
        /* Determine current addressing mode based on density */
        if (params->density_bytes > 0x1000000)  /* > 16MB */
            params->address_bytes = 4;
        else
            params->address_bytes = 3;
    }
    else
    {
        params->address_bytes = 3;
        params->supports_3byte = true;
        params->supports_4byte = false;
    }
    
    /* Fast Read Support - check DWORD 1 bits */
    if (length >= 1)
    {
        dword = bfpt_data[0];
        params->supports_fast_read_112 = (dword & (1 << 16)) ? true : false;
        params->supports_fast_read_144 = (dword & (1 << 21)) ? true : false;
        params->supports_fast_read_114 = (dword & (1 << 22)) ? true : false;
        params->supports_fast_read_122 = (dword & (1 << 20)) ? true : false;
        params->fast_read_dummy_clocks = 8;  /* Common default */
    }
}

/**
 * @brief Read and parse SFDP parameters
 * @param params Pointer to SFDP_Params_t structure to fill
 * @return true if successful
 */
bool SFDP_ReadAndParse(SFDP_Params_t* params)
{
    uint8_t header_data[16];
    SFDP_Header_t sfdp_header;
    
    if (params == NULL)
        return false;
    
    /* Initialize structure */
    memset(params, 0, sizeof(SFDP_Params_t));
    
    /* Read SFDP header (8 bytes minimum, but read 16 for alignment) */
    if (!SFDP_Read(SFDP_HEADER_ADDR, header_data, 16))
    {
        return false;
    }
    
    /* Parse SFDP header */
    SFDP_ParseHeader(header_data, &sfdp_header);
    
    /* Validate SFDP signature */
    if (sfdp_header.signature != SFDP_SIGNATURE)
    {
        return false;
    }
    
    /* Store version info */
    params->sfdp_major = sfdp_header.major_rev;
    params->sfdp_minor = sfdp_header.minor_rev;
    params->param_count = sfdp_header.nph + 1;
    
    /* Read parameter headers (start at byte 8) */
    /* We're interested in the Basic Flash Parameter Table (first one) */
    uint8_t param_header_data[8];
    if (!SFDP_Read(SFDP_HEADER_ADDR + 8, param_header_data, 8))
    {
        return false;
    }
    
    SFDP_ParamHeader_t param_header;
    SFDP_ParseParamHeader(param_header_data, &param_header);
    
    /* Read Basic Flash Parameter Table */
    uint16_t param_id = (param_header.id_msb << 8) | param_header.id_lsb;
    if (param_id == SFDP_PARAM_ID_BASIC)
    {
        /* Read BFPT data (in DWORDs, little-endian) */
        uint8_t bfpt_length = param_header.length;
        uint32_t bfpt_data[20];  /* Max 20 DWORDs per JESD216 */
        
        if (bfpt_length > 20)
            bfpt_length = 20;
        
        uint8_t* bfpt_bytes = (uint8_t*)bfpt_data;
        if (!SFDP_Read(param_header.table_address, bfpt_bytes, bfpt_length * 4))
        {
            return false;
        }
        
        /* Convert to DWORDs (already little-endian on most systems) */
        for (int i = 0; i < bfpt_length; i++)
        {
            bfpt_data[i] = bfpt_bytes[i*4] | 
                          (bfpt_bytes[i*4 + 1] << 8) |
                          (bfpt_bytes[i*4 + 2] << 16) |
                          (bfpt_bytes[i*4 + 3] << 24);
        }
        
        /* Parse BFPT */
        SFDP_ParseBFPT(bfpt_data, bfpt_length, params);
        
        params->valid = true;
        return true;
    }
    
    return false;
}

/**
 * @}
 */

/** @addtogroup SPI_FLASH_SFDP_Helper_Functions
 * @{
 */

/**
 * @brief Get flash density in bytes
 */
uint32_t SFDP_GetDensityBytes(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 0;
    
    return params->density_bytes;
}

/**
 * @brief Get flash density in megabits
 */
uint32_t SFDP_GetDensityMegabits(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 0;
    
    return (uint32_t)(params->density_bits / (1024 * 1024));
}

/**
 * @brief Get page size
 */
uint16_t SFDP_GetPageSize(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 256;  /* Default */
    
    return params->page_size;
}

/**
 * @brief Get sector size
 */
uint32_t SFDP_GetSectorSize(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 4096;  /* Default 4KB */
    
    return params->sector_size > 0 ? params->sector_size : 4096;
}

/**
 * @brief Get sector erase opcode
 */
uint8_t SFDP_GetSectorEraseOpcode(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 0x20;  /* Default */
    
    return params->sector_erase_opcode > 0 ? params->sector_erase_opcode : 0x20;
}

/**
 * @brief Get block size
 */
uint32_t SFDP_GetBlockSize(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 65536;  /* Default 64KB */
    
    return params->block_size > 0 ? params->block_size : 65536;
}

/**
 * @brief Get block erase opcode
 */
uint8_t SFDP_GetBlockEraseOpcode(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 0xD8;  /* Default */
    
    return params->block_erase_opcode > 0 ? params->block_erase_opcode : 0xD8;
}

/**
 * @brief Get number of address bytes
 */
uint8_t SFDP_GetAddressBytes(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 3;  /* Default */
    
    return params->address_bytes;
}

/**
 * @brief Check fast read support
 */
bool SFDP_SupportsFastRead(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return false;
    
    return params->supports_fast_read_112 || 
           params->supports_fast_read_122 ||
           params->supports_fast_read_114 || 
           params->supports_fast_read_144;
}

/**
 * @brief Get page program time
 */
uint32_t SFDP_GetPageProgramTime(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 3000;  /* Default 3ms */
    
    return params->page_prog_typ_us > 0 ? params->page_prog_typ_us : 3000;
}

/**
 * @brief Get chip erase time
 */
uint32_t SFDP_GetChipEraseTime(const SFDP_Params_t* params)
{
    if (params == NULL || !params->valid)
        return 10000;  /* Default 10 seconds */
    
    return params->chip_erase_typ_ms > 0 ? params->chip_erase_typ_ms : 10000;
}

/**
 * @brief Get erase type by index
 */
const SFDP_EraseType_t* SFDP_GetEraseType(const SFDP_Params_t* params, uint8_t index)
{
    if (params == NULL || !params->valid || index >= 4)
        return NULL;
    
    if (params->erase_type[index].size_bytes == 0)
        return NULL;
    
    return &params->erase_type[index];
}

/**
 * @brief Find best erase type for size
 */
const SFDP_EraseType_t* SFDP_FindBestEraseType(const SFDP_Params_t* params, uint32_t size)
{
    if (params == NULL || !params->valid)
        return NULL;
    
    /* Find largest erase size that fits the requested size */
    const SFDP_EraseType_t* best = NULL;
    
    for (int i = 0; i < 4; i++)
    {
        if (params->erase_type[i].size_bytes > 0 && 
            params->erase_type[i].size_bytes <= size)
        {
            if (best == NULL || params->erase_type[i].size_bytes > best->size_bytes)
            {
                best = &params->erase_type[i];
            }
        }
    }
    
    return best;
}

/**
 * @brief Print SFDP parameters (debug helper)
 * Note: Requires printf or similar output function
 */
void SFDP_PrintParams(const SFDP_Params_t* params)
{
    if (params == NULL)
        return;
    
    /* This is a placeholder - implement with your debug output method */
    /* Example using printf (if available):
    
    printf("\n=== SFDP Parameters ===\n");
    printf("SFDP Version: %d.%d\n", params->sfdp_major, params->sfdp_minor);
    printf("Parameter Headers: %d\n", params->param_count);
    printf("\nFlash Density:\n");
    printf("  Bits: %llu\n", params->density_bits);
    printf("  Bytes: %u (%.2f MB)\n", params->density_bytes, params->density_bytes / (1024.0 * 1024.0));
    printf("  Megabits: %u\n", SFDP_GetDensityMegabits(params));
    printf("\nAddressing:\n");
    printf("  Address Bytes: %d\n", params->address_bytes);
    printf("  3-Byte Support: %s\n", params->supports_3byte ? "Yes" : "No");
    printf("  4-Byte Support: %s\n", params->supports_4byte ? "Yes" : "No");
    printf("\nPage Programming:\n");
    printf("  Page Size: %d bytes\n", params->page_size);
    printf("  Typical Time: %u us\n", params->page_prog_typ_us);
    printf("\nErase Operations:\n");
    for (int i = 0; i < params->erase_type_count; i++)
    {
        printf("  Type %d: Size=%u KB, Opcode=0x%02X, Time=%u ms\n", 
               i, params->erase_type[i].size_bytes / 1024,
               params->erase_type[i].opcode,
               params->erase_type[i].typ_time_ms);
    }
    printf("  Sector: %u bytes, Opcode=0x%02X\n", params->sector_size, params->sector_erase_opcode);
    printf("  Block: %u bytes, Opcode=0x%02X\n", params->block_size, params->block_erase_opcode);
    printf("  Chip Erase Time: %u ms\n", params->chip_erase_typ_ms);
    printf("\nFast Read Support:\n");
    printf("  1-1-2: %s\n", params->supports_fast_read_112 ? "Yes" : "No");
    printf("  1-2-2: %s\n", params->supports_fast_read_122 ? "Yes" : "No");
    printf("  1-1-4: %s\n", params->supports_fast_read_114 ? "Yes" : "No");
    printf("  1-4-4: %s\n", params->supports_fast_read_144 ? "Yes" : "No");
    printf("====================\n\n");
    
    */
}

/**
 * @}
 */

/**
 * @}
 */
