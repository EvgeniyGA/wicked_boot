#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include "version.h"
#include "version_check.h"
#include "app.h"
#include <string.h>

#include "main.h"
#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "bootutil/bootutil_log.h"
#include "boot_hal.h"
#include "Driver_Usart.h"

#include "flash_map_backend/flash_map_backend.h"

#include "shell/shell.h"

#ifdef STM32F407xx
    #include "lcd_driver.h"
#endif

void init(void){
}

void My_Delay(uint32_t Delay){
    uint64_t del = Delay*1000;
    while(del--);
}

static void emergency_cli(void);

static void do_boot(struct boot_rsp *rsp)
{
    struct boot_arm_vector_table *vt;
    uintptr_t flash_base;
    int rc;

    /* The beginning of the image is the ARM vector table, containing
     * the initial stack pointer address and the reset vector
     * consecutively. Manually set the stack pointer and jump into the
     * reset vector
     */
    rc = flash_device_base(rsp->br_flash_dev_id, &flash_base);
    (void)rc;
    assert(rc == 0);

    if (rsp->br_hdr->ih_flags & IMAGE_F_RAM_LOAD) {
       /* The image has been copied to SRAM, find the vector table
        * at the load address instead of image's address in flash
        */
        vt = (struct boot_arm_vector_table *)(rsp->br_hdr->ih_load_addr +
                                         rsp->br_hdr->ih_hdr_size);
    } else {
        /* Using the flash address as not executing in SRAM */
        vt = (struct boot_arm_vector_table *)(flash_base +
                                         rsp->br_image_off +
                                         rsp->br_hdr->ih_hdr_size);
    }

#if defined(MCUBOOT_HAVE_LOGGING)
//    stdio_uninit();
#endif

    /* This function never returns, because it calls the secure application
     * Reset_Handler().
     */
    boot_platform_quit(vt);
}

void setup(void){
    BOOT_LOG_INF("Bootloader Version: %d.%d.%d", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);
	BOOT_LOG_INF("Build: %s %s (git: %s)", FW_BUILD_DATE, FW_BUILD_TIME, FW_GIT_HASH);

#ifdef STM32F407xx
    static LCD_HandleTypeDef hlcd1 = {.delay_ms = My_Delay}; //HAL_Delay;
    lcd_printer_msg_t msg;

    LCD_Init(&hlcd1);

    msg.line = 0;
    msg.offset = 0;
    strcpy(msg.data, "loading");
    LCD_SetCursor(&hlcd1, msg.line, msg.offset);
    LCD_SendString(&hlcd1, msg.data);
#endif
    BOOT_LOG_INF("Wait or enter any symbol to go CLI:\n\r");
    if((int8_t)console_getc() != - 1){
        emergency_cli();
    }
    
    struct boot_rsp rsp;
    fih_int fih_rc = FIH_FAILURE;
    FIH_CALL(boot_go, fih_rc, &rsp);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        BOOT_LOG_ERR("Unable to find bootable image. Falling into CLI:");
        emergency_cli();//FIH_PANIC;
    }

    BOOT_LOG_INF("Bootloader chainload address offset: 0x%x", (int)rsp.br_image_off);
    BOOT_LOG_INF("Jumping to the first image slot");
    do_boot(&rsp);

    BOOT_LOG_ERR("Something came up. Go CLI");
FAIL:
#ifdef STM32F407xx
    strcpy(msg.data, " fail");
    LCD_SendString(&hlcd1, msg.data);
#endif
    emergency_cli();
}

void emergency_cli(void){
    char c = 0;
    sShellImpl shell_impl = { .send_char = console_putc, };
    shell_boot(&shell_impl);
    while (true) {
        c = console_getc();
        if((int8_t)c != - 1)
            shell_receive_char(c);
    }
}