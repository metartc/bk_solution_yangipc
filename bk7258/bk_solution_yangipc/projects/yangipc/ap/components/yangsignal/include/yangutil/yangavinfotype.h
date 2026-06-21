//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef YANGUTIL_YANGAVINFOTYPE_H_
#define YANGUTIL_YANGAVINFOTYPE_H_

#include <yangutil/yangtype.h>

typedef enum{
	YangIPCServerHttp,
	YangIPCServerMqtt
}YangIpcServerType;

typedef struct{
	yangbool enableMqttTls;
	int32_t  mqttPort;
	int32_t  maxReconnectTimes;
	int32_t  reconnectIntervalTime;
	char mqttServerIP[32];
	char mqttUserName[32];
	char mqttPassword[64];
}YangMqttInfo;

typedef struct{
	yangbool enableRecord;
	yangbool enableFilePlay;
	uint32_t fileTimeLength;
	uint32_t maxFilePlayCount;

	char path[256];

}YangFileInfo;

typedef struct YangSysInfo {

	yangbool enableHttps;
	yangbool enableLogFile;
	int32_t  mediaServer;

	YangIpFamilyType familyType;
	YangIpcServerType ipcServerType;

	int32_t userId;

	int32_t  rtmpPort;
	int32_t  httpPort;
	int32_t  transType;
	int32_t  logLevel;

}YangSysInfo;

#endif /* YANGUTIL_YANGTYPE_H_ */
