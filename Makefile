PROJECT_NAME = EdgeOS

# Configurations
NRF_IC = nrf52833
SDK_VERSION = 16
SOFTDEVICE_MODEL = blank

# ETL
ETL_INCLUDES = ./external/etl/include/etl/array.h
ETL_INCLUDES += ./external/etl/include/etl/vector.h
ETL_INCLUDES += ./external/etl/include/etl/optional.h
ETL_INCLUDES += ./external/etl/include/etl/string.h
ETL_INCLUDES += ./external/etl/include/etl/string_stream.h
ETL_INCLUDES += ./external/etl/include/etl/to_string.h
ETL_INCLUDES += ./external/etl/include/etl/delegate.h

# Source and header files
APP_HEADER_PATHS += ./include
APP_SOURCE_PATHS += ./src
APP_SOURCES = $(notdir $(wildcard src/*.cpp))

# Path to base of nRF52x-base repo
NRF_BASE_DIR = external/nrf52x-base/

# Include board Makefile (if any)
include external/microbit_v2/Board.mk

# Include main Makefile
include $(NRF_BASE_DIR)/make/AppMakefile.mk


CPP_FILES := $(wildcard src/*.cpp include/*.hpp)

.PHONY: format
format:
	clang-format -style=file -i $(CPP_FILES)

.PHONY: tidy
tidy:
	clang-tidy $(CPP_FILES) -- -std=c++20

.PHONY: lint
lint: format tidy

