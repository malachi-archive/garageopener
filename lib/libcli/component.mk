INC_DIRS += $(libcli_ROOT)src
#freertos_cpp_INC_DIR += $(freertos_cpp_ROOT)FreeRTOScpp

libcli_SRC_DIR=$(libcli_ROOT)src

$(eval $(call component_compile_rules,libcli))
