/* Memory regions */
MEMORY {
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH =  32K
}

/* Library configurations */
/*
GROUP(
	armv7e-m/libgcc.a
	armv7e-m/libc.a
	armv7e-m/libm.a
)
*/

ENTRY(Reset_Handler)

SECTIONS {
	.text : {
		KEEP(*(.isr_vectors))
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
	} > ROM

	.sram : {
		*(.sram*)
	} > RAM

/*
	.ARM.extab : {
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > ROM

	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > ROM
*/

/*
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
