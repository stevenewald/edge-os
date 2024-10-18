# Building

1. Install `bear` and `arm-none-eabi-gcc` relevant to your system. Ensure both the C and C++ compilers for `arm-none-eabi-gcc` are installed.
2. Run `bear -- make -j` to generate the `compile_commands.json` file while building the project.

# Flashing

`bear` is used only to generate `compile_commands.json`. For flashing, you can run `make flash -j` directly or prepend `bear --` if you need to regenerate `compile_commands.json` during the process.
