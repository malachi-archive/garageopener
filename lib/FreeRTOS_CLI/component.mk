INC_DIRS += $(FreeRTOS_CLI_ROOT)

FreeRTOS_CLI_SRC_DIR=$(FreeRTOS_CLI_ROOT)

$(eval $(call component_compile_rules,FreeRTOS_CLI))
