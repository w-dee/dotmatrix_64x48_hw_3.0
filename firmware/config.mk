# config.mk
THIS_DIR := $(realpath $(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(THIS_DIR)/..
LIBS = $(ESP_LIBS)/SPI \
  $(ESP_LIBS)/Wire \
  $(ESP_LIBS)/ESP8266WiFi \
  $(ESP_LIBS)/WiFiClient \
  $(ESP_LIBS)/ESP8266WebServer \
  $(ESP_LIBS)/ESP8266mDNS 
  

UPLOAD_SPEED = 115200


UPLOAD_RESET = nodemcu

