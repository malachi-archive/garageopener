INC_DIRS += $(freertos_cpp_ROOT)FreeRTOScpp
#freertos_cpp_INC_DIR += $(freertos_cpp_ROOT)FreeRTOScpp

$(eval $(call component_compile_rules,freertos_cpp))
