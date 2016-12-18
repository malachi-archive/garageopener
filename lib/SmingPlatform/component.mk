INC_DIRS += $(SmingPlatform_ROOT)/src_rsync/SmingCore

SmingPlatform_SRC_DIR += $(SmingPlatform_ROOT)/src_rsync/SmingCore
#SmingPlatform_SRC_DIR += $(SmingPlatform_ROOT)/src_rsync/Wiring

$(eval $(call component_compile_rules,SmingPlatform))
