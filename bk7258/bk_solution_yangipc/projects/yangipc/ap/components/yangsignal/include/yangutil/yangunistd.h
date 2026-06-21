//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANG_UNISTD_H_
#define INCLUDE_YANGUTIL_YANG_UNISTD_H_

#include "yang_config_os.h"
#ifdef __cplusplus
extern "C"{
#endif

void yang_usleep_win(uint32_t milli_seconds);
void yang_msleep_win(uint32_t milli_seconds);
void yang_sleep_us(uint32_t micro_seconds);
void yang_sleep_ms(uint32_t milli_seconds);

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <Windows.h>
#define yang_sleep(x) Sleep(1000*x)
#define yang_exit ExitProcess
#elif Yang_OS_RTOS
#include <posix/unistd.h>
#define yang_sleep(x) rtos_delay_milliseconds(x*1000)
#define yang_exit exit
#else
#include <unistd.h>
#define yang_sleep sleep
#define yang_exit _exit
#endif


#ifdef _WIN32
	#define yang_usleep yang_usleep_win
	#define yang_msleep yang_msleep_win
#elif Yang_OS_RTOS
	#define yang_usleep(x) rtos_delay_milliseconds(x/1000)
	#define yang_msleep rtos_delay_milliseconds
#else
	#define yang_usleep yang_sleep_us
	#define yang_msleep(x) yang_usleep(x*1000)
#endif


#endif /* INCLUDE_YANGUTIL_YANG_UNISTD_H_ */
