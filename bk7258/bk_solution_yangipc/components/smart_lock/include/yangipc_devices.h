#ifndef __YANGIPC_DEVICES_H__
#define __YANGIPC_DEVICES_H__

#include <components/bk_video_pipeline/bk_video_pipeline_types.h>
#include <components/bk_camera_ctlr.h>
#include <yangipc_transmission.h>

#include "wifi_transfer.h"
#include "components/bk_display.h"


typedef struct
{
    uint16_t id;
    uint16_t width;
    uint16_t height;
    uint16_t format;
    uint16_t protocol;
    uint16_t rotate;

#ifdef CONFIG_STANDARD_DUALSTREAM
    uint16_t dualstream;
    uint16_t d_width;
    uint16_t d_height;
#endif
} camera_parameters_t;

typedef struct
{
    uint16_t id;
    uint16_t rotate_angle;
    uint8_t  pixel_format;
} display_parameters_t;

typedef struct
{
    uint16_t lcd_id;
    const void *lcd_device;
    bk_video_pipeline_handle_t video_pipeline_handle;
    bk_display_ctlr_handle_t display_ctlr_handle;
    camera_type_t cam_type;
    image_format_t transfer_format;
    bk_camera_ctlr_handle_t handle;
    media_transfer_cb_t *video_transfer_cb;
} db_device_info_t;

int yangipc_get_supported_camera_devices(int opcode, db_channel_t *channel, yangipc_transmission_send_t cb);
int yangipc_get_supported_lcd_devices(int opcode, db_channel_t *channel, yangipc_transmission_send_t cb);
int yangipc_get_lcd_status(int opcode, db_channel_t *channel, yangipc_transmission_send_t cb);
void yangipc_devices_deinit(void);
int yangipc_devices_init(void);

int yangipc_camera_turn_on(camera_parameters_t *parameters);
int yangipc_camera_turn_off(void);

int yangipc_display_turn_on(display_parameters_t *parameters);
int yangipc_display_turn_off(void);


/**
 * @brief      开启视频传输功能
 *
 * @return     int 操作结果
 * BK_OK: 成功
 * BK_ERR: 失败
 * BK_ERR_NOT_SUPPORT: 不支持该操作
 */
int yangipc_video_transfer_turn_on(void);

/**
 * @brief      关闭视频传输功能
 *
 * @return     int 操作结果
 * BK_OK: 成功
 * BK_ERR: 失败
 * BK_ERR_NOT_SUPPORT: 不支持该操作
 */
int yangipc_video_transfer_turn_off(void);


/**
 * @brief      设置视频传输回调函数
 *
 * @param      cb  视频传输回调函数指针
 *
 * @return     int 操作结果
 * BK_OK: 成功
 * BK_ERR: 失败
 * BK_ERR_INVALID_PARAM: 无效参数
 * BK_ERR_NOT_SUPPORT: 不支持该操作
 */
int yangipc_devices_set_camera_transfer_callback(void *cb);

#endif
