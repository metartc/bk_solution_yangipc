set(incs include .)
set(srcs)

armino_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}" PRIV_REQUIRES "${priv_req}")

file(GLOB libs ${CMAKE_CURRENT_SOURCE_DIR}/libs/*.a)

foreach(lib ${libs})
 	get_filename_component(lib_name ${lib} NAME)
 	add_prebuilt_library(${lib_name} "${CMAKE_CURRENT_SOURCE_DIR}/libs/${lib_name}")
 	target_link_libraries(${COMPONENT_LIB} INTERFACE ${lib_name})
endforeach()


