/* Memory regions */
MEMORY {
	ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
	RAM (rwx) : ORIGIN = 0x20000000, LENGTH =  32K
}

/* Library configurations */
/* INPUT(libevicsdk_crt0.o) */
GROUP(
	armv7e-m/libgcc.a
	armv7e-m/libc.a
	armv7e-m/libm.a
/*	libevicsdk.a	*/
)

ENTRY(Reset_Handler)

SECTIONS {
	.text : {
		KEEP(*(.isr_vector))
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP(*(.preinit_array))
		__preinit_array_end = .;
		__init_array_start = .;
		KEEP(*(.init_array))
		__init_array_end = .;
		__fini_array_start = .;
		KEEP(*(.fini_array))
		__fini_array_end = .;
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(SORT(.fini_array.*)))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		KEEP(*(.evicsdk_tag))
		KEEP(*(.verify))

		. = ALIGN(4);
		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > ROM

	.ARM.extab : {
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > ROM

	.ARM.exidx : {
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
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

	.bss : {
		. = ALIGN(4);
		BSS_Start = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		BSS_End = .;
	} > RAM

	BSS_Size = BSS_End - BSS_Start;

	.stack : {
		/* Stack must be 8-byte aligned for double local vars */
		. = ALIGN(8);
		*(.stack)
	} > RAM

	/* Stack top to end of RAM */
	Stack_Top = ORIGIN(RAM) + LENGTH(RAM);
	Stack_Limit = Stack_Top - SIZEOF(.stack);

	/* Fill all unused RAM with heap */
	Heap_Start = BSS_End;
	Heap_Limit = Stack_Limit;

	/* Check for ROM overflow */
	/* 8kB OFW dataflash, 16kB custom dataflash */
	ASSERT(Data_Start_ROM + Data_Size <= ORIGIN(ROM) + LENGTH(ROM) - 24K, "ROM too big")

	/* Check for RAM overflow */
	ASSERT(Stack_Limit > Heap_Start, "RAM overflowed with stack")
}
