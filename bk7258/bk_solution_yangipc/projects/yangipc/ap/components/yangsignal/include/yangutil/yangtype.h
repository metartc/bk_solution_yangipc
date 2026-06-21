//
// Copyright (c) 2019-2026 yanggaofeng
//
#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_

#include <stdlib.h>
#include <stdint.h>
#include <yangutil/yang_config_os.h>

#include <yangutil/yangunistd.h>
#include <yangutil/yangmemory.h>
#include <yangutil/yangerrorcode.h>


#if !Yang_OS_WIN
#define Yang_Enable_Phtread 1
#endif

typedef  uint8_t yangbool;
#define yangtrue 1
#define yangfalse 0

#define yang_free(a) {if( (a)) {yangfree((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))
#define yang_abs(a, b) ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

#define YANGALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#define YANG_INADDR_ANY 0x00000000


typedef enum{
	Yang_Socket_Protocol_Udp,
	Yang_Socket_Protocol_Tcp
}YangSocketProtocol;

typedef enum {
	Yang_IpFamilyType_IPV4,
	Yang_IpFamilyType_IPV6
} YangIpFamilyType;



#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
