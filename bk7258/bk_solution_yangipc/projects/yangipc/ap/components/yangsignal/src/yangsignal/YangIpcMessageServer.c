//
// Copyright (c) 2019-2026 yanggaofeng
//


#include <yangutil/yangtype.h>
#include <yangsignal/YangIpcMessageDef.h>
#include <yangsignal/YangIpcMessageServer.h>

#include <yangutil/YangIni.h>
#include <yangutil/YangTime.h>

#include "YangIpcMessage.h"

#define Yang_LiveTime_Second 30

typedef struct{
	yangbool isConnected;
	int32_t uid;
	uint64_t ts;
	char cid[Yang_Cid_Length];
}YangConnCid;

typedef struct {
	yang_thread_mutex_t msgLock;
	YangConnCid connCid;
	YangMqttInfo mqttInfo;
	YangIpcDataMsg dataMsg;
	YangIpcHandle handle;
	YangIpcMessage message;
} YangIpcMessageServerSession;

static int32_t yang_ipcmessage_answer(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	char *answer = NULL;
	char *sdpstr = NULL;
	char *filename=NULL;

	YangIpcMessageServerSession* serverSession=NULL;

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;

	answer = (char*) yang_calloc(1024 * 8, 1);
	sdpstr = mss->data;

	serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.addPeer) {

		serverSession->handle.addPeer(sdpstr, yang_strlen(sdpstr) + 1, answer, "",
						mss->uid, serverSession->handle.user);

	} else {
		yang_error("mqtt server publish error,session->receive is null");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}
	yang_memset(&session->request,0,sizeof(session->request));
	session->request.connectType=YangM_ConnectType_P2p;
	session->request.requestType=YangIpcRequestAnswer;
	session->request.uid=mss->uid;
	session->request.data=answer;
	yang_memcpy(session->request.cid,mss->cid,sizeof(session->request.cid));

    if (yang_ipcmessage_sendmsg(session,session->request.cid[0]==0?Yang_Topic_Client:session->request.cid, &session->request) != Yang_Ok) {
		yang_error("mqtt server publish error");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}

	if (serverSession&&serverSession->handle.setLocalDescription) {
		serverSession->handle.setLocalDescription( answer, mss->uid, serverSession->handle.user);
	}

	cleanup:
	yang_free(filename);
	yang_free(answer);
	return err;
}

static int32_t yang_ipcmessage_connect(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	char *sdp = NULL;

	YangIpcMessageServerSession* serverSession=NULL;

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;


	serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.addPeer2) {

		serverSession->handle.addPeer2(&sdp, "",
						mss->uid, serverSession->handle.user);

		if(sdp==NULL) goto cleanup;

	} else {
		yang_error("mqtt server publish error,session->receive is null");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}
	yang_memset(&session->request,0,sizeof(session->request));
	session->request.connectType=YangM_ConnectType_P2p;
	session->request.requestType=YangIpcRequestConnectSdp;
	session->request.uid=mss->uid;
	session->request.data=sdp;
	yang_memcpy(session->request.cid,mss->cid,sizeof(session->request.cid));

    if (yang_ipcmessage_sendmsg(session,session->request.cid[0]==0?Yang_Topic_Client:session->request.cid, &session->request) != Yang_Ok) {
		yang_error("mqtt server publish error");
		err = ERROR_MQTT_SERVER;
		goto cleanup;
	}

	cleanup:
	yang_free(sdp);
	return err;
}

static int32_t yang_ipcmessage_response(YangIpcMessageSession *session,
		YangIpcRequest *mss) {

	YangIpcMessageServerSession* serverSession=NULL;

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;

	serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.setRemoteDescription) {
		serverSession->handle.setRemoteDescription( mss->data, mss->uid, serverSession->handle.user);
	}

	return Yang_Ok;
}

static int32_t yang_ipcmessage_candidate(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession;

	if (session == NULL || mss == NULL)
		return ERROR_MQTT_SERVER;

	serverSession = (YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.addCandidate)
		return serverSession->handle.addCandidate(mss->uid,mss->data, serverSession->handle.user);

	return Yang_Ok;
}

static int32_t yang_ipcmessage_handleMessage_ping(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession->connCid.isConnected)
		serverSession->connCid.ts=yang_get_system_time();

	return Yang_Ok;
}

static int32_t yang_ipcmessage_handleMessage_verify(
		YangIpcMessageSession *session, YangIpcRequest *mss) {

	int32_t err=Yang_Ok;
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;
	if(!serverSession->connCid.isConnected)
		return 1;

	if (yang_memcmp(serverSession->connCid.cid,mss->cid,Yang_Cid_Length-1)!=0) {
		yang_memset(&session->request,0,sizeof(session->request));
		session->request.connectType = mss->connectType;
		session->request.requestType = YangIpcRequestNotLogin;
		session->request.uid=mss->uid;
		yang_memcpy(session->request.cid,mss->cid,sizeof(session->request.cid));

		err = yang_ipcmessage_sendmsg(session, session->request.cid[0]==0?Yang_Topic_Client:session->request.cid,&session->request);
		if (err != Yang_Ok)
			return yang_error_wrap(err, "mqtt server publish error");
		return 1;
	}


	return Yang_Ok;
}

