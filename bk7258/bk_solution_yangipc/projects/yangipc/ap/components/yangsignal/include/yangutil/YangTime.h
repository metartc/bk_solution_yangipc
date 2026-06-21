//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef YANG_TIME_H__
#define YANG_TIME_H__

#include <yangutil/yangtype.h>
#ifdef __cplusplus
extern "C"{
#include <yangutil/YangCTime.h>
}
#else
#include <yangutil/YangCTime.h>
#endif

#define yang_get_system_time yang_get_system_micro_time
#define yang_update_system_time yang_get_system_micro_time

#endif
