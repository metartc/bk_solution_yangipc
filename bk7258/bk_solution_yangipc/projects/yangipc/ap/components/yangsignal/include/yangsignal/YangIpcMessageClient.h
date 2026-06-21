//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGSIGNAL_YANGIPCMESSAGECLIENT_H_
#define INCLUDE_YANGSIGNAL_YANGIPCMESSAGECLIENT_H_
#include <yangipc/YangIpcConfig.h>
#include <yangsignal/YangIpcMessageDef.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/yangtype.h>

typedef struct{
	void* session;

	void (*putMessage)(void* session,YangIpcRequest* request);
	int32_t  (*start)(void* session,char* remoteIp,int32_t  remotePort,char* username,char* password);
	void (*stop)(void* session);
	int32_t  (*subscribe)(void* session,char* topic);
	int32_t  (*publish)(void* session,char* topic,char* msg,int32_t  msgLen);
	int32_t  (*sendData)(void* session,char* msg,int32_t  msgLen);
	int32_t  (*addPeerRequest)(void* psession,char* url,char* sdp);
	int32_t  (*loginRequest)(void* psession);
	int32_t  (*addIceCandidate)(void* psession,int32_t ,char* sdp);
	int32_t  (*sendClose)(void* psession,int32_t  uid);

	void (*setIpcParam)(void* psession,YangIpcConnectType connectType,yangbool isHttps);
	YangIpcConnectType (*getConnectType)(void* psession);
	char* (*getCid)(void* psession);
}YangIpcMessageClient;

#ifdef __cplusplus
extern "C"{
#endif

int32_t  yang_create_ipcmessageClient(YangIpcMessageClient *message,YangIpcPlayCallback* playI,YangIpcDataMsg *dataMsg,YangAVInfo* avinfo,yangbool enableTls);
void yang_destroy_ipcmessageClient(YangIpcMessageClient* message);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_YANGSIGNAL_YANGIPCMESSAGECLIENT_H_ */
