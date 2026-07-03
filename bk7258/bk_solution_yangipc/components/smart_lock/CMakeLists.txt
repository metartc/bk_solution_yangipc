set(incs include .)
set(srcs)

if (CONFIG_INTEGRATION_YANGIPC)
	list(APPEND srcs
		src/yangipc_core.c
		src/yangipc_boarding.c
		src/yangipc_sdp.c
		src/yangipc_network.c
		src/yangipc_transmission.c
		src/yangipc_udp_service.c
		src/yangipc_tcp_service.c
		src/yangipc_audio_device.c
		src/yangipc_devices.c
		src/yangipc_cmd.c
		src/bk_smart_config.c
		src/camera/uvc.c
		src/camera/dvp.c
		src/decoder/mjpeg_decoder.c
		src/yuv_display/yuv_display.c
	)

	# V1版本：多消费者帧队列管理（可选）
	# 取消下面的注释来启用V1版本
	# list(APPEND srcs
	# 	src/frame/frame_que.c
	# )

	# V2版本：多消费者帧队列管理（可选）
	# 取消下面的注释来启用V2版本
	list(APPEND srcs
		src/frame/frame_que_v2.c
	)

	# V2测试代码（可选，仅用于开发和测试）
	# 取消下面的注释来启用测试
	# list(APPEND srcs
	# 	src/frame/frame_que_v2_test.c
	# )

if (CONFIG_INTEGRATION_YANGIPC)
	list(APPEND srcs
		src/camera/yangipc_camera.c
	)
endif()#CONFIG_INTEGRATION_YANGIPC



endif()

#set(priv_req multimedia bk_display media_utils wifi_transfer avdk_utils lwip_intf_v2_1 bk_wifi_driver cs2p2p bt bk_video_pipeline bk_camera)
set(priv_req multimedia media_utils wifi_transfer avdk_utils lwip_intf_v2_1 bk_wifi_driver cs2p2p bt bk_video_pipeline bk_camera)
armino_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}" PRIV_REQUIRES "${priv_req}"
)
