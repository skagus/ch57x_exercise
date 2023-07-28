
################ Top configuration.
PRJ_TOP = ..
TARGET = exec
OBJDIR = obj

MCU = rv32imac
#MCU =rv64imc

BUILD_STRICT = TRUE
BUILD_PRINT = FALSE

ifeq ($(BUILD_PRINT), TRUE)
ECHO = 
else
ECHO = @
endif

VER_FILE = version.c
################ source files ##############
# Source file들은 project TOP 에서의 위치를 나타낸다.
CSRC =	\
		src/hal.c \
		src/overlay.c \
		src/crc16.c \
		src/mcu_dbg.c \
		src/os.c \
		src/led.c \
		src/cli.c \
		src/dot_mat.c \
		src/ymodem_os.c \
		src/main.c \
		com/util.c \
		rvmsis/core_riscv.c \
		driver/CH57x_gpio.c \
		driver/CH57x_uart0.c \
		driver/CH57x_uart1.c \
		driver/CH57x_timer0.c \
		driver/CH57x_sys.c \
		driver/CH57x_spi0.c \
		$(VER_FILE)

#		src/flash_spi.c \

CPPSRC =

ASRC =	startup/startup_CH573.S

DATA =

LINK_SCRIPT = startup/link.ld

################ Object ##############
OBJ =	$(CSRC:%.c=$(OBJDIR)/%.c.o) \
		$(CPPSRC:%.cpp=$(OBJDIR)/%.p.o) \
		$(ASRC:%.S=$(OBJDIR)/%.s.o) \
		$(DATA:%.bin=$(OBJDIR)/%.b.o)

LST =	$(OBJ:%.o=%.lst)
DEP =	$(OBJ:%.o=%.d)

################  Define
DEFINE =
#DEFINE += -DWCH_INT
DEFINE += -D__riscv__

################  Library directory.
PRJ_LIB_DIR = driver

EXT_LIB_DIR = 

LIB_FILE = ISP573 c_nano

LIB_OPT =	$(patsubst %,-L$(PRJ_TOP)/%,$(PRJ_LIB_DIR)) \
			$(patsubst %,-L%,$(EXT_LIB_DIR)) \
			$(patsubst %,-l%,$(LIB_FILE))

################  Include.
# Add relative path from $(PRJ_TOP)
PRJ_INC = build \
		com \
		rvmsis \
		driver \
		src

# Add absolue path. (ex. c:/lib/inc)
EXT_INC =

INCLUDE =	$(patsubst %,-I$(PRJ_TOP)/%,$(PRJ_INC)) \
			$(patsubst %,-I%,$(EXT_INC))

################  Common Options for C, C++, ASM
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
FLAGS  = -march=$(MCU)
ifeq ($(findstring rv32,$(MCU)),rv32)
FLAGS += -mabi=ilp32
else
FLAGS += -mabi=lp64
endif
FLAGS += -mcmodel=medany
FLAGS += -msmall-data-limit=8
FLAGS += -std=gnu99 # c89, c99, gnu89, gnu99

FLAGS += -funsigned-char
FLAGS += -funsigned-bitfields
FLAGS += -fshort-enums
#FLAGS += -fpack-struct  # CAUTION: RISC-V can't handle un-aligned load/save.
FLAGS += -fmessage-length=0

FLAGS += -Os
#FLAGS += -msave-restore 
FLAGS += -ffunction-sections
FLAGS += -fdata-sections 
#FLAGS += -fno-unit-at-a-time
#FLAGS += -mshort-calls
FLAGS += -g
FLAGS += -Wa,-adhlns=$(@:.o=.lst)
FLAGS += -gdwarf-2
FLAGS += -Wno-comment

ifeq ($(BUILD_STRICT), TRUE)
FLAGS += -Wall
FLAGS += -Wunused
FLAGS += -Wextra
FLAGS += -Wstrict-prototypes
#CFLAGS += -Wundef # for undefined macro evaluation
FLAGS += -Wunreachable-code
FLAGS += -Wno-attributes # 
endif

FLAGS += -MMD -MP # dependancy generation.

################  C Options
CFLAGS  = $(FLAGS)
CFLAGS += $(DEP_OPT)
CFLAGS += $(DEFINE)
CFLAGS += $(INCLUDE)
# CFLAGS += -fstack-usage  # show stack usage for each function.
CFLAGS += -fcallgraph-info  # make call graph information.
#CFLAGS += -fdump-tree-optimized #
#CFLAGS += -fdump-rtl-dfinish  # 
################  C++ Options
CPPFLAGS  = $(FLAGS)
CPPFLAGS += $(DEP_OPT)
CPPFLAGS += $(DEFINE)
CPPFLAGS += $(INCLUDE)

