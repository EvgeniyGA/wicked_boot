#include "shell/shell.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "bootutil/bootutil.h"

#include "stm32f4xx.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

bool kv_store_write(const char *key, const void *val, uint32_t len) {
  // Stub
  return true;
}

int cli_cmd_kv_write(int argc, char *argv[]) {
  // We expect 3 arguments:
  // 1. Command name
  // 2. Key
  // 3. Value
  if (argc != 3) {
    shell_put_line("> FAIL,1");
  }

  const char *key = argv[1];
  const char *value = argv[2];

  bool result = kv_store_write(key, value, strlen(value));
  if (!result) {
    shell_put_line("> FAIL,2");
  }
  shell_put_line("> OK");
  return 0;
}

int cli_cmd_hello(int argc, char *argv[]) {
  shell_put_line("Hello World!");
  return 0;
}

static void prv_reboot(void) {
  NVIC_SystemReset();
}

static int prv_reboot_cli(int argc, char *argv[]) {
  prv_reboot();
  return 0;
}

static int prv_swap_images(int argc, char *argv[]) {
  printf("Triggering Image Swap");

  const int permanent = 0;
  boot_set_pending(permanent);
  prv_reboot();
  return 0;
}

static const sShellCommand s_shell_commands[] = {
  {"swap_images", prv_swap_images, "Swap images"},
  {"reboot", prv_reboot_cli, "Reboot System"},
//  {"kv_write", cli_cmd_kv_write, "Write a Key/Value pair"},
//  {"hello", cli_cmd_hello, "Say hello"},
  {"help", shell_help_handler, "Lists all commands"},
};

const sShellCommand *const g_shell_commands = s_shell_commands;
const size_t g_num_shell_commands = ARRAY_SIZE(s_shell_commands);
