TARGET := myevic

# We make the following assumptions on Windows:
# arm-none-eabi gcc and binutils are compiled for Windows,
# so if you are using Cygwin, we will need path translations
# NUVOSDK must be lazily evaluated, so that we can later
# change EVICSDK when building include paths.

# Small fix to bug where cygpath -w mistranslates paths with mixed slashes (/, \)
EVICSDK := $(subst \,/,$(EVICSDK))
NUVOSDK = $(EVICSDK)/nuvoton-sdk/Library

OBJS := $(NUVOSDK)/Device/Nuvoton/M451Series/Source/system_M451Series.o \
		$(NUVOSDK)/StdDriver/src/clk.o \
		$(NUVOSDK)/StdDriver/src/fmc.o \
		$(NUVOSDK)/StdDriver/src/gpio.o \
		$(NUVOSDK)/StdDriver/src/spi.o \
		$(NUVOSDK)/StdDriver/src/sys.o \
		$(NUVOSDK)/StdDriver/src/timer.o \
		$(NUVOSDK)/StdDriver/src/rtc.o \
		$(NUVOSDK)/StdDriver/src/usbd.o \
		$(NUVOSDK)/StdDriver/src/eadc.o \
		$(NUVOSDK)/StdDriver/src/wdt.o \
		$(NUVOSDK)/StdDriver/src/uart.o \
		$(NUVOSDK)/StdDriver/src/crc.o \
		$(NUVOSDK)/StdDriver/src/pwm.o

MYEVIC_OBJS :=	src/myevic.o \
				src/myprintf.o \
				src/atomizer.o \
				src/dataflash.o \
				src/screens.o \
				src/menus.o \
				src/mainview.o \
				src/battery.o \
				src/events.o \
				src/myrtc.o \
				src/miscs.o \
				src/eh.o \
				src/timers.o \
				src/meadc.o \
				src/megpio.o \
				src/strings.o \
				src/meusbd.o \
				src/vcom.o \
				src/storage.o \
				src/flappy.o \
				src/fbdata.o \
				src/display.o \
				src/SSD1306.o \
				src/SSD1327.o \
				src/main.o

AEABI_OBJS := src/aeabi/aeabi_memset-thumb2.o \
			  src/aeabi/aeabi_memclr.o

OUTDIR := bin
DOCDIR := doc

CPU := cortex-m4
FPU := fpv4-sp-d16

# We need to find out if on cygwin or not
ifeq ($(OS),Windows_NT)
	ifeq (, $(findstring cygwin, $(shell gcc -dumpmachine)))
		WIN_CYG := 0
	else
		WIN_CYG := 1
	endif
	
endif

ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"), 1)
	CC_IS_CLANG := 1
endif

ifeq ($(ARMGCC),)
	ARMGCC := $(shell cd $(shell arm-none-eabi-gcc --print-search-dir | grep 'libraries' | \
		tr '=$(if $(filter Windows_NT,$(OS)),;,:)' '\n' | \
		grep -E '/arm-none-eabi/lib/?$$' | head -1)/../.. && pwd)
endif

ifeq ($(OS),Windows_NT)
	# Always fix binutils path
	ifneq ($(ARMGCC),)
		# If using cygwin, use cygpath
		ifeq ($(WIN_CYG),1)
			ARMGCC := $(shell cygpath -w $(ARMGCC))
		endif
		
	endif
	
	ifndef CC_IS_CLANG
		NEED_FIXPATH := 1
	endif
endif

ifneq ($(ARMGCC),)
	ifdef CC_IS_CLANG
		CFLAGS += -target armv7em-none-eabi -fshort-enums

		AEABI_COUNT := $(shell arm-none-eabi-nm -g $(ARMGCC)/arm-none-eabi/lib/armv7e-m/libc.a | grep -Ec 'T __aeabi_mem(set|clr)[48]?$$')
		ifeq ($(AEABI_COUNT), 0)
			# __aeabi_memset* and __aeabi_memclr* are not exported by libc
			# We provide our own implementations
			OBJS += $(AEABI_OBJS)
		else ifneq ($(AEABI_COUNT), 6)
			# Only part of __aeabi_memset* and __aeabi_memclr* are exported by libc
			# This should never happen, bail out in env_check
			AEABI_ERROR := 1
		endif
	else
		CC := arm-none-eabi-gcc
	endif
	
	ifdef NEED_FIXPATH
		ifeq ($(WIN_CYG), 0)
			OBJS_FIXPATH := $(OBJS)
		else
			OBJS_FIXPATH := $(shell cygpath -w $(OBJS))
			EVICSDK := $(shell cygpath -w $(EVICSDK))
		endif
	else
		OBJS_FIXPATH := $(OBJS)
	endif
