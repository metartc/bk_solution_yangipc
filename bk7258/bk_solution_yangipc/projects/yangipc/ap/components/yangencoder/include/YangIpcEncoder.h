/*
 * YangIpcEncoder.h
 *
 *  Created on: 2026年5月27日
 *      Author: yang
 */

#ifndef SRC_YANGAPP_YANGIPCENCODER_H_
#define SRC_YANGAPP_YANGIPCENCODER_H_

#include <stdint.h>

typedef struct{
	int32_t width;
	int32_t height;
	int32_t fps;
}YangVideoConfig;

#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
typedef  uint8_t yangbool;
typedef struct{
	int32_t  uid;
	int32_t  mediaType;
	int32_t  frametype;
	int32_t  nb;
	int64_t  pts;
	int64_t  dts;
	uint8_t* payload;
}YangFrame;
#endif
#ifndef YangRequestType
typedef enum YangRequestType {
	Yang_Req_Sendkeyframe,
	Yang_Req_HighLostPacketRate,
	Yang_Req_LowLostPacketRate,
	Yang_Req_Connected,
	Yang_Req_Disconnected
}YangRequestType;
#endif
#ifndef INCLUDE_YANGCODEC_YANGCODEC_H_
typedef struct{
	void* session;
	void (*onAudioData)(void* session,YangFrame* pframe);
	void (*onVideoData)(void* session,YangFrame* pframe);
}YangCodecCallback;

typedef struct{
	void* session;
	yangbool (*enable)(void* session);
}YangCodecEnable;
#endif

typedef struct{
	void* session;
	void (*onAudioData)(void* session,uint8_t* audioData,int32_t len);
}YangPlayAudioCallback;

typedef struct{
	void* session;
	int32_t  (*init)(void* session);
	int32_t  (*start)(void* session);
	void (*stop)(void* session);
	void (*sendMsgToEncoder)(void* session,YangRequestType request);
	YangPlayAudioCallback* (*getPlayAudioCallback)(void* session);
}YangIpcEncoder;

int32_t yang_create_ipcEncoder(YangIpcEncoder* encoder,YangVideoConfig* config,YangCodecEnable *codecEnable,YangCodecCallback *callback);
void yang_destroy_ipcEncoder(YangIpcEncoder* encoder);

#endif /* SRC_YANGAPP_YANGIPCENCODER_H_ */
