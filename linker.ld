/* Memory regions */
MEMORY {
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH =  32K
}

/* Library configurations */
/*
GROUP(
	armv7e-m/libnosys.a
	armv7e-m/libgcc.a
	armv7e-m/libc.a
	armv7e-m/libm.a
)
*/

GROUP(
	armv7e-m/libgcc.a
)

ENTRY(Reset_Handler)

SECTIONS {
	.text : {
		KEEP(*(.isr_vectors))

		. = ALIGN(4);
		*(.text*)

		. = ALIGN(4);
		KEEP(*(.myevic*))

		. = ALIGN(4);
		*(.rodata*)

	} > ROM

	. = ALIGN(4);
	Data_Start_ROM = .;

	.data : AT (Data_Start_ROM) {
		Data_Start_RAM = .;
		*(vtable)
		*(.data*)

		. = ALIGN(4);
		Data_End_RAM = .;
	} > RAM

	Data_Size = Data_End_RAM - Data_Start_RAM;

	.sram : {
		SRAMZ_Start = .;

		. = ALIGN(4);
		*(COMMON)
		*(.bss*)

		end = .;

		. = ALIGN(8);
		*(.stack)

		SRAMZ_End = .;
	} > RAM

	SRAMZ_Size = SRAMZ_End - SRAMZ_Start;

	.text : {
		. = ALIGN(4);
		RAMInitTable = .;

		LONG(Data_Start_ROM)
		LONG(Data_Start_RAM)
		LONG(Data_Size)

		LONG(SRAMZ_Start)
		LONG(SRAMZ_Start)
		LONG(SRAMZ_Size)

		RAMInitEnd = .;

	} > ROM

/*
	.ARM.extab : {
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > ROM

	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > ROM
*/

/*
	.bss : {
		. = ALIGN(4);
		BSS_Start = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		BSS_End = .;
	} > RAM

	BSS_Size = BSS_End - BSS_Start;
*/

/*	.stack : {	*/
		/* Stack must be 8-byte aligned for double local vars */
/*		. = ALIGN(8);
		*(.stack)
	} > RAM				*/

	/* Stack top to end of RAM */
/*	Stack_Top = ORIGIN(RAM) + LENGTH(RAM);
	Stack_Limit = Stack_Top - SIZEOF(.stack);	*/
}
