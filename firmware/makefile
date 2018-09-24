
NAME ?= lm00

ifdef TEST
SKETCH = test_PTL_IR_remote_shield
CNAME = $(NAME)_test
else
SKETCH = PTL_IR_remotev3
CNAME = $(NAME)
endif

PORT ?= /dev/ttyUSB0
DIR = $(PWD)/$(SKETCH)
BPATH = /usr/share/arduino
APATH = /home/seba/.arduino15/packages

LIBRARIES = /home/seba/Arduino/libraries
DIR = $(PWD)/$(SKETCH)
OUTPUT = $(PWD)/firmwares
BUILD_DIR = $(DIR)/build
BCACHE = $(DIR)/cache
CC = $(BPATH)/arduino-builder
HARDWARE = $(BPATH)/hardware
TOOLS = $(BPATH)/tools-builder

WARNINGS = -warnings=all
LOG = -logger=machine
DEVICE = esp8266:esp8266:d1_mini:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,Debug=Disabled,DebugLevel=None____,FlashErase=none,UploadSpeed=921600 
IVER = -ide-version=10805

NODES = lm00 lm001 lm01 lm02 lm03 lm04 lm05 lm06 lm07 lm08 lm09 lm10
OTASERVER = http:\/\/mqtt.localdomain:8080\/firmware
MQTTSERVER = mqtt.localdomain
INO = $(SKETCH:=.ino)
BIN = $(INO:=.bin)
HEADER = $(SKETCH:=.h)

DUMP = -dump-prefs
COMPILE = -compile
TPREFS = build.warn_data_percentage=75 runtime.tools.esptool.path=$(APATH)/esp8266/tools/esptool/0.4.13 runtime.tools.mkspiffs.path=$(APATH)/esp8266/tools/mkspiffs/0.2.0 runtime.tools.xtensa-lx106-elf-gcc.path=$(APATH)/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2 
PREFS = $(addprefix -prefs=, $(TPREFS))
VERBOSE = -verbose

CFLAGS = $(LOG) -hardware $(HARDWARE) -hardware $(APATH) -tools $(TOOLS) -tools $(APATH) -libraries $(LIBRARIES) -fqbn=$(DEVICE) $(IVER) -build-path $(BUILD_DIR) $(WARNINGS) -build-cache $(BCACHE)


$(BUILD_DIR)/$(BIN): $(DIR)/$(INO) $(DIR)/$(HEADER)
	-mkdir -p $(BUILD_DIR)
	-mkdir -p $(BCACHE)
	$(CC) $(DUMP) $(CFLAGS) $(PREFS) $(VERBOSE) $(DIR)/$(INO)
	$(CC) $(COMPILE) $(CFLAGS) $(PREFS) $(VERBOSE) $(DIR)/$(INO)
	mv $(BUILD_DIR)/$(BIN) $(OUTPUT)/$(CNAME).bin
	-rm $(DIR)/$(HEADER)

$(DIR)/$(HEADER): 
	@sed -e 's/NNNN/$(CNAME)/g' -e 's/BBBB/$(OTASERVER)/g' -e 's/CCCC/$(MQTTSERVER)/g' $(DIR)/$(HEADER).org > $(DIR)/$(HEADER)

upload: $(OUTPUT)/$(CNAME).bin
	$(APATH)/esp8266/tools/esptool/0.4.13/esptool -vv -cd nodemcu -cb 921600 -cp $(PORT) -ca 0x00000 -cf $<

sync_OTA:
	rsync -av --progress --delete $(OUTPUT) ptl@mqtt:/home/ptl/esp8266_OTA_server/public/

clean:
	-rm -rf $(DIR)/$(HEADER) $(BUILD_DIR)/* $(BCACHE)/*
	@echo $(CNAME) $(TEST)

help:
	@echo "Available targets:"
	@echo ""
	@echo "  Usage exemple"
	@echo ""
	@echo " 	make NAME=firmware00 TEST= PORT=/dev/ttyUSB0"
	@echo ""
	@echo "  help     : this help"
	@echo "  upload   : upload the bin (option PORT=/dev/ttyUSB0)"
	@echo "  clean    : clean all intermediary and target files"
	@echo "  sync_OTA : upload to OTA server"


.PHONY: upload sync_OTA clean 

