//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGSIGNAL_YANGIPCMESSAGEDEF_H_
#define INCLUDE_YANGSIGNAL_YANGIPCMESSAGEDEF_H_
#include <stdint.h>
#define Yang_Topic_Client "metaIpc/client"
#define Yang_Topic_Server "metaIpc/server"
#define Yang_Cid_Length 13

typedef enum{
	YangM_ConnectType_P2p,
	YangM_ConnectType_Sfu,
	YangM_ConnectType_Datachannel,
	YangM_ConnectType_User
}YangIpcConnectType;

typedef enum{
	YangIpcRequestNoQueryFile =-3,
	YangIpcRequestNotLogin   = -2,
	YangIpcRequestError      = -1,

	YangIpcRequestLogin =0,
	YangIpcRequestLoginResponse,
	YangIpcRequestConnect,
	YangIpcRequestConnectSdp,
	YangIpcRequestAnswer,
	YangIpcRequestCandidate,
	YangIpcRequestSfuSuccess,
	YangIpcRequestSfuFail,
	YangIpcRequestClose,
	YangIpcRequestData,
	YangIpcRequestPing
}YangIpcRequestType;

typedef struct{
	YangIpcConnectType connectType;
	YangIpcRequestType requestType;
	int32_t  uid;
	uint8_t isHttps;
	char cid[Yang_Cid_Length];
	char* data;
	char* url;
}YangIpcRequest;

typedef struct{
	void* user;
	void (*addPeer)(char *sdp, int32_t  nb_data,char* response,char* remoteIp,int32_t uid,void* user);
	void (*addPeer2)(char** sdp,char* remoteIp,int32_t uid,void* user);
	void (*addFilePeer)(char* filename, char *data, int32_t  nb_data,char* response,char* remoteIp,int32_t  uid,void* user);
	void (*setLocalDescription)(char *sdp,int32_t  uid ,void* user);
	void (*setRemoteDescription)(char *sdp,int32_t  uid ,void* user);
	int32_t  (*addCandidate)(int32_t  uid,char* candidate,void* user);
	void (*close)(int32_t  uid,void* user);
}YangIpcHandle;

typedef struct{
	void* user;
	void (*answerP2p)(char *sdp ,void* user);
	void (*loginResponse)(void* user);
	int32_t  (*answerSfu)(void* user);
	int32_t  (*addIceCandidate)(char* candidateStr,void* user);
}YangIpcPlayCallback;

#endif /* INCLUDE_YANGSIGNAL_YANGIPCMESSAGEDEF_H_ */
