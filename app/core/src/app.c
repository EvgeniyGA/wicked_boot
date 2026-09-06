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
#include "bootutil/bootutil.h"
#include "bootutil/image.h"

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

#include "stm32f4xx.h"
#include "bootutil/bootutil.h"

static void do_boot(struct boot_rsp *rsp) {
    // 1. Вычисляем РЕАЛЬНЫЙ адрес начала приложения.
    // br_image_off = 0x08040000 (начало слота)
    // ih_hdr_size  = 0x00000200 (512 байт заголовка MCUboot)
    // Итого: 0x08040200. Именно здесь лежит таблица векторов приложения.
    uint32_t app_start_addr = rsp->br_image_off + rsp->br_hdr->ih_hdr_size;
printf("\n\r==before jump==\n\r");
    // 2. Отключаем прерывания
    __disable_irq();

    // 3. Деинициализируем HAL (как в твоем рабочем коде)
    HAL_DeInit();

    // 4. Отключаем все прерывания NVIC
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;

    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    NVIC->ICPR[2] = 0xFFFFFFFF;

    // 5. Деактивируем SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // 6. Устанавливаем VTOR на реальный адрес таблицы векторов приложения
    SCB->VTOR = app_start_addr;

    // 7. Читаем начальный MSP и адрес Reset_Handler из таблицы векторов приложения
    uint32_t app_msp = *(__IO uint32_t*)app_start_addr;
    uint32_t app_reset_handler = *(__IO uint32_t*)(app_start_addr + 4u);

    // 8. Меняем главный указатель стека
    __set_MSP(app_msp);

    // 9. Прыгаем на Reset_Handler приложения
    ((void (*)(void))app_reset_handler)();

    // Сюда мы не должны вернуться никогда
    while (1) {
        __NOP();
    }
}

#include "lcd_driver.h"

void My_Delay(uint32_t Delay){
    uint64_t del = Delay*1000;
    while(del--);
}

void setup(void){
    static LCD_HandleTypeDef hlcd1 = {.delay_ms = My_Delay}; //HAL_Delay;
    lcd_printer_msg_t msg;

    msg.line = 0;
    msg.offset = 0;
    msg.data[0] = 'l';
    msg.data[1] = 'o';
    msg.data[2] = 'a';
    msg.data[3] = 'd';
    msg.data[4] = 'i';
    msg.data[5] = 'n';
    msg.data[6] = 'g';
    msg.data[7] = '.';
    msg.data[8] = '.';
    msg.data[9] = '.';
    msg.data[10] = 0;


    LCD_Init(&hlcd1);
    LCD_SetCursor(&hlcd1, msg.line, msg.offset);
    LCD_SendString(&hlcd1, msg.data);
    
    //lcd_print(LCD_PRINTER_LINE1, LCD_PRINTER_OFFSET_ZERO + 1, "Version: %d", 123);
    My_Delay(1000);


  printf("==Starting Bootloader==");


	struct boot_rsp rsp;
	int rv = boot_go(&rsp);

	if (rv == 0) {
		do_boot(&rsp);
	}

	printf("No bootable image found. Falling into Bootloader CLI:");
	
	/*printf("Firmware version: %s\n", FW_VERSION_STR);
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

	FATFS_Init();*/
	cli_service_init();
	/*config_service_init();
	status_service_init(heartbit_callback);

	usb_device_init(&usb_device_config);
	usb_cdc_init();*/

	vTaskStartScheduler();
}


