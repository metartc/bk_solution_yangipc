set(incs include  .)
set(srcs ap_main.c yang_ipc.c)

set(priv_req  media_service multimedia smart_lock wifi_transfer lwip_intf_v2_1 yang_libs)

armino_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}" PRIV_REQUIRES "${priv_req}")
