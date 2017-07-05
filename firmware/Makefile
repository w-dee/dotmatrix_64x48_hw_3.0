VERBOSE=1
ESP_ROOT := /workspace/esp8266/Arduino
include ~/Arduino/makeEspArduino/makeEspArduino.mk

SETTINGS_FS_UPLOAD_COM := $(shell echo $(FS_UPLOAD_COM) | perl -npe 's! -ca .*! -ca $(SETTINGS_SPIFFS_START) -cf $(SETTINGS_FS_IMAGE)!' )

SETTINGS_MKSPIFFS_COM := $(shell echo $(MKSPIFFS_COM) | perl -npe 's! -s .*! -s $(SETTINGS_SPIFFS_SIZE) -c settings $(SETTINGS_FS_IMAGE)!' )

MKSPIFFS_COM := $(shell echo $(MKSPIFFS_COM) | perl -npe 's! -s [^ ]*! -s $(MY_SPIFFS_SIZE)!' ) &&\
	cat $(BFF_FONT_FILE_NAME) >> $(FS_IMAGE)

$(FS_IMAGE): $(BFF_FONT_FILE_NAME)

COMBINED_IMAGE_ALIGN := 2048

COMBINED_HEADER_PERL:= perl -e \
	'my $$part = shift; my $$fn = shift; print "\xea PART $$part BLOCK HEADER           "; my $$sz = -s $$fn; \
	print pack("VV", -s $$fn, int(($$sz + ($(COMBINED_IMAGE_ALIGN) - 1)) / ($(COMBINED_IMAGE_ALIGN)) )); \
	print "?" x (($(COMBINED_IMAGE_ALIGN))-32-4-4-32)'

COMBINED_PADDING_PERL:= perl -e \
	'my $$fn = shift; my $$sz = -s $$fn; \
	my $$padding = (($$sz + ($(COMBINED_IMAGE_ALIGN) - 1)) & ~($(COMBINED_IMAGE_ALIGN) - 1)) - $$sz; \
	print "-" x $$padding;'

.PHONY: settings_fs_image
settings_fs_image: $(SETTINGS_FS_IMAGE)

$(SETTINGS_FS_IMAGE):  Makefile config.mk
	$(SETTINGS_MKSPIFFS_COM)

.PHONY: upload_settings_fs
upload_settings_fs: $(SETTINGS_FS_IMAGE)
	$(SETTINGS_FS_UPLOAD_COM)

.PHONY: combined_rom_image
combined_rom_image: $(COMBINED_ROM_IMAGE)

$(COMBINED_ROM_IMAGE): $(FS_IMAGE) $(MAIN_EXE)
	rm -f $(COMBINED_ROM_IMAGE)
	#
	$(COMBINED_HEADER_PERL) M $(MAIN_EXE)  >> $(COMBINED_ROM_IMAGE)
	(md5sum $(MAIN_EXE) | cut -d' ' -f1 | tr -d '\n') >> $(COMBINED_ROM_IMAGE)
	cat $(MAIN_EXE) >> $(COMBINED_ROM_IMAGE)
	$(COMBINED_PADDING_PERL) $(MAIN_EXE) >> $(COMBINED_ROM_IMAGE)
	#
	$(COMBINED_HEADER_PERL) F $(FS_IMAGE)  >> $(COMBINED_ROM_IMAGE)
	(md5sum $(FS_IMAGE) | cut -d' ' -f1 | tr -d '\n') >> $(COMBINED_ROM_IMAGE)
	cat $(FS_IMAGE) >> $(COMBINED_ROM_IMAGE)
	$(COMBINED_PADDING_PERL) $(FS_IMAGE) >> $(COMBINED_ROM_IMAGE)

	
