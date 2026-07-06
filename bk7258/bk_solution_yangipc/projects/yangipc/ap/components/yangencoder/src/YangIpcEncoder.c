//
// Copyright (c) 2019-2026 yanggaofeng
//

#include "YangIpcEncoder.h"

#include "frame/frame_que_v2.h"
#include "frame_buffer.h"
#include "yangipc_comm.h"
#include "yangipc_devices.h"
#include "yangipc_audio_device.h"

#define yang_malloc psram_malloc
#define yang_calloc(x,y) psram_zalloc(x*y)
#define yang_free(a) {if( (a)) {psram_free((a)); (a) = NULL;}}

#define Yang_Ok 0
#define yangtrue 1
#define yangfalse 0
#define yang_trace( fmt, ...) BK_LOGI("metaRTC",fmt, ##__VA_ARGS__)
#define yang_error( fmt, ...) BK_LOGE("metaRTC",fmt, ##__VA_ARGS__)

#define Yang_Audio_FrameSize 160

typedef enum {
    H264_NAL_UNSPECIFIED     = 0,
    H264_NAL_SLICE           = 1,
    H264_NAL_DPA             = 2,
    H264_NAL_DPB             = 3,
    H264_NAL_DPC             = 4,
    H264_NAL_IDR_SLICE       = 5,
    H264_NAL_SEI             = 6,
    H264_NAL_SPS             = 7,
    H264_NAL_PPS             = 8,
    H264_NAL_AUD             = 9,
    H264_NAL_END_SEQUENCE    = 10,
    H264_NAL_END_STREAM      = 11,
    H264_NAL_FILLER_DATA     = 12,
    H264_NAL_SPS_EXT         = 13,
    H264_NAL_PREFIX          = 14,
    H264_NAL_SUB_SPS         = 15,
    H264_NAL_DPS             = 16,
    H264_NAL_RESERVED17      = 17,
    H264_NAL_RESERVED18      = 18,
    H264_NAL_AUXILIARY_SLICE = 19,
    H264_NAL_EXTEN_SLICE     = 20,
    H264_NAL_DEPTH_EXTEN_SLICE = 21,
    H264_NAL_B_FRAME         = 22,
    H264_NAL_P_FRAME         = 23,
    H264_NAL_I_FRAME         = 24,
    H264_NAL_UNSPECIFIED25   = 25,
    H264_NAL_UNSPECIFIED26   = 26,
    H264_NAL_UNSPECIFIED27   = 27,
    H264_NAL_UNSPECIFIED28   = 28,
    H264_NAL_UNSPECIFIED29   = 29,
    H264_NAL_UNSPECIFIED30   = 30,
    H264_NAL_UNSPECIFIED31   = 31,
} h264_type_t;

#define FRAME_FLAG_IS_I_FRAME      (1 << H264_NAL_I_FRAME)     // 0x01000000
#define FRAME_FLAG_IS_P_FRAME      (1 << H264_NAL_P_FRAME)     // 0x00800000
#define FRAME_FLAG_HAS_SPS         (1 << H264_NAL_SPS)         // 0x00000080
#define FRAME_FLAG_HAS_PPS         (1 << H264_NAL_PPS)         // 0x00000100
#define FRAME_FLAG_HAS_IDR_SLICE   (1 << H264_NAL_IDR_SLICE)   // 0x00000020

typedef struct{
	int16_t capacity;
	int16_t audioSize;
	beken_mutex_t mutex;
	uint8_t* audioBuffer;
}YangAudioData;

typedef struct {
	yangbool isStart;
	yangbool isLoop;
	yangbool sendKeyframe;
	yangbool hasConnected;

	int32_t width;
	int32_t height;

	beken_thread_t threadId;
	YangAudioData audioData;
	YangCodecEnable codecEnable;
	YangCodecCallback codecCallback;
	YangPlayAudioCallback playAudioCallback;
} YangBkIpcVideo;

static void yang_put_data(YangAudioData* data,unsigned char *audioData, unsigned int len){
	if((data->audioSize+len)>data->capacity) return;
	rtos_lock_mutex(&data->mutex);
	memcpy(data->audioBuffer+data->audioSize,audioData,len);

	data->audioSize+=len;
	rtos_unlock_mutex(&data->mutex);
}

static void yang_init_data(YangAudioData* data){
	rtos_lock_mutex(&data->mutex);
	data->audioSize=0;
	rtos_unlock_mutex(&data->mutex);
}

