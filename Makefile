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
ETL_INCLUDES += ./external/etl/include/etl/unordered_map.h
ETL_INCLUDES += ./external/etl/include/etl/set.h

# Source and header files
APP_HEADER_PATHS += ./include
APP_SOURCE_PATHS = ./src
APP_SOURCE_PATHS += ./src/user_programs
APP_SOURCE_PATHS += ./src/drivers
APP_SOURCE_PATHS += ./src/faults
APP_SOURCE_PATHS += ./src/hal
APP_SOURCE_PATHS += ./src/ipc
APP_SOURCE_PATHS += ./src/scheduler
APP_SOURCE_PATHS += ./src/userlib
APP_SOURCE_PATHS += ./src/svc

# I hate make
rwildcard = $(foreach d,$(wildcard $1*), \
                $(call rwildcard,$d/,$2)) $(wildcard $1$2)

APP_SOURCES = $(foreach dir,$(APP_SOURCE_PATHS),$(call rwildcard,$(dir)/,*.cpp))

# Path to base of nRF52x-base repo
NRF_BASE_DIR = external/nrf52x-base/

# Include board Makefile (if any)
include external/microbit_v2/Board.mk

LINKER_SCRIPT = edge_os.ld

# Include main Makefile
include $(NRF_BASE_DIR)/make/AppMakefile.mk


CPP_FILES := $(wildcard src/*.cpp include/*.hpp)

.PHONY: format
format:
	find src include -name '*.cpp' -o -name '*.hpp' | xargs clang-format --style=file -i

.PHONY: lint
lint: format

