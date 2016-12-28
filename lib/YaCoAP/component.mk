INC_DIRS += $(coap_ROOT)YaCoAP

coap_INC_DIR += $(coap_ROOT)include
coap_SRC_DIR += $(coap_ROOT)YaCoAP

# OK don't try to pull in EVERYTHING just yet!!
#sming_SRC_DIR += $(sming_ROOT)Sming/Sming/SmingCore

$(eval $(call component_compile_rules,coap))

#$(info INC_DIRS = ${INC_DIRS})
#$(info Component root = ${sming_ROOT})
#$(info Component inc_dir = ${sming_INC_DIR})
