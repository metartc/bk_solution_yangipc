//
// Copyright (c) 2019-2026 yanggaofeng
//

#ifndef INCLUDE_YANGIPC_YANGCANDIDATECALLBACK_H_
#define INCLUDE_YANGIPC_YANGCANDIDATECALLBACK_H_
#include <stdint.h>
typedef struct{
	void* user;
	int32_t  (*addIceCandidate)(void* user,int32_t  uid,char* candidateStr);
	//void (*onConnectionStateChange)(void* user, int32_t  uid,YangRtcConnectionState connectionState);
}YangIpcDataCallback;
#endif /* INCLUDE_YANGIPC_YANGCANDIDATECALLBACK_H_ */
