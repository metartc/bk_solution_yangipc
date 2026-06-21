#ifndef __YANGIPC_SDP_H__
#define __YANGIPC_SDP_H__

typedef struct
{
    char *data;
    uint16_t length;
} sdp_data_t;

int yangipc_sdp_start(const char *name, uint32_t cmd_port, uint32_t img_port, uint32_t aud_port);
int yangipc_sdp_stop(void);
int yangipc_sdp_reload(void);

#endif
