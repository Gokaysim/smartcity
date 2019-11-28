CONTIKI_PROJECT = coap-example-server
all: $(CONTIKI_PROJECT)

# Do not try to build on Sky because of code size limitation
PLATFORMS_EXCLUDE = sky z1

# Include the CoAP implementation
MODULES += os/net/app-layer/coap

# Include CoAP resources
MODULES_REL += ./resources

# Include the CoAP implementation
# MODULES += $(CONTIKI_NG_APP_LAYER_DIR)/coap

PROJECT_SOURCEFILES += queueSerializer.c
PROJECT_SOURCEFILES += nodeData.c

CONTIKI=../..
include $(CONTIKI)/Makefile.include
WERROR=0
