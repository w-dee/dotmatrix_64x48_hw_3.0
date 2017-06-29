VERBOSE=1
ESP_ROOT := $(HOME)/workspace/esp8266/Arduino
include ~/Arduino/makeEspArduino/makeEspArduino.mk


SETTINGS_MKSPIFFS_COM := $(shell echo $(MKSPIFFS_COM) | perl -npe 's! -s .*! -s $(SETTINGS_SPIFFS_SIZE) -c settings $(SETTINGS_FS_IMAGE)!' )

MKSPIFFS_COM := $(shell echo $(MKSPIFFS_COM) | perl -npe 's! -s [^ ]*! -s $(MY_SPIFFS_SIZE)!' ) &&\
	cat $(SETTINGS_FS_IMAGE) $(BFF_FONT_FILE_NAME) >> $(FS_IMAGE)



.PHONY: settings_fs_image
settings_fs_image: settings_fs_image.bin

settings_fs_image.bin:
	$(SETTINGS_MKSPIFFS_COM)