static void yang_bk_video_sendMsgToEncoder(void *psession,
		YangRequestType request) {
	YangBkIpcVideo* session=(YangBkIpcVideo*)psession;

	if (session == NULL)
		return;

	if (request == Yang_Req_Sendkeyframe || request == Yang_Req_Connected) {
		session->sendKeyframe=yangtrue;
		yang_trace("BK_Encoder_RequestIDR\n");
	} else if (request == Yang_Req_HighLostPacketRate) {

	} else if (request == Yang_Req_LowLostPacketRate) {

	}
}


static int32_t find_next_start_code(const uint8_t *buf, int32_t start, int32_t end, int32_t *sc_len) {
    int32_t i;
    for (i = start; i <= end - 3; i++) {
      
        if (buf[i] == 0x00 && buf[i+1] == 0x00 && buf[i+2] == 0x03) {
            i += 2;
            continue;
        }

        if (buf[i] == 0x00 && buf[i+1] == 0x00 && buf[i+2] == 0x01) {
            *sc_len = 3;
            return i;
        }

        if (buf[i] == 0x00 && buf[i+1] == 0x00 && buf[i+2] == 0x00 && (i + 3 < end) && buf[i+3] == 0x01) {
            *sc_len = 4;
            return i;
        }
    }
    
    *sc_len = 0;
    return -1;
}

static void yang_send_p(YangBkIpcVideo *session, frame_buffer_t *frame, YangFrame* videoFrame) {
    uint8_t naluType;
    uint8_t *buf = frame->frame;
		
    int32_t len = frame->length;
    int32_t pos = 0;
	int32_t sc_len=0;
	int32_t sc_pos = find_next_start_code(buf, pos, len, &sc_len);
	if(sc_pos==-1) return;
	naluType=(*(buf+sc_len)) & 0x1F;
	if(naluType==1){
		 videoFrame->payload = buf + sc_len;
         videoFrame->nb = frame->length-sc_len;
         session->codecCallback.onVideoData(session->codecCallback.session, videoFrame);
         return;
	}
	pos=sc_pos+sc_len;
	sc_pos = find_next_start_code(buf, pos, len, &sc_len);
	if(sc_pos==-1 || sc_pos>len) return;
	
	videoFrame->payload = buf + sc_pos + sc_len;
    videoFrame->nb = frame->length-sc_pos-sc_len;
    session->codecCallback.onVideoData(session->codecCallback.session, videoFrame);  		
}

YangBkIpcVideo *g_yang_session=NULL;

int webrtc_voice_send_callback(unsigned char *data, unsigned int len, void *args)
{
	if(g_yang_session==NULL)
		return 0;

	yang_put_data(&g_yang_session->audioData,data,(int32_t)len);

	return 0;
}


static void yang_bk_video_thread(beken_thread_arg_t obj){
	uint32_t baseTs=0;
	frame_buffer_t *frame;
	YangBkIpcVideo *session = (YangBkIpcVideo*) obj;

	YangFrame audioFrame = { 0 };
	YangFrame videoFrame = { 0 };

	audioFrame.nb=Yang_Audio_FrameSize;
	session->isLoop = yangtrue;
	session->isStart = yangtrue;
	session->hasConnected=yangfalse;

	while (session->isLoop) {

		if(session->codecEnable.session&&session->codecEnable.enable){
			if(!session->codecEnable.enable(session->codecEnable.session)){
				session->hasConnected=yangfalse;
				rtos_delay_milliseconds(5);
				continue;
			}
		}
		if(!session->hasConnected){

			session->hasConnected=yangtrue;
			if(g_yang_session==NULL)
				g_yang_session=session;
			yang_init_data(&session->audioData);
			baseTs=0;
		}

		if(session->audioData.audioSize>=Yang_Audio_FrameSize){
			rtos_lock_mutex(&session->audioData.mutex);
			audioFrame.payload=session->audioData.audioBuffer;
			session->codecCallback.onAudioData(session->codecCallback.session,&audioFrame);
			session->audioData.audioSize-=Yang_Audio_FrameSize;
			if(session->audioData.audioSize<0)
				session->audioData.audioSize=0;
			else if(session->audioData.audioSize>0)
				memmove(session->audioData.audioBuffer,session->audioData.audioBuffer+Yang_Audio_FrameSize,session->audioData.audioSize);

			rtos_unlock_mutex(&session->audioData.mutex);
		}

		if(session->sendKeyframe){
			yangipc_camera_idr_rest();		
			session->sendKeyframe=yangfalse;
		}

		frame = frame_queue_v2_get_frame(IMAGE_H264, CONSUMER_TRANSMISSION, 200);
		if (!frame) { rtos_delay_milliseconds(10); continue; }
		if (frame->length < 4) { 
			frame_queue_v2_release_frame(IMAGE_H264, CONSUMER_TRANSMISSION, frame);
			continue;
		}
		if(baseTs==0) baseTs=frame->timestamp;
		
		videoFrame.dts = videoFrame.pts = frame->timestamp-baseTs;

		videoFrame.frametype = (frame->h264_type & FRAME_FLAG_IS_I_FRAME) != 0?1:0;

		if(videoFrame.frametype){
			videoFrame.nb = frame->length;
			videoFrame.payload=frame->frame;
			session->codecCallback.onVideoData(session->codecCallback.session,&videoFrame);
		}else{
			yang_send_p(session,frame,&videoFrame);
		}
		
		frame_queue_v2_release_frame(IMAGE_H264, CONSUMER_TRANSMISSION, frame);
		
		rtos_delay_milliseconds(15);
	}

	yang_trace("yang_bkEncoder_close_thread \n");

	session->isStart = yangfalse;
	rtos_delete_thread(NULL);
}

