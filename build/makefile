
################ Top configuration.
PRJ_TOP = ..
OBJDIR = out
TARGET = $(OBJDIR)/exec

MCU = rv32imac_zicsr
#MCU = rv32imac_zicsr_zba_zbb_zbc_zbs  # B extension isn't support.
#MCU =rv64imc

BUILD_STRICT = TRUE
BUILD_PRINT = FALSE

OPTI = -O1
VER_STRING = $$(date_time)_$(OPTI)

LINK_SCRIPT = startup/link.ld

################  Define
DEFINE = \
	__riscv__

#WCH_INT

################  Library directory.
PRJ_LIB_DIR = driver

EXT_LIB_DIR = 

LIB_FILE = ISP573 c_nano

################  Include.
# Add relative path from $(PRJ_TOP)
PRJ_INC = . \
		build \
		com \
		rvmsis \
		driver \
		src

# Add absolue path. (ex. c:/lib/inc)
EXT_INC =

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
		driver/CH57x_spi0.c

#		src/flash_spi.c \

CPPSRC =

ASRC =	startup/startup_CH573.S

DATA =

include rules.mk

test: $(TARGET).elf
	$(ECHO)$(NM) -n $< > aa.sym
	$(ECHO)$(NM) -S -l -n --format=sysv $< > bb.sym

