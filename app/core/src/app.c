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

#include "tusb.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
const char *upload_image[2] = {"Hello world from TinyUSB DFU! - Partition 0",
                               "Hello world from TinyUSB DFU! - Partition 1"};

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED     = 1000,
  BLINK_SUSPENDED   = 2500,
};

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
//FAIL:
#ifdef STM32F407xx
    strcpy(msg.data, " fail");
    LCD_SendString(&hlcd1, msg.data);
#endif
    emergency_cli();
}

void emergency_cli(void){
    char c = 0;

    // init device stack on configured roothub port
    tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO};
    tusb_init(BOARD_TUD_RHPORT, &dev_init);
      while (1) {
        tud_task();
    }

    sShellImpl shell_impl = { .send_char = console_putc, };
    shell_boot(&shell_impl);
    while (true) {
        c = console_getc();
        if((int8_t)c != - 1)
            shell_receive_char(c);
    }
}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
  printf("usb mount\r\n");//blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  printf("usb unmount\r\n");//blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  printf("usb suspend\r\n");//blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
  printf("usb resume\r\n");//blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// DFU callbacks
// Note: alt is used as the partition number, in order to support multiple partitions like FLASH, EEPROM, etc.
//--------------------------------------------------------------------+

// Invoked right before tud_dfu_download_cb() (state=DFU_DNBUSY) or tud_dfu_manifest_cb() (state=DFU_MANIFEST)
// Application return timeout in milliseconds (bwPollTimeout) for the next download/manifest operation.
// During this period, USB host won't try to communicate with us.
uint32_t tud_dfu_get_timeout_cb(uint8_t alt, uint8_t state) {
  if (state == DFU_DNBUSY) {
    // For this example
    // - Atl0 Flash is fast : 1   ms
    // - Alt1 EEPROM is slow: 100 ms
    return (alt == 0) ? 1 : 100;
  } else if (state == DFU_MANIFEST) {
    // since we don't buffer entire image and do any flashing in manifest stage
    return 0;
  } else {
    // nothing to do
  }

  return 0;
}

// Invoked when received DFU_DNLOAD (wLength>0) following by DFU_GETSTATUS (state=DFU_DNBUSY) requests
// This callback could be returned before flashing op is complete (async).
// Once finished flashing, application must call tud_dfu_finish_flashing()
void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, const uint8_t *data, uint16_t length) {
    (void)alt;
    (void)block_num;
    static uint32_t last_erased_sector = 0xFFFFFFFF;

    printf("\r\nReceived Alt %u BlockNum %u of length %u\r\n", alt, block_num, length);

    uint32_t current_addr = 0x80000 + ((uint32_t)block_num * length);
    
    uint32_t current_sector = GetSectorFromAddress(0x8000000 + current_addr);

    if ((current_sector != last_erased_sector) && (length == 64)) {
        printf("\r\n[DFU] >>> Erasing Sector %u at 0x%08lX...\r\n", current_sector, current_addr);
        
        if (Driver_FLASH0.EraseSector(current_addr) != ARM_DRIVER_OK) {
            printf("[DFU] >>> Erase FAILED!\r\n");
            tud_dfu_finish_flashing(DFU_STATUS_ERR_ERASE);
            return;
        }
        
        last_erased_sector = current_sector;
        printf("[DFU] >>> Erase OK. Starting programming...\r\n");
    }

    if (Driver_FLASH0.ProgramData(current_addr, data, length) != ARM_DRIVER_OK) {
        printf("[DFU] Program FAILED at 0x%08lX!\r\n", current_addr);
        tud_dfu_finish_flashing(DFU_STATUS_ERR_WRITE);
        return;
    }

    //for (uint16_t i = 0; i < length; i++) {
    //  printf("%c", data[i]);
    //}

    // flashing op for download complete without error
    tud_dfu_finish_flashing(DFU_STATUS_OK);
}

// Invoked when download process is complete, received DFU_DNLOAD (wLength=0) following by DFU_GETSTATUS (state=Manifest)
// Application can do checksum, or actual flashing if buffered entire image previously.
// Once finished flashing, application must call tud_dfu_finish_flashing()
void tud_dfu_manifest_cb(uint8_t alt) {
  (void)alt;
    const int permanent = 0;
  boot_set_pending(permanent);
  NVIC_SystemReset();

  printf("Download completed, enter manifestation\r\n");

  // flashing op for manifest is complete without error
  // Application can perform checksum, should it fail, use appropriate status such as errVERIFY.
  tud_dfu_finish_flashing(DFU_STATUS_OK);
}

// Invoked when received DFU_UPLOAD request
// Application must populate data with up to length bytes and
// Return the number of written bytes
uint16_t tud_dfu_upload_cb(uint8_t alt, uint16_t block_num, uint8_t *data, uint16_t length) {
  (void)block_num;
  (void)length;

  if (block_num != 0u) {
    return 0; // for this example we only support single block upload
  }

  const uint16_t xfer_len = tu_min16((uint16_t)strlen(upload_image[alt]), length);
  memcpy(data, upload_image[alt], xfer_len);

  return xfer_len;
}

// Invoked when the Host has terminated a download or upload transfer
void tud_dfu_abort_cb(uint8_t alt) {
  (void)alt;
  printf("Host aborted transfer\r\n");
}

// Invoked when a DFU_DETACH request is received
void tud_dfu_detach_cb(void) {
  printf("Host detach, we should probably reboot\r\n");
}