static int32_t yang_bk_video_start(void *psession) {
	int32_t err;
	YangBkIpcVideo *session = (YangBkIpcVideo*) psession;

	if (session == NULL)
		return 1;

	if (session->isStart)
		return Yang_Ok;

	err=rtos_create_thread(&session->threadId,6,"yang_ipcEncoder",
				(beken_thread_function_t)yang_bk_video_thread,1024*4,(beken_thread_arg_t)session);
	if (err) 
		yang_error("YangThread::start could not start thread");

	return Yang_Ok;
}

static void yang_bk_video_stop(void *psession) {
	YangBkIpcVideo *session=(YangBkIpcVideo*) psession;
	if (psession == NULL)
		return;

	session->isLoop = yangfalse;
}

static int32_t yang_bk_video_init(void *psession) {
	YangBkIpcVideo *session = (YangBkIpcVideo*) psession;
	if (session == NULL)
		return 1;

	(void)session;
	return Yang_Ok;
}

static void yang_audio_onAudioData(void* psession,uint8_t* audioData,int32_t len){
	YangBkIpcVideo *session = (YangBkIpcVideo*) psession;
	if (session == NULL)
		return;

	yangipc_audio_data_callback(audioData,len);
}

static YangPlayAudioCallback* yang_getPlayAudioCallback(void* psession){
	YangBkIpcVideo *session = (YangBkIpcVideo*) psession;
	if (session == NULL)
		return NULL;
		
	return &session->playAudioCallback;
}

int32_t yang_create_ipcEncoder(YangIpcEncoder* encoder,YangVideoConfig* config,YangCodecEnable *codecEnable,YangCodecCallback *codecCallback)
{
	YangBkIpcVideo* session;
	if(encoder==NULL || codecCallback==NULL)
		return 1;
		
	session=(YangBkIpcVideo*)yang_calloc(sizeof(YangBkIpcVideo),1);
	encoder->session=session;
	session->width=config->width;
	session->height=config->height;
	if(codecCallback){
		session->codecCallback.session=codecCallback->session;
		session->codecCallback.onAudioData=codecCallback->onAudioData;
		session->codecCallback.onVideoData=codecCallback->onVideoData;
	}

	if(codecEnable){
		session->codecEnable.session=codecEnable->session;
		session->codecEnable.enable=codecEnable->enable;
	}
	
	session->playAudioCallback.session=session;
	session->playAudioCallback.onAudioData=yang_audio_onAudioData;
	
	session->audioData.capacity=16*Yang_Audio_FrameSize;
	session->audioData.audioBuffer=(uint8_t*)yang_malloc(session->audioData.capacity);
	rtos_init_mutex(&session->audioData.mutex);

	encoder->init = yang_bk_video_init;
	encoder->start = yang_bk_video_start;
	encoder->stop = yang_bk_video_stop;
	
	encoder->getPlayAudioCallback = yang_getPlayAudioCallback;
	encoder->sendMsgToEncoder = yang_bk_video_sendMsgToEncoder;

	return Yang_Ok;
}

void yang_destroy_ipcEncoder(YangIpcEncoder* encoder){
	YangBkIpcVideo* session;
	if(encoder==NULL||encoder->session==NULL)
		return;
		
	session=(YangBkIpcVideo*)encoder->session;
	g_yang_session=NULL;
	session->isLoop=yangfalse;
	while(session->isStart){
		rtos_delay_milliseconds(10);
	}
	rtos_deinit_mutex(&session->audioData.mutex);
	yang_free(session->audioData.audioBuffer);
	yang_free(encoder->session);
}
