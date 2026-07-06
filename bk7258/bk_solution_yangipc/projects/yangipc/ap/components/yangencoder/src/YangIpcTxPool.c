/*
 * YangIpcTxPool.c
 *
 *  Created on: 2026年7月4日
 *      Author: yang
 */

#include <stdint.h>
#include <stdio.h>

#ifndef CONFIG_YANGIPC_TXPOOL
#define CONFIG_YANGIPC_TXPOOL 0
#endif
#ifndef CONFIG_YANGIPC_TXPOOL_SIZE
#define CONFIG_YANGIPC_TXPOOL_SIZE 16
#endif
#ifndef CONFIG_YANGIPC_TXPOOL_PAYLOAD_SIZE
#define CONFIG_YANGIPC_TXPOOL_PAYLOAD_SIZE 2208
#endif

int32_t yang_enable_txPoolSize(){
	return CONFIG_YANGIPC_TXPOOL;
}

uint32_t yang_get_txPoolSize(){
	return CONFIG_YANGIPC_TXPOOL_SIZE;
}

uint32_t yang_get_txPoolPayloadSize(){
	return CONFIG_YANGIPC_TXPOOL_PAYLOAD_SIZE;
}
