//
// Copyright (c) 2019-2026 yanggaofeng
//
#include <yangutil/yangtype.h>
#include "YangIpcMessage.h"
#include "YangIpcMessageParser.h"

yang_vector_impl2(YangIpcRequest)

static void yang_ipcmessage_putMessage(YangIpcMessageSession *session,  YangIpcRequest* request) {

	if (!session->isLoop)
		return;

	yang_thread_mutex_lock(&session->mutex);
	session->messages.insert(&session->messages.vec, request);

	yang_thread_mutex_unlock(&session->mutex);

}


 int32_t yang_ipcmessage_sendmsg(YangIpcMessageSession *session,char* topic,YangIpcRequest* request){

	if(session==NULL||!session->isConnected||session->mqtt==NULL) return 1;

	if(request->cid[0]==0){
		char* cid=session->mqtt->getClientId(session->mqtt->session);
		if(cid) yang_memcpy(request->cid,cid,Yang_Cid_Length);
	}

	return yang_ipcmsg_send(session->mqtt,topic,request);
}


static void yang_ipcmessage_mqtt_receive(void* psession,char* topic,char* msg,int32_t msgLen){
	YangIpcRequest *request;
	YangIpcMessageSession *session = (YangIpcMessageSession*) psession;

	if(psession==NULL || msg==NULL)
		return;

	request = (YangIpcRequest*) yang_calloc(sizeof(YangIpcRequest),	1);
	if(yang_ipcmsg_parse(msg,request)!=Yang_Ok){
		yang_free(request);
		return;
	}
	yang_ipcmessage_putMessage(session,request);
}

static void yang_ipcmessage_mqtt_connect_error(void *psession,int32_t errCode){
	YangIpcMessageSession *session = (YangIpcMessageSession*) psession;
	if(session==NULL)
		return;

	session->isConnected=yangfalse;
	yang_error("yang_ipcmessage_mqtt connect error");
}

static int32_t yang_reconnectMqtt(YangIpcMessageSession *session){
	YangMqttCallback callback;
	int32_t waitTime=1;
	uint32_t reconnectCount=0;
	char* username=yang_strlen(session->mqttInfo->mqttUserName)==0?NULL:session->mqttInfo->mqttUserName;
	char* password=yang_strlen(session->mqttInfo->mqttPassword)==0?NULL:session->mqttInfo->mqttPassword;
	char* remoteIp=session->mqttInfo->mqttServerIP;
	callback.session=session;
	callback.mqtt_receive=yang_ipcmessage_mqtt_receive;
	callback.mqtt_connect_error=yang_ipcmessage_mqtt_connect_error;
	if(session->mqtt){
		yang_destroy_mqtt(session->mqtt);
		yang_free(session->mqtt);
	}
	session->mqtt=(YangMqtt*)yang_calloc(sizeof(YangMqtt),1);
	yang_create_mqtt(session->mqtt,&callback,session->mqttInfo->enableMqttTls);

	if(session->mqtt->setReconnectParam)
		session->mqtt->setReconnectParam(session->mqtt->session,session->mqttInfo->maxReconnectTimes,session->mqttInfo->reconnectIntervalTime);

	while (session->isMqttLoop){
		if(reconnectCount){

			yang_usleep(waitTime*1000*1000);
			if(waitTime<=64)
				waitTime*=2;

			yang_error("try %d times reconnect mqtt server!",reconnectCount);
		}

		if(session->mqtt->connect(session->mqtt->session,yangfalse,16*1024,16*1024,remoteIp,
				session->mqttInfo->mqttPort,username,password)==Yang_Ok){
			session->mqtt->subscribe(session->mqtt->session,session->topic);
			session->isConnected=yangtrue;
			return Yang_Ok;
		}
		reconnectCount++;

	}
	if(session->mqtt){
		yang_destroy_mqtt(session->mqtt);
		yang_free(session->mqtt);
	}
	return 1;
}

void* yang_ipcmessage_mqttc_thread(void *obj) {
	int32_t err=Yang_Ok;
	YangIpcMessageSession *session=(YangIpcMessageSession*)obj;
	session->isMqttStart=yangtrue;

	yangbool isReconnect=yangfalse;
	uint32_t loopCount=0;
	while (session->isMqttLoop) {
		if((err=session->mqtt->synMessage(session->mqtt->session))!=Yang_Ok){
			session->isConnected=yangfalse;
			yang_error("connect mqtt server fail! try reconnect!");
			if(!isReconnect){
				session->mqtt->reconnect2(session->mqtt->session);
				isReconnect=yangtrue;
				continue;
			}
			if(yang_reconnectMqtt(session)){
				yang_error("reconnect mqtt server fail!");
				break;
			}else{
				isReconnect=yangfalse;
			}

		}
        if(loopCount++>50){
        	loopCount=0;
        	session->mqtt->ping(session->mqtt->session);
        }
		yang_usleep(100 * 1000);
	}

	if(session->mqtt){
			yang_destroy_mqtt(session->mqtt);
			yang_free(session->mqtt);
	}

	session->isMqttStart=yangfalse;
	yang_trace("\nmqtt listen thread is close!");
	return NULL;
}