static void yang_ipcmessage_handleMessage_p2p(YangIpcMessageSession *session,
		YangIpcRequest *mss) {
	switch (mss->requestType) {
	case YangIpcRequestConnect: {
		yang_ipcmessage_connect(session,mss);
		break;
	}
	case YangIpcRequestConnectSdp: {
		yang_ipcmessage_answer(session, mss);
		break;
	}
	case YangIpcRequestAnswer: {
		yang_ipcmessage_response(session,mss);
		break;
	}
	case YangIpcRequestCandidate: {
		yang_ipcmessage_candidate(session,mss);
		break;
	}
	case YangIpcRequestClose: {
		break;
	}
	case YangIpcRequestPing: {
		yang_ipcmessage_handleMessage_ping(session,mss);
		break;
	}
	default:
		break;
	}
}


static int32_t yang_ipcmessage_handleMessage_close(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->handle.close) {
		serverSession->handle.close(mss->uid, serverSession->handle.user);
	}
	return Yang_Ok;
}



static int32_t yang_ipcmessage_handleMessage_login(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	int32_t err = Yang_Ok;

	YangIpcMessageServerSession *serverSession =
			(YangIpcMessageServerSession*) session->callback.user;

	YangConnCid *connCid=&serverSession->connCid;

	yang_memset(&session->request,0,sizeof(session->request));
	session->request.connectType = mss->connectType;
	session->request.requestType = YangIpcRequestLoginResponse;
	session->request.uid = session->uidSeq++;

	yang_memcpy(session->request.cid, mss->cid, sizeof(session->request.cid));

	if(serverSession->handle.close)
			serverSession->handle.close(connCid->uid,serverSession->handle.user);

	yang_thread_mutex_lock(&serverSession->msgLock);
	yang_memcpy(connCid->cid, session->request.cid, sizeof(session->request.cid));
	connCid->ts = yang_get_system_time();
	connCid->uid = session->request.uid;
	connCid->isConnected=yangtrue;
	yang_thread_mutex_unlock(&serverSession->msgLock);

	if ((err = yang_ipcmessage_sendmsg(session,
			session->request.cid[0] == 0 ? Yang_Topic_Client : session->request.cid, &session->request))
			!= Yang_Ok)
		return yang_error_wrap(err, "mqtt server publish error");

	return err;
}

static int32_t yang_ipcmessage_handleMessage_data(
		YangIpcMessageSession *session, YangIpcRequest *mss) {
	int32_t err = Yang_Ok;
	YangIpcMessageServerSession* serverSession=(YangIpcMessageServerSession*)session->callback.user;

	if (serverSession&&serverSession->dataMsg.receiveMsg && mss->data) {
		serverSession->dataMsg.receiveMsg(serverSession->dataMsg.user, mss->uid,
				(uint8_t*) mss->data, yang_strlen(mss->data) + 1);
	}

	return err;
}

static int32_t yang_ipcmessage_handleMessage_user(
		YangIpcMessageSession *session, YangIpcRequest *request) {

	request->connectType=YangM_ConnectType_P2p;
	return yang_ipcmessage_sendmsg(session,
			request->cid[0] == 0 ? Yang_Topic_Client : request->cid, request);
}


static void yang_ipcmessage_handleMessage(void* psession,
		YangIpcRequest *mss) {
	YangIpcMessageSession *session=(YangIpcMessageSession *)psession;
	if(session==NULL)
		return;

	if (mss->requestType == YangIpcRequestLogin) {
		yang_ipcmessage_handleMessage_login(session, mss);
		return;
	}

	if(yang_ipcmessage_handleMessage_verify(session,mss)!=Yang_Ok){
		return;
	}

	if(mss->connectType==YangM_ConnectType_User){
		yang_ipcmessage_handleMessage_user(session,mss);
		return;
	}

	if (mss->requestType == YangIpcRequestClose) {
		yang_ipcmessage_handleMessage_close(session, mss);
		return;
	}

	switch (mss->connectType) {
	case YangM_ConnectType_P2p: {
		yang_ipcmessage_handleMessage_p2p(session, mss);
		break;
	}
	case YangM_ConnectType_Datachannel: {
		yang_ipcmessage_handleMessage_data(session, mss);
		break;
	}
	default:
		break;

	}
}

