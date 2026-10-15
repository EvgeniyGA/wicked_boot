#include "boot_hal.h"
#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    __disable_irq();

    // HAL_DeInit(); // Можно раскомментировать, но иногда это ломает отладку по SWD
    
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICER[2] = 0xFFFFFFFF;

    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
    NVIC->ICPR[2] = 0xFFFFFFFF;

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    uint32_t app_start_addr = vt->reset - 4;

    SCB->VTOR = app_start_addr;
    
    __DSB();
    __ISB();

    __set_MSP(vt->msp);

    // 9. Приводим адрес к типу функции и вызываем её (прыжок в приложение)
    // Использование volatile и явного приведения типов предотвращает оптимизации компилятора
    void (*app_entry)(void) = (void (*)(void))vt->reset;
    
    app_entry();

    // 10. На случай, если приложение вернет управление (чего быть не должно)
    while (1) {
        __NOP();
    }
}
