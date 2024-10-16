PROJECT_NAME = EdgeOS

# Configurations
NRF_IC = nrf52833
SDK_VERSION = 16
SOFTDEVICE_MODEL = blank

# Source and header files
APP_HEADER_PATHS += ./include
APP_SOURCE_PATHS += ./src
APP_SOURCES = $(notdir $(wildcard src/*.c))

# Path to base of nRF52x-base repo
NRF_BASE_DIR = sdk/nrf52x-base/

# Include board Makefile (if any)
include sdk/microbit_v2/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)/make/AppMakefile.mk