static void yang_run_ipcmessage_thread(YangIpcMessageSession *session) {
	session->isStart = yangtrue;
	session->isLoop = yangtrue;
	while (session->isLoop) {
		while(session->messages.vec.vsize > 0) {

			YangIpcRequest *request = session->messages.vec.payload[0];

			if(session->callback.handleMessage)
				session->callback.handleMessage(session,request);

			yang_free(request->url);
			yang_free(request->data);

			yang_free(session->messages.vec.payload[0]);
			yang_thread_mutex_lock(&session->mutex);
			session->messages.remove(&session->messages.vec, 0);
			yang_thread_mutex_unlock(&session->mutex);

		}
		yang_usleep(100*1000);
	}

	session->isStart = yangfalse;
}


static int32_t yang_ipcmessage_start(YangIpcMessageSession *session,char* subscribeTopic) {
	int32_t err=Yang_Ok;
	if(session==NULL)
		return 1;

	session->isMqttLoop=yangtrue;

	if(session->topic==NULL){
		session->topic=(char*)yang_calloc(1,yang_strlen(subscribeTopic)+1);
		yang_strcpy(session->topic,subscribeTopic);
	}

	err=yang_reconnectMqtt(session);

	if(err!=Yang_Ok){
		return yang_error_wrap(err,"mqtt connect fail!");
	}


	if ((err=yang_thread_create(&session->mqttThreadId, 0, yang_ipcmessage_mqttc_thread,
			session)) != Yang_Ok) {
		yang_error("YangThread::start could not start thread");
		err=ERROR_THREAD;
	}

	yang_run_ipcmessage_thread(session);
#if 0
	if ((err=yang_thread_create(&session->threadId, 0, yang_run_ipcmessage_thread,
			session)) != Yang_Ok) {
		yang_error("YangThread::start could not start thread");
		err=ERROR_THREAD;
	}
#endif
	return err;

}

static void yang_ipcmessage_stop(YangIpcMessageSession *session) {
	if(session==NULL)
		return;

	session->isLoop = yangfalse;
	session->isMqttLoop=yangfalse;

}


static int32_t yang_ipcmessage_publish(YangIpcMessageSession* session,char* topic,char* msg,int32_t msgLen){
	if(session==NULL || topic==NULL)
		return ERROR_MQTT_SERVER;

	if(session->isConnected&&session->mqtt&&session->mqtt->publish) {

		return session->mqtt->publish(session->mqtt->session,topic,msg,msgLen);
	}
	return ERROR_MQTT_SERVER;
}

int32_t yang_create_ipcmessage(YangIpcMessage *message,YangIpcMessageCallback* msgcallback,YangMqttInfo* mqttInfo) {
	YangIpcMessageSession *session = &message->session;
	if(message==NULL || mqttInfo==NULL )
		return ERROR_MQTT_SERVER;

	session->isLoop = yangfalse;
	session->isStart = yangfalse;
	session->isMqttLoop=yangfalse;
	session->isMqttStart=yangfalse;
	session->isConnected=yangfalse;

	session->topic=NULL;
	session->mqttInfo=mqttInfo;
	session->uidSeq=10;

	session->callback.user=msgcallback->user;
	session->callback.handleMessage=msgcallback->handleMessage;

	yang_mutex_init(&session->mutex);

	yang_create_YangIpcRequestVector2(&session->messages);

	session->mqtt=NULL;

	message->start = yang_ipcmessage_start;
	message->stop = yang_ipcmessage_stop;
	message->putMessage = yang_ipcmessage_putMessage;

	message->publish = yang_ipcmessage_publish;

	return Yang_Ok;
}

void yang_destroy_ipcmessage(YangIpcMessage *message) {
	YangIpcMessageSession *session = &message->session;
	if(message==NULL) return;

	yang_ipcmessage_stop(session);

	if (session->isStart||session->isMqttStart) {

		while (session->isStart||session->isMqttStart) {
			yang_usleep(1000);
		}
	}

	if(session->mqtt){
		yang_destroy_mqtt(session->mqtt);
		yang_free(session->mqtt);
	}


	while (session->messages.vec.vsize > 0) {
		YangIpcRequest *request = session->messages.vec.payload[0];
		session->callback.handleMessage(session,request);
		yang_free(session->messages.vec.payload[0]);

		session->messages.remove(&session->messages.vec, 0);

	}

	yang_thread_mutex_destroy(&session->mutex);

	yang_destroy_YangIpcRequestVector2(&session->messages);
	yang_free(session->topic);

}
