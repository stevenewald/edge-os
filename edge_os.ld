/* Linker script to configure memory regions. */
/* Only for gcc nrf52833 with 128kb ram and 512kb flash */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x80000
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 0x20000
}

FLASH_PAGE_SIZE       = 4096;
FLASH_DATA_PAGES_USED = 4;

SECTIONS
{
  . = ALIGN(4);
  .svc_data :
  {
    PROVIDE(__start_svc_data = .);
    KEEP(*(.svc_data))
    PROVIDE(__stop_svc_data = .);
  } > RAM
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM
  .user_programs_data ALIGN(16384) :
  {
	  PROVIDE(__start_user_programs_data = .);
	  _build/user_*.o(.data*)
	  /* _build/user_*.o(.bss*)
	   * bss needs to be within __bss_start__ and __bss_end__ to be zeroed by the c lib on startup
	   * however, that makes it really really hard to isolate
	   * I don't think this current isolation technique will work for bss, so I've enabled user read/write of all bss code
	   * If we made a real OS, we would have a different binary for each program, but because this is a mini os, we didn't bother.
	   *
	   * Because of this reason (and discovering it during finals week), I have disabled the MPU
	   * */
	  PROVIDE(__end_user_programs_data = .);
	  ASSERT((__end_user_programs_data - __start_user_programs_data) <= 16384, "Error: .user_programs_data section exceeds 16 KB");
  } > RAM
} INSERT AFTER .data;

SECTIONS
{
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
  /* TODO: make sure this doesnt cause duplication? */
  .user_programs_code ALIGN(16384) :
  {
	  PROVIDE(__start_user_programs_code = .);
	  _build/user_*.o(.text*)
	  _build/user_*.o(.rodata*)
	  KEEP(*(.user_code))
	  PROVIDE(__end_user_programs_code = .);
	  ASSERT((__end_user_programs_code - __start_user_programs_code) <= 16384, "Error: .user_programs_code section exceeds 16 KB");
  } > FLASH
} INSERT AFTER .text

SECTIONS
{
    __stop_ot_flash_data = (ORIGIN(FLASH) + LENGTH(FLASH) - FLASH_PAGE_SIZE);
    __start_ot_flash_data = (__stop_ot_flash_data - (FLASH_PAGE_SIZE * FLASH_DATA_PAGES_USED));

    /* Assure that code does not overlap flash data area.*/
    ASSERT((__start_ot_flash_data >= __etext + SIZEOF(.data)), "Error: Code overlaps flash data area.")
}

INCLUDE "nrf_common.ld"