################  Assembler Options
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
#  -listing-cont-lines: Sets the maximum number of continuation lines of hex 
#       dump that will be displayed for a given single line of source input.
ASFLAGS  = $(FLAGS)
#ASFLAGS += -Wa,-gstabs,--listing-cont-lines=100
ASFLAGS += $(DEP_OPT)
ASFLAGS += $(DEFINE)
ASFLAGS += $(INCLUDE)

################  Linker Options
# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDS_OPT = $(patsubst %, -T $(PRJ_TOP)/%, $(LINK_SCRIPT))

LDFLAGS = -g
LDFLAGS += -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(LDS_OPT)
#LDFLAGS += -fstack-usage
LDFLAGS += -nostartfiles
#LDFLAGS += -nodefaultlibs
#LDFLAGS += -nolibc
#LDFLAGS += -nostdlib
LDFLAGS += -Xlinker --gc-sections
LDFLAGS += -Xlinker --print-memory-usage
LDFLAGS += -march=$(MCU)
LDFLAGS += --specs=nosys.specs
#LDFLAGS += --specs=nano.specs
#LDFLAGS += -mcmodel=medany
#LDFLAGS += -msmall-data-limit=8
LDFLAGS += -fmessage-length=0
LDFLAGS += -ffunction-sections
LDFLAGS += -fdata-sections
#LDFLAGS += -fsigned-char
#LDFLAGS += -fno-common
ifeq ($(MCU),rv32imac)
LDFLAGS += -mabi=ilp32
else
LDFLAGS += -mabi=lp64
endif
################### Action #################
# add @ for silent.
GNU_PRIFIX = riscv-none-embed-

CC 		= $(GNU_PRIFIX)gcc
OBJCOPY	= $(GNU_PRIFIX)objcopy
OBJDUMP	= $(GNU_PRIFIX)objdump
SIZE	= $(GNU_PRIFIX)size
AR		= $(GNU_PRIFIX)ar rcs
NM		= $(GNU_PRIFIX)nm

SHELL = sh
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp

INFO 	= @echo Making: $@
DIR_CHK = @mkdir -p $(@D)

################ Object file
$(OBJDIR)/%.b.o : $(PRJ_TOP)/%.bin
	$(INFO)
	$(DIR_CHK)
	$(OBJCOPY) -I binary -O elf32 $< $@

$(OBJDIR)/%.c.o : $(PRJ_TOP)/%.c
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(CC) -c $(CFLAGS) $< -o $@ 

$(OBJDIR)/%.p.o : $(PRJ_TOP)/%.cpp
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(CC) -c $(CPPFLAGS) $< -o $@ 

$(OBJDIR)/%.s.o : $(PRJ_TOP)/%.S
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(CC) -c $(ASFLAGS) $< -o $@

################### File Creation #################
all: elf bin lst hex size

$(PRJ_TOP)/$(VER_FILE):
	$(INFO)
	@echo "char* gpVersion = \"$$(date +%Y%m%d_%H%M%S)\";" > $@

%.elf: $(OBJ)
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(CC) $(LDFLAGS) $^ $(LIB_OPT) -o $@ 

%.hex: %.elf
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(OBJCOPY) -O ihex  $< $@

%.bin: %.elf
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(OBJCOPY) -O binary $< $@

%.lss: %.elf
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(OBJDUMP) -h -S -z $< > $@

%.sym: %.elf
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(NM) -n $< > $@

%.a: $(OBJ)
	$(INFO)
	$(DIR_CHK)
	$(ECHO)$(AR) $@ $^

################ Actions.
TARGET_ALL=	$(TARGET).elf $(TARGET).a \
			$(TARGET).hex $(TARGET).bin \
			$(TARGET).lss $(TARGET).sym $(TARGET).map

version:
	@echo "char* gpVersion = \"$$(date +%Y%m%d_%H%M%S)\";" > $(PRJ_TOP)/$(VER_FILE)

obj: $(OBJ)
lib: $(TARGET).a
elf: $(TARGET).elf
bin: $(TARGET).bin $(TARGET).hex
lst: $(TARGET).lss $(TARGET).sym

size: $(TARGET).elf
	$(INFO)
	@$(SIZE) --format=gnu $<
	@$(SIZE) --format=sysv --radix=16 --common $<

# Display compiler version information.
gccversion : 
	$(ECHO)$(CC) --version

clean:
	$(REMOVE) $(TARGET).elf $(TARGET).a \
			$(TARGET).hex $(TARGET).bin \
			$(TARGET).lss $(TARGET).sym \
			$(TARGET).map
	$(REMOVEDIR) $(OBJDIR)

# Include the dependency files.
-include $(patsubst %.c,$(OBJDIR)/%.c.d,$(CSRC))
-include $(patsubst %.cpp,$(OBJDIR)/%.p.d,$(CPPSRC))
-include $(patsubst %.S,$(OBJDIR)/%.S.d,$(ASRC))

# Listing of phony targets.
.PHONY : all obj elf bin hex lst size gccversion clean version
