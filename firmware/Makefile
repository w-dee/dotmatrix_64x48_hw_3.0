VERBOSE=1
ESP_ROOT := $(HOME)/workspace/esp8266/Arduino
include ~/Arduino/makeEspArduino/makeEspArduino.mk


MKSPIFFS_COM := $(shell echo $(MKSPIFFS_COM) | perl -npe "s! -s [^ ]*! -s $(MY_SPIFFS_SIZE)!" ) &&\
	cat $(BFF_FONT_FILE_NAME) >> $(FS_IMAGE)


