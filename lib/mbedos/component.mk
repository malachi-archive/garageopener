INC_DIRS += $(mbedos_ROOT)mbed-os/features
INC_DIRS += $(mbedos_ROOT)mbed-os/platform

mbedos_INC_DIR=$(mbedos_ROOT)mbed-shim/TARGET_esp8266
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-shim
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os
mbedos_INC_DIR+=$(mbedos_ROOT)mbed-os/drivers
mbedos_SRC_DIR=$(mbedos_ROOT)mbed-os/features/netsocket
#freertos_cpp_INC_DIR += $(freertos_cpp_ROOT)FreeRTOScpp

$(eval $(call component_compile_rules,mbedos))
