#include "YangIpcAgent.h"
#include "yang_ipc.h"

#include "driver/lcd_types.h"
#include "frame/frame_que_v2.h"
#include "frame_buffer.h"
#include "yangipc_comm.h"
#include "yangipc_devices.h"
#include "yangipc_audio_device.h"
#include <stdio.h>
#include <string.h>
#include "cli.h"

#include "lwip/apps/sntp.h"
#include <time.h>
#include <sys/time.h>
#include <driver/aon_rtc.h>
#define TAG "metaRTC"
#define Yang_Topic_Server "metaIpc/server"
#define LOGI(...) BK_LOGW(TAG, ##__VA_ARGS__)
#define LOGW(...) BK_LOGW(TAG, ##__VA_ARGS__)
#define LOGE(...) BK_LOGE(TAG, ##__VA_ARGS__)
#define LOGD(...) BK_LOGD(TAG, ##__VA_ARGS__)
#define LOGV(...) BK_LOGV(TAG, ##__VA_ARGS__)

beken_thread_t s_yang_thd = NULL;

#define Yang_Audio 1
#define Yang_Video 1 
typedef struct{
	int32_t width;
	int32_t height;
	YangIpcConfig config;
}YangBkSession;


static void yang_task(beken_thread_arg_t data)
{
	YangIpcConfig* config=psram_zalloc(sizeof(YangIpcConfig));
	YangIpcAgent* yang_ipc=(YangIpcAgent*)psram_zalloc(sizeof(YangIpcAgent));
	yang_init_ipcConfig(config);
	
#if Yang_Video	
	
	//open camera
	//uvc
	//camera_parameters_t camera_parameters = {UVC_DEVICE_ID, config->width, config->height, 1, 0, 90};
	//dvp
	camera_parameters_t camera_parameters = {0, config->width, config->height, 1, 0, 0};
	if(yangipc_camera_turn_on(&camera_parameters)){
		return;
	}
#endif

#if Yang_Audio
	//open audio
	//uac mic
	//audio_parameters_t audio_parameters = {config->audioDirection==1?1:0, 1, CODEC_FORMAT_G711A, CODEC_FORMAT_G711A, DB_SAMPLE_RARE_8K, DB_SAMPLE_RARE_8K, 0};
	//board mic
	audio_parameters_t audio_parameters = {config->audioDirection==1?1:0, 0, CODEC_FORMAT_G711A, CODEC_FORMAT_G711A, DB_SAMPLE_RARE_8K, DB_SAMPLE_RARE_8K, 0};
	
	yangipc_audio_turn_on(&audio_parameters);
#endif
#if Yang_Video
	//open lcd
	// display_parameters_t display_parameters = {LCD_DEVICE_ST7701SN, 90, 0};
	// yangipc_display_turn_on(&display_parameters);
	frame_queue_v2_register_consumer(IMAGE_H264, CONSUMER_TRANSMISSION);
#endif	
	
	if(yang_create_ipcAgent(yang_ipc,config)){
		BK_LOGW("metaRTC", "create ipcHd fail!\n");
		goto cleanup;
	}

	if(yang_ipc->start(yang_ipc->session)){
		BK_LOGW("metaRTC", "start ipcHd fail!\n");
		goto cleanup;

	}
	
	if(yang_ipc->stop)
		yang_ipc->stop(yang_ipc->session);

	cleanup:
	if(yang_ipc){
		yang_destroy_ipcAgent(yang_ipc);
		psram_free(yang_ipc);
	}
	psram_free(config);
#if Yang_Audio
		// yangipc_display_turn_off();
	yangipc_audio_turn_off();
#endif
#if Yang_Video
	yangipc_camera_turn_off();
#endif
	rtos_delete_thread(NULL);
}

int32_t yang_ipc()
{
	int ret;

	ret = rtos_create_thread(&s_yang_thd,
								4,
								"yang_ipc",
								(beken_thread_function_t)yang_task,
								1024 * 16,NULL);
								//(beken_thread_arg_t)NULL);
	if (ret != kNoErr)
	{
		LOGE("Error: failed to create webrtc server: %d\n", ret);
	}
	return ret;
}

#define CMDS_COUNT  (sizeof(s_yang_ipc_commands) / sizeof(struct cli_command))
void cli_yang_ipc_cmd(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
   yang_ipc();
}

static const struct cli_command s_yang_ipc_commands[] =
{
    {"yang_ipc", "yang_ipc", cli_yang_ipc_cmd},
};

int cli_yang_ipc_init(void)
{
    return cli_register_commands(s_yang_ipc_commands, CMDS_COUNT);
}
