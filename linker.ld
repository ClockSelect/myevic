/* Memory regions */
MEMORY {
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 100K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH =  32K
}

/* Library configurations */
GROUP(
/*	armv7e-m/libnosys.a
	armv7e-m/libgcc.a
	armv7e-m/libc.a
	armv7e-m/libm.a		*/
	v7e-m/libnosys.a
	v7e-m/libgcc.a
	v7e-m/libc.a
	v7e-m/libm.a
)

ENTRY(Reset_Handler)

SECTIONS {
	.text : {
		KEEP(*(.isr_vectors))

		LONG(0x5746594D)
		LONG(images1_start)
		LONG(images1_end)
		LONG(images2_start)
		LONG(images2_end)
		LONG(Strings_Start)
		LONG(Strings_End)

		. = ALIGN(4);
		*(.text*)

		. = ALIGN(4);
		KEEP(*(.myevic*))

		. = ALIGN(4);
		KEEP(*(.productid*))

		. = ALIGN(4);
		*(.rodata*)

		. = ALIGN(4);
		Strings_Start = .;
		*(.strings*)
		Strings_End = .;

		. = ALIGN(4);
		images1_start = .;
		*(.imgblock1)
		images1_end = .;

		images2_start = .;
		*(.imgblock2)
		images2_end = .;

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
}
