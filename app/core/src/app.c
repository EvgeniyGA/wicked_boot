#include <stdio.h>
#include <ctype.h>
#include <FreeRTOS.h>
#include "task.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#include "tusb.h"
#include "fatfs.h"
#include "version.h"
#include "version_check.h"
#include "app.h"

#include "main.h"
#include "queue.h"
#include "lcd_printer.h"
#include "cli_service.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "gpio_driver.h"
#include "config_service.h"
#include "status_service.h"

	#include "usb_service.h"


usb_device_config_t usb_device_config;

void init(void){
#ifndef FOR_QEMU
	SEGGER_RTT_ConfigUpBuffer( 0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM );
#endif
	SEGGER_SYSVIEW_Conf();
  	SEGGER_SYSVIEW_Start();
  	while(SEGGER_SYSVIEW_IsStarted()==0);
  	SEGGER_RTT_WriteString( 0, "SEGGER Real-Time-Terminal Started\n" );
}


void usb_device_mounted_callback(void){
	printf("USB device mounted\n\r");
}

void usb_device_unmounted_callback(void){
	printf("USB device unmounted\n\r");
}

void heartbit_callback(void){
//	printf("status callback\n\r");
}

void setup(void){
	printf("Firmware version: %s\n", FW_VERSION_STR);
	printf("Build: %s %s (git: %s)\n", FW_BUILD_DATE, FW_BUILD_TIME, FW_GIT_HASH);
	printf("Version: %d.%d.%d\n", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);

	if (is_hash_invalid(FW_GIT_HASH)) {
		printf("ERROR: Invalid firmware hash detected: %s\r\n", FW_GIT_HASH ? FW_GIT_HASH : "NULL");
	} else {
		printf("FW Hash: %s\r\n", FW_GIT_HASH);
	}

  	lcd_printer_init();
  	lcd_print(LCD_PRINTER_LINE1, LCD_PRINTER_OFFSET_ZERO + 1, "Version: %s", FW_VERSION_STR);

	usb_device_config.mounted = usb_device_mounted_callback;
	usb_device_config.unmounted = usb_device_unmounted_callback;

	FATFS_Init();
	cli_service_init();
	config_service_init();
	status_service_init(heartbit_callback);

	usb_device_init(&usb_device_config);
	usb_cdc_init();

	vTaskStartScheduler();
}


