#include "bk_private/bk_init.h"
#include <components/system.h>
#include <os/os.h>
#include <media_service.h>
#include "bk_smart_config.h"
#include <yangipc_comm.h>
#include "yangipc_network.h"
#include "yang_ipc.h"

#define Yang_Enable_Cmd 1

void yang_init_ipcConfig(YangIpcConfig* config){
    config->audioDirection=1;
    
    config->width=640;
    config->height=480;
    config->fps=30;

    strcpy(config->deviceName, "test1001");

    config->icePort=3478;
    strcpy(config->iceServerIP, "192.168.0.104");
    strcpy(config->iceUserName, "metartc");
    strcpy(config->icePassword, "metartc");

    config->mqttPort=1883;
    strcpy(config->mqttServerIP, "192.168.0.104");
    strcpy(config->mqttUserName, "");
    strcpy(config->mqttPassword, "");
}

int main(void)
{
    bk_init();
    media_service_init();

#if (defined(CONFIG_INTEGRATION_YANGIPC))
    bk_smart_config_init();
    yangipc_core_init();
#endif
	
    yangipc_wifi_sta_connect("ssid", "password");
    
#if Yang_Enable_Cmd
    cli_yang_ipc_init();
#else
    yang_ipc();
#endif
    return 0;
}
