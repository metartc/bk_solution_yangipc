//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGSIGNAL_YANGIPCCONFIG_H_
#define INCLUDE_YANGSIGNAL_YANGIPCCONFIG_H_

#include <yangsignal/YangIpcMessageDef.h>
//audioDirection 0:sendonly 1:sendrecv
typedef struct{
	uint16_t icePort;
	uint16_t mqttPort;

	int32_t width;
	int32_t height;
	int32_t fps;

	int32_t audioDirection;

	uint8_t deviceName[64];

	uint8_t iceServerIP[64];
	uint8_t mqttServerIP[64];
	char iceUserName[32];
	char icePassword[64];
	char mqttUserName[32];
	char mqttPassword[64];;
}YangIpcConfig;

typedef struct{
		void* user;
		void (*receiveMsg)(void* user,int32_t  uid,uint8_t* data,int32_t  len);
}YangIpcDataMsg;


#endif /* INCLUDE_YANGSIGNAL_YANGIPCCONFIG_H_ */
