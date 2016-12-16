PROGRAM=main
EXTRA_COMPONENTS=../lib/FreeRTOS_CLI extras/mbedtls
#../lib/SmingPlatform

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
