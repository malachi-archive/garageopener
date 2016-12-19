INC_DIRS += $(mbedos_ROOT)mbed-os/features
INC_DIRS += $(mbedos_ROOT)mbed-os/platform
INC_DIRS += $(mbedos_ROOT)mbed-shim/TARGET_esp8266/lwip
INC_DIRS += $(mbedos_ROOT)mbed-shim/rtos/rtx

mbedos_INC_DIR=$(mbedos_ROOT)mbed-shim/TARGET_esp8266
# for lwip-interface to find its includes
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os/features/netsocket
#mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os/features/FEATURE_LWIP/lwip-interface
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-shim
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os/drivers

mbedos_SRC_DIR=$(mbedos_ROOT)mbed-os/features/netsocket
#mbedos_SRC_DIR+=$(mbedos_ROOT)mbed-os/features/FEATURE_LWIP/lwip-interface

$(eval $(call component_compile_rules,mbedos))
