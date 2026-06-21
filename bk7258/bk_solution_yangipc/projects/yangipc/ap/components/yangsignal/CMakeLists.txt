set(incs include .)
set(srcs)

if (CONFIG_YANGSIGNAL)
	list(APPEND incs	
		include
	)
	file(GLOB_RECURSE yangsignal_source
		src/yangsignal/*.c
        )
	list(APPEND srcs ${yangsignal_source})
endif()
set(priv_req app  avdk_utils lwip_intf_v2_1 bk_vfs bk_rtos psa_mbedtls)

armino_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}" PRIV_REQUIRES "${priv_req}")

list(APPEND c_compile_options "-DCMAKE_BUILD_TYPE=Release")
list(APPEND c_compile_options "-D_POSIX_SOURCE")
list(APPEND c_compile_options "-Wno-error=unused-but-set-variable")
list(APPEND c_compile_options "-Wno-error=incompatible-pointer-types")
list(APPEND c_compile_options "-Wno-error=unused-variable")
list(APPEND c_compile_options "-enable-non-contiguous-regions")
list(APPEND c_compile_options "-Wno-format")
list(APPEND c_compile_options "-Wno-missing-field-initializers")
list(APPEND c_compile_options "-Wno-write-strings")
list(APPEND c_compile_options "-Wno-unused-value") 
list(APPEND c_compile_options "-Wno-pointer-sign") 
list(APPEND c_compile_options "-Wno-int-conversion")
list(APPEND c_compile_options "-Wno-error=address") 
list(APPEND c_compile_options "-Wno-error=enum-compare") 
list(APPEND c_compile_options "-Wno-maybe-uninitialized")
list(APPEND c_compile_options "-Wno-error=unused-const-variable")
list(APPEND c_compile_options "-Wno-error=pointer-to-int-cast")

armino_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}" APPEND)

