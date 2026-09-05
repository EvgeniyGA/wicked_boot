#include "fatfs.h"
#include "W25Qxx.h"

char sramPath[4];
FATFS USERFatFS;
FIL file;
SemaphoreHandle_t xDiskMutex = NULL;

uint8_t FATFS_Init(void) {
    static BYTE work[DISK_BLOCK_SIZE] = {0};

    if (xDiskMutex == NULL) {
        xDiskMutex = xSemaphoreCreateMutex();
        if (xDiskMutex == NULL) {
            printf("CRITICAL: Failed to create disk mutex!\n");
            while(1);
        }
    }
#ifdef MSC_USE_SRAM
    if (FATFS_LinkDriver(&SRAMDISK_Driver, sramPath) != 0) {
#elif defined(MSC_USE_FLASH)
    uint32_t ID = W25Q_ReadID();
    printf("ID = 0x%08lX\r\n", ID);
    if (FATFS_LinkDriver(&USER_Driver, sramPath) != 0) {
#endif
        printf("ERROR: Cannot link Disk driver\n");
        return -1;
    }

    FRESULT res = f_mount(&USERFatFS, sramPath, 1);

    if (res == FR_NO_FILESYSTEM) {
        printf("No filesystem found. Formatting disk...\n");
        MKFS_PARM fmt_opt = {FM_FAT | FM_SFD, 0, 0, 0, 0};
        FRESULT fr = f_mkfs(sramPath, &fmt_opt, work, sizeof(work));

        if (fr != FR_OK) {
            printf("ERROR: Formatting failed! Code: %d\n", fr);
            return -1;
        }
        printf("Formatting successful!\n");
        res = f_mount(&USERFatFS, sramPath, 1);
    }

    if (res != FR_OK) {
        printf("ERROR: Cannot mount FS! Code: %d\n", res);
        return -1;
    }

    printf("FatFS mounted successfully!\n");

    FIL file;

    FRESULT fr = f_open(&file, "0:/README.TXT", FA_OPEN_ALWAYS | FA_WRITE);
    if (fr == FR_OK) {
        if (f_size(&file) == 0) {
            char data[] = "Hello from External Flash disk!";
            UINT bw;
            f_write(&file, data, sizeof(data) - 1, &bw);
            f_sync(&file);
            printf("Created README.TXT\n");
        } else {
            printf("README.TXT already exists, preserving its content.\n");
        }
        f_close(&file);
    } else {
        printf("ERROR: Failed to open/create README.TXT: %d\n", fr);
        return -1;
    }
    return 0;

}
