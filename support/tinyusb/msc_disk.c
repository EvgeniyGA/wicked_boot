/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board_api.h"
#include "tusb.h"
#include "semphr.h"
//#include "fatfs.h"
#include "sram_diskio.h"
#include "flash_diskio.h"

#if CFG_TUD_MSC

// Use async IO in example or not
#define CFG_EXAMPLE_MSC_ASYNC_IO    1

// Simulate read/write operation delay
#define CFG_EXAMPLE_MSC_IO_DELAY_MS 0

#if CFG_EXAMPLE_MSC_ASYNC_IO
#define IO_STACK_SIZE      configMINIMAL_STACK_SIZE

typedef struct {
  uint8_t lun;
  bool is_read;
  uint32_t lba;
  uint32_t offset;
  void* buffer;
  uint32_t bufsize;
} io_ops_t;

QueueHandle_t io_queue;
#if configSUPPORT_STATIC_ALLOCATION
uint8_t io_queue_buf[sizeof(io_ops_t)];
StaticQueue_t io_queue_static;
StackType_t  io_stack[IO_STACK_SIZE];
StaticTask_t io_taskdef;
#endif

static void io_task(void *params);
#endif

// whether host does safe-eject
static bool ejected = false;

#if CFG_EXAMPLE_MSC_ASYNC_IO
void msc_disk_init(void) {

#if configSUPPORT_STATIC_ALLOCATION
  io_queue = xQueueCreateStatic(1, sizeof(io_ops_t), io_queue_buf, &io_queue_static);
  xTaskCreateStatic(io_task, "io", IO_STACK_SIZE, NULL, 2, io_stack, &io_taskdef);
#else
  io_queue = xQueueCreate(1, sizeof(io_ops_t));
  xTaskCreate(io_task, "io", IO_STACK_SIZE, NULL, 2, NULL);
#endif
}

static void io_task(void *params) {
  (void) params;
  io_ops_t io_ops;
  while (1) {
    if (xQueueReceive(io_queue, &io_ops, portMAX_DELAY)) {
			int32_t nbytes = (int32_t) io_ops.bufsize;
			if (io_ops.is_read) {
#ifdef MSC_USE_SRAM
				SRAMDISK_read(io_ops.lun, io_ops.buffer, io_ops.lba, 1);
#elif defined(MSC_USE_FLASH)
				USER_read(io_ops.lun, io_ops.buffer, io_ops.lba, 1);
#endif
			} else {
#ifndef CFG_EXAMPLE_MSC_READONLY
#ifdef MSC_USE_SRAM
				SRAMDISK_write(io_ops.lun, io_ops.buffer, io_ops.lba, 1);
#elif defined(MSC_USE_FLASH)
				USER_write(io_ops.lun, io_ops.buffer, io_ops.lba, 1);
#endif
#else
				nbytes = -1; // failed to write
#endif
			}
			tusb_time_delay_ms_api(CFG_EXAMPLE_MSC_IO_DELAY_MS);
			tud_msc_async_io_done(nbytes, false);
    }

  }
}

#else
void msc_disk_init() {}
#endif

// Invoked when received SCSI_CMD_INQUIRY, v2 with full inquiry response
// Some inquiry_resp's fields are already filled with default values, application can update them
// Return length of inquiry response, typically sizeof(scsi_inquiry_resp_t) (36 bytes), can be longer if included vendor data.
uint32_t tud_msc_inquiry2_cb(uint8_t lun, scsi_inquiry_resp_t* inquiry_resp, uint32_t bufsize) {
  (void) lun;
  (void) bufsize;
  const char vid[] = "TinyUSB";
  const char pid[] = "Mass Storage";
  const char rev[] = "1.0";

  strncpy((char*) inquiry_resp->vendor_id, vid, 8);
  strncpy((char*) inquiry_resp->product_id, pid, 16);
  strncpy((char*) inquiry_resp->product_rev, rev, 4);

  return sizeof(scsi_inquiry_resp_t); // 36 bytes
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun) {
  (void) lun;

  // RAM disk is ready until ejected
  if (ejected) {
    // Additional Sense 3A-00 is NOT_FOUND
    tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
    return false;
  }

  return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size) {
  (void) lun;
  *block_count = DISK_BLOCK_NUM;
  *block_size  = DISK_BLOCK_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) {
  (void) lun;
  (void) power_condition;

  if (load_eject) {
    if (start) {
      // load disk storage
    } else {
      // unload disk storage
      ejected = true;
    }
  }

  return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  (void) lun;

  // out of ramdisk
  if (lba >= DISK_BLOCK_NUM) {
    return TUD_MSC_RET_ERROR;
  }

  // Check for overflow of offset + bufsize
  if (lba * DISK_BLOCK_SIZE + offset + bufsize > DISK_BLOCK_NUM * DISK_BLOCK_SIZE) {
    return TUD_MSC_RET_ERROR;
  }

  #if CFG_EXAMPLE_MSC_ASYNC_IO
  io_ops_t io_ops = {.is_read = true, .lun = lun, .lba = lba, .offset = offset, .buffer = buffer, .bufsize = bufsize};

  // Send IO operation to IO task
  TU_ASSERT(xQueueSend(io_queue, &io_ops, 0) == pdPASS);

  return TUD_MSC_RET_ASYNC;
  #else
  uint8_t const *addr = msc_disk[lba] + offset;
  memcpy(buffer, addr, bufsize);
  return bufsize;
  #endif
}

bool tud_msc_is_writable_cb (uint8_t lun) {
  (void) lun;

  #ifdef CFG_EXAMPLE_MSC_READONLY
  return false;
  #else
  return true;
  #endif
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  // out of ramdisk
  if (lba >= DISK_BLOCK_NUM) {
    return TUD_MSC_RET_ERROR;
  }

  // Check for overflow of offset + bufsize
  if (lba * DISK_BLOCK_SIZE + offset + bufsize > DISK_BLOCK_NUM * DISK_BLOCK_SIZE) {
    return TUD_MSC_RET_ERROR;
  }

#ifdef CFG_EXAMPLE_MSC_READONLY
  (void) lun;
  (void) buffer;
  return bufsize;
#elif CFG_EXAMPLE_MSC_ASYNC_IO
  io_ops_t io_ops = {.is_read = false, .lun = lun, .lba = lba, .offset = offset, .buffer = buffer, .bufsize = bufsize};

  // Send IO operation to IO task
  TU_ASSERT(xQueueSend(io_queue, &io_ops, 0) == pdPASS);

  return TUD_MSC_RET_ASYNC;
  #else
  uint8_t *addr = msc_disk[lba] + offset;
  memcpy(addr, buffer, bufsize);
  tusb_time_delay_ms_api(CFG_EXAMPLE_MSC_IO_DELAY_MS);

  return bufsize;
#endif
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) {
  (void) lun;
  (void) scsi_cmd;
  (void) buffer;
  (void) bufsize;

  // currently no other commands are supported

  // Set Sense = Invalid Command Operation
  (void) tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

  return -1; // stall/failed command request;
}

#endif
