#include "config_service.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h> 
#include "fatfs.h"

#define STORAGE_TASK_STACK_SIZE         (configMINIMAL_STACK_SIZE * 1)
#define STORAGE_TASK_PRIORITY           (configMAX_PRIORITIES - 5)

#define STORAGE_QUEUE_LEN               (1)
#define STORAGE_MAX_SAVING_SIZE         (100)

static StaticTask_t config_tx_task_def, config_rx_task_def;
static TaskHandle_t config_tx_task_handle, config_rx_task_handle;
static QueueHandle_t storage_rx_queue_handle, storage_tx_queue_handle;
static StaticQueue_t storage_rx_queue_def, storage_tx_queue_def;
static SemaphoreHandle_t fs_mutex = NULL;

static uint8_t massive_for_saving[STORAGE_MAX_SAVING_SIZE];

typedef enum{
    STORAGE_MSG_SAVE_INT = 0,
    STORAGE_MSG_LOAD_INT,
    STORAGE_MSG_MAX_OPT
}storage_service_msg_e;

typedef struct{
    char* name;
    uint8_t type;
    uint8_t* data;
    uint16_t datalen;
}storage_service_msg_tx_t;

typedef struct{
    uint8_t type;
    uint8_t* data;
    uint16_t datalen;
    SemaphoreHandle_t done;
}storage_service_msg_rx_t;

uint8_t config_save_int(char* name, uint32_t data){
    storage_service_msg_tx_t msg;
    msg.name = name;
    msg.type = STORAGE_MSG_SAVE_INT;
    msg.data = massive_for_saving;
    msg.datalen = sizeof(data);
    memcpy(msg.data, &data, msg.datalen);

    if(xQueueSendToBack(storage_tx_queue_handle, &msg, portMAX_DELAY) != pdPASS){
        //todo error
    }
    return 0;
}

uint8_t config_load_int(char* name, uint32_t* data){
//    *data = 123;
    storage_service_msg_rx_t msg;
    msg.type = STORAGE_MSG_LOAD_INT;
    msg.data = (uint8_t*)data;
    msg.datalen = sizeof(*data);
    msg.done = xSemaphoreCreateBinary();
    if(xQueueSendToBack(storage_rx_queue_handle, &msg, portMAX_DELAY) != pdPASS){
        vSemaphoreDelete(msg.done);
        return -1;
    }
    if(xSemaphoreTake(msg.done, portMAX_DELAY)){
        vSemaphoreDelete(msg.done);
        return 0;
    }
    vSemaphoreDelete(msg.done);
    return -1;
}

void config_set_readonly(uint8_t event){

}

SemaphoreHandle_t storage_get_fs_mutex(void){
    return fs_mutex;
}

void config_service_tx_task(void* param){
    static uint8_t storage_queue_buffer[STORAGE_QUEUE_LEN * sizeof(storage_service_msg_tx_t)];
    storage_service_msg_tx_t msg;
    storage_tx_queue_handle = xQueueCreateStatic(STORAGE_QUEUE_LEN, sizeof(storage_service_msg_tx_t), storage_queue_buffer, &storage_rx_queue_def);
    while(1){
        if(xQueueReceive(storage_tx_queue_handle, &msg, portMAX_DELAY)){
            xSemaphoreTake(fs_mutex, portMAX_DELAY);
            if(msg.type == STORAGE_MSG_SAVE_INT){
                int tmp;
                memcpy(&tmp, msg.data, msg.datalen);
                printf("storage will save %d\n\r", tmp);
                
/*                FRESULT res;
                DIR dir;
                FILINFO fno;
                static FIL fil;
                uint32_t byteswritten;
                uint8_t wdata[10];// = "321";

                FIL file;
                char buf[32];
                UINT bw;
                int len = 0;

                if(f_opendir(&dir, "/") == 0){
                    f_readdir(&dir, &fno);
                    if(f_open(&fil, msg.name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){

                    }
                    else{
                        len = snprintf(buf, sizeof(buf), "%d", msg.val);
                        res = f_write(&fil, wdata, sizeof(wdata), (void*)&byteswritten)
                        if((bytessritten == 0) || (res != FR_OK)){

                        }
                        f_close(&fil);
                    }
                }*/
            }
            else{

            }
            xSemaphoreGive(fs_mutex);
        }
    }
}

void config_service_rx_task(void* param){
    static uint8_t storage_queue_buffer[STORAGE_QUEUE_LEN * sizeof(storage_service_msg_rx_t)];
    storage_service_msg_rx_t msg;
    storage_rx_queue_handle = xQueueCreateStatic(STORAGE_QUEUE_LEN, sizeof(storage_service_msg_rx_t), storage_queue_buffer, &storage_tx_queue_def);
    while(1){
        if(xQueueReceive(storage_rx_queue_handle, &msg, portMAX_DELAY)){
            xSemaphoreTake(fs_mutex, portMAX_DELAY);

            if(msg.type == STORAGE_MSG_LOAD_INT){
                /*
                    FIL file;
                    char buf[32];
                    UINT br;

                    if(f_open(&file, msg.filename, FA_READ) == FR_OK){
                        f_read(&file, buf, sizeof(buf) - 1, &br)
                        f_close(&file);
                        buf[br] = '\0';
                        if(msg.type == STORAGE_MSG_LOAD_INT){
                            msg.out = atoi(buf);
                        }
                    }
                    else{
                        //msg.resutl
                    }
                */
                uint32_t test_val = 777;
                memcpy(msg.data, &test_val, msg.datalen);
            }

            xSemaphoreGive(fs_mutex);
            xSemaphoreGive(msg.done);

        }
    }
}

uint8_t config_service_init(void){
    static StackType_t storage_stack_tx[STORAGE_TASK_STACK_SIZE];
    static StackType_t storage_stack_rx[STORAGE_TASK_STACK_SIZE];
    fs_mutex = xSemaphoreCreateMutex();
    config_tx_task_handle = xTaskCreateStatic(config_service_tx_task, "config_tx", STORAGE_TASK_STACK_SIZE,
                NULL, STORAGE_TASK_PRIORITY, storage_stack_tx, &config_tx_task_def);
    config_rx_task_handle = xTaskCreateStatic(config_service_rx_task, "config_rx", STORAGE_TASK_STACK_SIZE,
                NULL, STORAGE_TASK_PRIORITY, storage_stack_rx, &config_rx_task_def);
    return 0;
}

