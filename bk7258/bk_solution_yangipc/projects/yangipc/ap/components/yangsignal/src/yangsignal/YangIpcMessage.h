//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef SRC_YANGSIGNAL_YANGMESSAGEHANDLE_H_
#define SRC_YANGSIGNAL_YANGMESSAGEHANDLE_H_

#include <yangutil/yangtype.h>
#include <yangmqtt/YangMqtt.h>

#include <yangsignal/YangIpcMessageDef.h>
#include <yangutil/yangavinfotype.h>
#include <yangutil/YangCString.h>
#include <yangutil/YangThread.h>
#include <yangutil/YangVector.h>

yang_vector_declare2(YangIpcRequest)

typedef struct{
	void* user;
	void (*handleMessage)(void *session,YangIpcRequest *mss);
}YangIpcMessageCallback;

typedef struct {
	yangbool isStart;
	yangbool isLoop;
	yangbool isMqttStart;
	yangbool isMqttLoop;
	yangbool isConnected;

	uint32_t uidSeq;

	//yang_thread_t threadId;
	yang_thread_t mqttThreadId;
	yang_thread_mutex_t mutex;

	char* topic;
	YangMqttInfo* mqttInfo;
	YangMqtt* mqtt;
	YangIpcRequest request;
	YangIpcMessageCallback callback;
	YangIpcRequestVector2 messages;
} YangIpcMessageSession;

typedef struct{
	YangIpcMessageSession session;

	void (*putMessage)(YangIpcMessageSession* session,YangIpcRequest* request);
	int32_t (*start)(YangIpcMessageSession* session,char* subscribeTopic);
	void (*stop)(YangIpcMessageSession* session);
	//int32_t (*subscribe)(YangIpcMessageSession* session,char* topic);
	int32_t (*publish)(YangIpcMessageSession* session,char* topic,char* msg,int32_t msgLen);

}YangIpcMessage;

#ifdef __cplusplus
extern "C"{
#endif
int32_t yang_ipcmessage_sendmsg(YangIpcMessageSession *session,char* topic,YangIpcRequest* request);
int32_t yang_create_ipcmessage(YangIpcMessage *message,YangIpcMessageCallback* callback,YangMqttInfo* mqttInfo);
void yang_destroy_ipcmessage(YangIpcMessage* message);

#ifdef __cplusplus
}
#endif
#endif /* SRC_YANGSIGNAL_YANGMESSAGEHANDLE_H_ */
