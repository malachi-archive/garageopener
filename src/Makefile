PROGRAM=main
EXTRA_COMPONENTS=../lib/FreeRTOS_CLI extras/mbedtls ../lib/FreeRTOS_cpp ../lib/util.embedded ../lib/YaCoAP
#../lib/SmingPlatform

PROGRAM_INC_DIR += ../ext/lwipcpp
PROGRAM_INC_DIR += ../ext/coapcpp
PROGRAM_INC_DIR += ../ext/mbedtlscpp
# this is to find lwip/inet.h file
PROGRAM_INC_DIR += $(ESP_OPEN_RTOS)/lwip/lwip/src/include/ipv4

PROGRAM_CPPFLAGS += -DESP_OPEN_RTOS -DFEATURE_IOSTREAM_SHIM -std=c++11
#PROGRAM_INC_DIR += $(freertos_cpp_ROOT)FreeRTOScpp

#ESPBAUD = 460800
#EXTRA_COMPONENTS = extras/spiffs
#FLASH_SIZE = 32

# spiffs configuration
#SPIFFS_BASE_ADDR = 0x200000
#SPIFFS_SIZE = 0x010000

include $(ESP_OPEN_RTOS)/common.mk

#$(eval $(call make_spiffs_image,files))

console:
	miniterm.py $(ESPPORT) 115200
