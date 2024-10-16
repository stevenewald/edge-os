# Commands and configurations for loading code onto hardware with JTAG
# Replaces the default Program.mk which uses JLink with new rules using OpenOCD
# Included by Board.mk

# Ensure that this file is only included once
# Stops the default Program.mk from being loaded
ifndef JTAG_MAKEFILE
JTAG_MAKEFILE = 1

# The first target Make finds is its default. So this line needs to be first to
# specify `all` as our default rule
all:


# Get directory of this makefile
OPENOCD_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))


# ---- OpenOCD configuration
OPENOCD = openocd
OPENOCD_OPTIONS = -f $(OPENOCD_DIR)/openocd.cfg


.PHONY: flash
flash: all
	$(Q) $(OPENOCD) $(OPENOCD_OPTIONS) -c "debug_level 0; program $(HEX); verify_image $(HEX); reset; shutdown;"

.PHONY: flash_debug
flash_debug: debug
	$(Q) $(OPENOCD) $(OPENOCD_OPTIONS) -c "debug_level 2; program $(DEBUG_HEX); verify_image $(DEBUG_HEX); reset; shutdown;"


# Softdevice
# No support right now for the softdevice. Doesn't seem like this repo needs it

# RTT
# No support right now for RTT. Doesn't seem like this repo needs it

# GDB
# No rule for GDB right now. But here are some steps to do it manually.
#
# 1. Run `make flash_debug` to load a version of code with debug symbols.
# 1. Run `openocd -f boards/microbit_v2/openocd.cfg` (leave that running).
# 2. To get an interface for OpenOCD run `telnet localhost 4444`. Then type
#    "reset halt" to stop the microcontroller.
# 3. To get a GDB interface run `arm-none-eabi-gdb -tui _build/blink_sdk16_blank_debug.elf`.
#    Hit enter. Then type "target remote localhost:3333" to attach GDB to OpenOCD.
#
# After all of that is completed, you can "break main" then "continue" to hit
# a breakpoint and pause. GDB commands like "next" will work normally at that
# point and you'll even see the code as you step through it!

endif

