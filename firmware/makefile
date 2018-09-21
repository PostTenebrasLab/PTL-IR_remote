SKETCH = test_PTL_IR_remote_shield

DIR = $(shell pwd)/$(SKETCH)
BPATH = /usr/share/arduino
APATH = /home/seba/.arduino15/packages

LIBRARIES = /home/seba/Arduino/libraries
BUILD_DIR = $(DIR)/build
BCACHE = $(DIR)/cache
CC = $(BPATH)/arduino-builder
HARDWARE = $(BPATH)/hardware
TOOLS = $(BPATH)/tools-builder

WARNINGS = -warnings=all
LOG = -logger=machine
DEVICE = esp8266:esp8266:d1_mini:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,Debug=Disabled,DebugLevel=None____,FlashErase=none,UploadSpeed=921600 
IVER = -ide-version=10805


NAME = lm00
OTASERVER = http:\/\/172.16.1.2\/firmware
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

#$(BUILD_DIR)/$(NAME).bin: $(BUILD_DIR)/$(BIN)

$(BUILD_DIR)/$(BIN): # $(DIR)/$(INO) $(DIR)/$(HEADER).org
	-mkdir -p $(BUILD_DIR)
	-mkdir -p $(BCACHE)
	@sed -e 's/NNNN/$(NAME)/g' -e 's/BBBB/$(OTASERVER)/g' -e 's/CCCC/$(MQTTSERVER)/g' $(DIR)/$(HEADER).org > $(DIR)/$(HEADER)
	$(CC) $(DUMP) $(CFLAGS) $(PREFS) $(VERBOSE) $(DIR)/$(INO)
	$(CC) $(COMPILE) $(CFLAGS) $(PREFS) $(VERBOSE) $(DIR)/$(INO)
	mv $< $(BUILD_DIR)/$(NAME).bin

#upload: $(BUILD_DIR)/$(BIN)
#	$(APATH)/esp8266/tools/esptool/0.4.13/esptool -vv -cd nodemcu -cb 921600 -cp /dev/ttyUSB1 -ca 0x00000 -cf $<

clean:
	-rm  -rf $(BUILD_DIR)/* $(BCACHE)/*

