TARGET	= ipac

# Application source en include includes
SRC_DIR	= ./source
INC_DIR = ./include
BUILD_DIR = ./buildfiles

# NutOS location (includes and libs)
NUT_INC = c:/ethernut-4.3.3/nut/include
NUT_BUILD_INC = c:/ethernut-4.3.3/build/gcc/atmega2561/lib/include
NUT_LIB_DIR = c:/ethernut-4.3.3/build/gcc/atmega2561/lib

# WinAvr includes
AVR_INC = c:/winavr/avr/include


# Compiler, assembler & linker (flags)
CC		= 	avr-gcc
CFLAGS	= 	-mmcu=atmega2561 -Os -Wall -Wstrict-prototypes -DNUT_CPU_FREQ=14745600 \
			-D__HARVARD_ARCH__ -DNUTOS_VERSION=433 \
			-Wa,-ahlms=$(BUILD_DIR)/$*lst
ASFLAGS = 	-mmcu=atmega2561 -I. -x assembler-with-cpp -Wa,-ahlms=$(BUILD_DIR)/$*lst,-gstabs 
LDFLAGS	=	-mmcu=atmega2561 -Wl,--defsym=main=0,-Map=TIStreamer.map,--cref


# =================================================================================
# Source files
CFILES =        main.c			\
				uart0driver.c	\
				log.c			\
                led.c			\
				keyboard.c		\
				display.c		\
                vs10xx.c		\
                remcon.c		\
                watchdog.c		\
				mmc.c			\
				spidrv.c        \
                mmcdrv.c        \
                fat.c			\
				flash.c			\
				rtc.c			\
				keyboardhandler.c \
				displayhandler.c \
				list.c			\
				state_machine.c \
				main_menu.c		\
				rss.c 			\
				idle.c \
				menu_alarm.c \
				menu_stream.c \
				menu_rss.c \
				alarmmenu.c \
				special.c \
				daily_alarm.c 	\
				customcharhandler.c		\
				stream.c	\
				audio.c	\
				week_alarm.c	\
				internet_connector.c 	\
				schedular.c \
				normal_alarm.c \
				weekly_alarm.c \
				special_alarm.c \
				x1205.c			\
				rtchandler.c	\
				stream_play.c \
				memoryhandler.c	\
				uxmlstream.c \
				uxmltree.c \
				uxmlparse.c \
				memorygame.c	\

				
# Header files.
HFILES =        display.h        keyboard.h              \
                led.h                            \
                portio.h         remcon.h         log.h          \
                system.h                 settings.h     \
                                  inet.h         \
                platform.h       version.h        update.h       \
                           uart0driver.h    typedefs.h     \
                       vs10xx.h         audio.h        \
                watchdog.h       mmc.h             \
                flash.h          spidrv.h         command.h      \
                parse.h          mmcdrv.h         fat.h          \
                fatdrv.h         flash.h	  	rtc.h			\
				keyboardhandler.h  displayhandler.h \
				list.h 	main_menu.h  state_machine.h \
				rss.h	\
				inet.h \
				internet_connector.h \
				idle.h	\
				menu_alarm.h \
				menu_stream.h \
				menu_rss.h \
				alarmmenu.h \
				special.h \
				keyboardhandler.h displayhandler.h \
				list.h \	main_menu.h \ state_machine.h	\
				alarmmenu.h		\
				customcharhandler.h		\
				stream.h	\
				keyboardhandler.h displayhandler.h \
				list.h \	main_menu.h \ state_machine.h daily_alarm.h\
				audio.h\
				week_alarm.h	\
				x1205.h			\
				rtchandler.h	\
				structs.h \
				schedular.h \
				normal_alarm.h \
				weekly_alarm.h \
				special_alarm.h \
				stream_play.h	\
				memoryhandler.h	\
				uxml.h\
				memdebug.h\
				memorygame.h	\
				

# Alle source files in de ./source dir
SRCS =	$(addprefix $(SRC_DIR)/,$(CFILES))
OBJS = 	$(addprefix $(BUILD_DIR)/,$(CFILES:.c=.o))

NUT_LIBS = $(NUT_LIB_DIR)/nutinit.o -lnutpro -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutarch -lnutnet -lnutcrt -lnutdev


# Alle includes (header files) in de ./header dir
INCS =	$(addprefix $(INC_DIR)/,$(HFILES))

# Linking rule. All *.o to elf file. Then convert to *.hex
$(TARGET):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -L$(NUT_LIB_DIR) $(NUT_LIBS) -o $@.elf
	avr-objcopy -O ihex $@.elf $@.hex
#	hex2bin -ebin $@.hex

# Compiling the objs's. avr-gcc autocalls assembler	
$(BUILD_DIR)/%o:	$(SRC_DIR)/%c 
	$(CC) -c $< $(CFLAGS) -I$(INC_DIR) -I$(NUT_INC) -I$(AVR_INC) -o $@

	
all: $(TARGET)

debug:
	
	@echo $(OBJS)


.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(OBJS:.o=.lst)
	-rm -f *.hex *.elf *.map *.bin


