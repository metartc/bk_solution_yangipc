//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef __YangIni__H_
#define __YangIni__H_

#include <yangutil/yangavinfotype.h>

typedef struct{
	char* filename;
	void (*initSys)(char* filename,YangSysInfo *sys);
    void (*initMqtt)(char* filename,YangMqttInfo *mqtt);
    void (*initFile)(char* filename,YangFileInfo *file);
    int32_t  (*readStringValue)(char* filename,const char *section, const char *key,char *val, const char *p_defaultStr);
    int32_t  (*readIntValue)(char* filename,const char *section, const char *key,	int32_t  p_defaultInt);
}YangIni;

#ifdef __cplusplus
extern "C"{
#endif
//void yang_create_ini(YangIni* ini,const char *filename);
void yang_create_ini(YangIni *ini,char* configPath, const char *filename);
void yang_destroy_ini(YangIni* ini);
#ifdef __cplusplus
}
#endif
#endif

