# config.mk
THIS_DIR := $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(THIS_DIR)/..
LIBS = $(ESP_LIBS)/SPI \
  $(ESP_LIBS)/Wire \
  $(ESP_LIBS)/ESP8266WiFi \
  $(ESP_LIBS)/ESP8266WebServer \
  $(ESP_LIBS)/ESP8266mDNS 


UPLOAD_SPEED = 921600
UPLOAD_RESET = nodemcu
FLASH_DEF = 4M3M

# shrink continuation stack size from 4kB to 2kB
CONT_STACKSIZE = 2048

# don't use builtin SPIFFS global instance; 
# we use special constructor which shrinks SPIFFS
# (the room after SPIFFS will contain font data)
MY_SPIFFS_SIZE=0x100000
BFF_FONT_FILE_NAME=fonts/takaop.bff
BFF_FONT_FILE_START_ADDRESS=0x200000
BUILD_EXTRA_FLAGS += -DNO_GLOBAL_SPIFFS -DMY_SPIFFS_SIZE=$(MY_SPIFFS_SIZE) \
	-DBFF_FONT_FILE_START_ADDRESS=$(BFF_FONT_FILE_START_ADDRESS) \
	-DCONT_STACKSIZE=$(CONT_STACKSIZE)



