//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGAPP_IPCCONFIG_H_
#define SRC_YANGAPP_IPCCONFIG_H_
#include <stdint.h>
#include "YangIpcConfig.h"
typedef struct{
	void* session;
	int32_t (*stop)(void* session);
	int32_t (*start)(void* session);
}YangIpcAgent;

int32_t yang_create_ipcAgent(YangIpcAgent* ipc,YangIpcConfig* config);
void yang_destroy_ipcAgent(YangIpcAgent* ipc);
#endif /* SRC_YANGAPP_IPCCONFIG_H_ */
