#include "cli_service.h"
#include "cli_commands.h"
#include "cli_commands_fs.h"
#include "cli_commands_app.h"

void cli_service_init(void){
    CLI_install_commands();
	CLI_install_commands_fs();
    CLI_install_commands_app();
}