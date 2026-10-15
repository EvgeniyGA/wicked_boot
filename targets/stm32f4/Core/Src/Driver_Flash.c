#include "Driver_Flash.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

static ARM_DRIVER_VERSION DriverVersion = { ARM_FLASH_API_VERSION, ARM_FLASH_DRV_VERSION };
static ARM_FLASH_CAPABILITIES DriverCapabilities = { 
    .event_ready = 1, .data_width = 4 // 32-bit (4 bytes) programming
};

// Вспомогательная функция: абсолютный адрес -> номер сектора STM32F4
static uint32_t GetSectorFromAddress(uint32_t Address) {
    if (Address < 0x08010000) return (Address - 0x08000000) / 0x4000;
    if (Address < 0x08020000) return 4;
    if (Address < 0x08100000) return 5 + (Address - 0x08020000) / 0x20000;
    return 0xFF;
}

static ARM_DRIVER_VERSION Flash_GetVersion(void) { return DriverVersion; }
static ARM_FLASH_CAPABILITIES Flash_GetCapabilities(void) { return DriverCapabilities; }
static int32_t Flash_Initialize(ARM_Flash_SignalEvent_t cb_event) { (void)cb_event; return ARM_DRIVER_OK; }
static int32_t Flash_Uninitialize(void) { return ARM_DRIVER_OK; }
static int32_t Flash_PowerControl(ARM_POWER_STATE state) { (void)state; return ARM_DRIVER_OK; }

static int32_t Flash_ReadData(uint32_t addr, void *data, uint32_t cnt) {
    memcpy(data, (const void *)addr, cnt);
    return ARM_DRIVER_OK;
}

static int32_t Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt) {
    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < cnt; i += 4) {
        uint32_t word = *((const uint32_t *)((const uint8_t *)data + i));
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + i, word) != HAL_OK) {
            HAL_FLASH_Lock();
            return ARM_DRIVER_ERROR;
        }
    }
    HAL_FLASH_Lock();
    return ARM_DRIVER_OK;
}

static int32_t Flash_EraseSector(uint32_t addr) {
    uint32_t sector = GetSectorFromAddress(addr);
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Banks = FLASH_BANK_1,
        .Sector = sector,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t sector_error = 0;
    
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    HAL_FLASH_Lock();
    
    return (status == HAL_OK) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static int32_t Flash_EraseChip(void) {
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase = FLASH_TYPEERASE_MASSERASE,
        .Banks = FLASH_BANK_1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t sector_error = 0;
    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    HAL_FLASH_Lock();
    return (status == HAL_OK) ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static ARM_FLASH_STATUS Flash_GetStatus(void) {
    ARM_FLASH_STATUS status = {0, 0, 0, 0, 0};
    status.busy = (FLASH->SR & FLASH_SR_BSY) ? 1 : 0;
    return status;
}

static ARM_FLASH_INFO Flash_GetInfo(void) {
    static ARM_FLASH_INFO info = {
        .sector_count = 12,
        .sector_size = 0x20000, // Усредненно, детали секторов MCUboot берет из flash_map
        .page_size = 4,
        .program_unit = 4,
        .erased_value = 0xFF
    };
    return info;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    .GetVersion     = Flash_GetVersion,
    .GetCapabilities= Flash_GetCapabilities,
    .Initialize     = Flash_Initialize,
    .Uninitialize   = Flash_Uninitialize,
    .PowerControl   = Flash_PowerControl,
    .ReadData       = Flash_ReadData,
    .ProgramData    = Flash_ProgramData,
    .EraseSector    = Flash_EraseSector,
    .EraseChip      = Flash_EraseChip,
    .GetStatus      = Flash_GetStatus,
    .GetInfo        = Flash_GetInfo
};