endif

ifeq ($(WIN_CYG),0)
	SDKTAG := $(shell git describe --abbrev --dirty --always --tags 2> NUL ) # Fix for Windows w/o cygwin (NUL instead of /dev/null)
else
	SDKTAG := $(shell git describe --abbrev --dirty --always --tags 2> /dev/null ) 
endif
ifeq ($(SDKTAG),)
	SDKTAG := unknown
endif

AS := arm-none-eabi-as
LD := arm-none-eabi-ld
AR := arm-none-eabi-ar
OBJCOPY := arm-none-eabi-objcopy

INCDIRS := -I$(ARMGCC)/arm-none-eabi/include

INCDIRS += $(foreach d,$(shell arm-none-eabi-gcc -x c -v -E /dev/null 2>&1 | sed -n -e '/<\.\.\.>/,/End/ p' | tail -n +2 | head -n -1 | sed 's/^\s*//'),-I$d) \
			-I$(NUVOSDK)/CMSIS/Include \
			-I$(NUVOSDK)/Device/Nuvoton/M451Series/Include \
			-I$(NUVOSDK)/StdDriver/inc \
			-Iinc \
			-Isrc

GCC_VERSION := 5.3.1

LIBDIRS :=	-L$(ARMGCC)/arm-none-eabi/lib \
			-L$(ARMGCC)/arm-none-eabi/newlib \
			-L$(ARMGCC)/lib/arm-none-eabi/newlib \
			-L$(ARMGCC)/gcc/arm-none-eabi/$(GCC_VERSION) \
			-L$(ARMGCC)/lib/gcc/arm-none-eabi/$(GCC_VERSION)

#LIBS := -lgcc

CFLAGS += -Wall -mcpu=$(CPU) -mfpu=$(FPU) -mthumb -Os -fdata-sections -ffunction-sections
CFLAGS += -fno-builtin-printf
CFLAGS += $(INCDIRS)

ASFLAGS := -mcpu=$(CPU) -mfpu=$(FPU)
ASFLAGS += $(INCDIRS)

LINKSCRIPT := linker.ld

LDFLAGS += -u __aeabi_uldivmod
LDFLAGS += $(LIBDIRS)
LDFLAGS += -gc-sections -nostdlib -nostartfiles -T$(LINKSCRIPT)
#LDFLAGS += --verbose --cref -Map=blah.map

SRCDIR   = src
INCDIR   = inc
OBJDIR   = src
BINDIR   = bin

INCLUDEH := $(wildcard $(INCDIR)/*.h)
INCLUDES := $(wildcard $(SRCDIR)/*.s)

all: env_check $(TARGET).bin

$(OBJS_FIXPATH) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.c $(INCLUDEH)
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.s $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

#$(TARGET).elf: $(OBJS_FIXPATH)
#	test -d $(OUTDIR) || mkdir $(OUTDIR)
#	$(LD) $(LDFLAGS) -o $(OUTDIR)/$(TARGET).elf $(OBJS_FIXPATH)

$(TARGET).bin: $(OBJS_FIXPATH) $(MYEVIC_OBJS)
	test -d $(OUTDIR) || mkdir $(OUTDIR)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/$(TARGET).elf --start-group $(LIBS) $(OBJS_FIXPATH) $(MYEVIC_OBJS) --end-group
	$(OBJCOPY) -O binary -j .text -j .data $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).bin
	evic convert $(OUTDIR)/$(TARGET).bin -o $(OUTDIR)/$(TARGET)_enc.bin

docs:
	doxygen

clean:
	rm -rf $(OBJS) $(AEABI_OBJS) $(OUTDIR)/$(TARGET).bin $(OUTDIR) $(DOCDIR)

env_check:
ifeq ($(ARMGCC),)
	$(error You must set the ARMGCC environment variable)
endif
ifneq ($(AEABI_ERROR),)
	$(error Your libc is exporting only part of __aeabi symbols)
endif

.PHONY: all clean docs env_check gen_tag
