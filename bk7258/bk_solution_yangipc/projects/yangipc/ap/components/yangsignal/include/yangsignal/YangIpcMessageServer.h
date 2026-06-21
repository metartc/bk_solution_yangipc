//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_
#define INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_
#include <yangsignal/YangIpcConfig.h>
#include <yangsignal/YangIpcMessageDef.h>

typedef struct{
	void* session;
	int32_t  (*start)(void* session,char* subscribeTopic);
	void (*stop)(void* session);
	void (*setFilePath)(void* session,char* filePath);
	int32_t  (*publish)(void* session,char* topic,char* msg,int32_t  msgLen);
	int32_t  (*addCandidate)(void* session,int32_t  uid,char* candidateStr);
	int32_t  (*checkLive)(void* session);
}YangIpcMessageServer;

#ifdef __cplusplus
extern "C"{
#endif

int32_t  yang_create_ipcmessageServer(YangIpcMessageServer *message,YangIpcHandle* handle,YangIpcDataMsg* dataMsg,YangIpcConfig* config);
void yang_destroy_ipcmessageServer(YangIpcMessageServer* message);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_YANGSIGNAL_YANGIPCMESSAGESERVER_H_ */
