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

# flash rom layout:

# start    size         content
# ------------------------------
# 0        0.5M         Program text
# 0.5M     0.5M         Program text (shadow, for OTA update)
# 1M       896k         Main SPIFFS content and scratch pad
# 1920k    1920k        Bitmap font
# 3840k    128k         SPIFFS for settings data

# don't use builtin SPIFFS global instance; 
# we use special constructor which shrinks main SPIFFS

MY_SPIFFS_SIZE=917504
SETTINGS_SPIFFS_SIZE=131072
SETTINGS_FS_IMAGE=settings_fs.spiffs
SETTINGS_SPIFFS_START=0x3c0000

BFF_FONT_FILE_NAME=fonts/takaop.bff
BFF_FONT_FILE_START_ADDRESS=0x1e0000
BUILD_EXTRA_FLAGS += -DNO_GLOBAL_SPIFFS -DMY_SPIFFS_SIZE=$(MY_SPIFFS_SIZE) \
	-DSETTINGS_SPIFFS_SIZE=$(SETTINGS_SPIFFS_SIZE) \
	-DSETTINGS_SPIFFS_START=$(SETTINGS_SPIFFS_START) \
	-DBFF_FONT_FILE_START_ADDRESS=$(BFF_FONT_FILE_START_ADDRESS) \
	-DSSID_H="<$(HOME)/.config/ssid.h>"

# combined rom image
COMBINED_ROM_IMAGE=combined_rom.bin




