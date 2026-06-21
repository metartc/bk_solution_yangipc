

#ifndef SRC_YANGDEMO_YANGIPC_H_
#define SRC_YANGDEMO_YANGIPC_H_

#include <stdint.h>
#include "YangIpcConfig.h"

int32_t yang_ipc();
int cli_yang_ipc_init(void);
void yang_init_ipcConfig(YangIpcConfig* config);
#endif /* SRC_YANGDEMO_YANGIPC_H_ */
