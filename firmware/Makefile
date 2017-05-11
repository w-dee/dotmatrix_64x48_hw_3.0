VERBOSE=1
include ~/Arduino/makeEspArduino/makeEspArduino.mk

FONT_FILE_NAME=fonts/takaop.bff
FONT_FILE_START_ADDRESS=0x100000

FONT_UPLOAD_COM = $(shell echo $(UPLOAD_COM) | perl -npe "s! -ca .*! -ca $(FONT_FILE_START_ADDRESS) -cf $(FONT_FILE_NAME)!")


font_upload:
	$(FONT_UPLOAD_COM)


