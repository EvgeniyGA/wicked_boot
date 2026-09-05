#include "status_service.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

#define BLINKY_STACK_SIZE   configMINIMAL_STACK_SIZE

void led_blinking_task(void* param);

uint8_t status_service_init(status_heartbit_t status_heartbit_callback){
    xTaskCreate(led_blinking_task, "blinky", BLINKY_STACK_SIZE, status_heartbit_callback, 1, NULL);//todo
    return 0;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void* param) {
  static uint8_t led_state = 0;
  while (1) {
    SEGGER_SYSVIEW_PrintfHost("BlikTask started");
    ((status_heartbit_t)param)();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
//    led_state = 1 - led_state; // toggle
//	  printf("blink %04d\n\r", led_state);
//    lcd_print(0, 1, "counter: %d", i++);
  }
}