static int32_t yang_ipcmsg_start(void *psession, char* subscribeTopic) {
	YangIpcMessageServerSession *session =	(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return 1;

	return session->message.start(&session->message.session,subscribeTopic);
}

static void yang_ipcmsg_stop(void *psession) {
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return;

	return session->message.stop(&session->message.session);
}

static int32_t yang_ipcmsg_publish(void *psession, char *topic, char *msg,
		int32_t msgLen) {
	YangIpcMessageServerSession *session =
			(YangIpcMessageServerSession*) psession;
	if (psession == NULL)
		return ERROR_MQTT_SERVER;

	return session->message.publish(&session->message.session, topic, msg,
			msgLen);
}

static int32_t yang_ipcmsg_addCandidate(void* psession,int32_t uid,char* candidateStr){
	int32_t len;
	YangIpcRequest* request;
	YangIpcMessageServerSession *session = (YangIpcMessageServerSession*) psession;
	if (session == NULL || !session->connCid.isConnected)
		return ERROR_MQTT_SERVER;

	request=(YangIpcRequest*)yang_calloc(sizeof(YangIpcRequest),1);

	request->connectType = YangM_ConnectType_User;
	request->requestType = YangIpcRequestCandidate;
	request->uid = uid;

	yang_memcpy(request->cid,session->connCid.cid,Yang_Cid_Length);
	len=yang_strlen(candidateStr);
	request->data=(char*)yang_calloc(len+1,1);
	yang_memcpy(request->data,candidateStr,len);
	session->message.putMessage(&session->message.session,request);

	return Yang_Ok;
}

static int32_t yang_checkLive(void *psession){
	int32_t ts=0;
	YangIpcMessageServerSession *session =(YangIpcMessageServerSession*) psession;

	if (session == NULL)
		return ERROR_MQTT_SERVER;

	if(!session->connCid.isConnected) return Yang_Ok;

	ts=((yang_get_system_time()-session->connCid.ts))/(1000*1000);

	if(ts>Yang_LiveTime_Second){
		if (session&&session->handle.close) {
			session->handle.close(session->connCid.uid, session->handle.user);
		}
		session->connCid.isConnected=yangfalse;

	}

	return Yang_Ok;
}


int32_t yang_create_ipcmessageServer(YangIpcMessageServer *message,
		YangIpcHandle *handle, YangIpcDataMsg *dataMsg,YangIpcConfig* config) {
	YangIpcMessageServerSession *session;

	if (message == NULL || handle == NULL)
		return ERROR_MQTT_SERVER;

	session = (YangIpcMessageServerSession*) yang_calloc(sizeof(YangIpcMessageServerSession), 1);
	message->session = session;

	session->handle.user = handle->user;
	session->handle.addPeer = handle->addPeer;
	session->handle.addPeer2 = handle->addPeer2;
	session->handle.addCandidate = handle->addCandidate;
	session->handle.setLocalDescription = handle->setLocalDescription;
	session->handle.setRemoteDescription = handle->setRemoteDescription;
	session->handle.close = handle->close;

	yang_mutex_init(&session->msgLock);

	if (dataMsg) {
		session->dataMsg.user = dataMsg->user;
		session->dataMsg.receiveMsg = dataMsg->receiveMsg;
	}

	YangIpcMessageCallback callback;
	callback.user = session;
	callback.handleMessage = yang_ipcmessage_handleMessage;

	session->mqttInfo.enableMqttTls=yangfalse;
	session->mqttInfo.mqttPort=config->mqttPort;
	session->mqttInfo.maxReconnectTimes=1000;
	session->mqttInfo.reconnectIntervalTime=1000;
	yang_strcpy(session->mqttInfo.mqttServerIP,config->mqttServerIP);
	yang_strcpy(session->mqttInfo.mqttUserName,config->mqttUserName);
	yang_strcpy(session->mqttInfo.mqttPassword,config->mqttPassword);
	yang_trace("mqtt subscribe topic=%s\n",config->deviceName);
	yang_trace("mqtt serverIP=%s,port=%d,username=%s,password=%s\n",session->mqttInfo.mqttServerIP,
			session->mqttInfo.mqttPort,session->mqttInfo.mqttUserName,session->mqttInfo.mqttPassword);

	yang_create_ipcmessage(&session->message, &callback, &session->mqttInfo);

	message->start = yang_ipcmsg_start;
	message->stop = yang_ipcmsg_stop;
	message->publish = yang_ipcmsg_publish;
	message->addCandidate = yang_ipcmsg_addCandidate;
	message->checkLive = yang_checkLive;
	return Yang_Ok;
}

void yang_destroy_ipcmessageServer(YangIpcMessageServer *message) {
	YangIpcMessageServerSession *session =
				(YangIpcMessageServerSession*) message->session;
	if (message == NULL || message->session == NULL)
		return;

	yang_destroy_ipcmessage(&session->message);

	yang_thread_mutex_destroy(&session->msgLock);
	yang_free(message->session);
}

