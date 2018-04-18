CC = arm-elf-gcc

DISTRIB_PATH = /home/airdur/Documents/projet_archi/uClinux-pragm-20070119-modified
INC_NANOX = $(DISTRIB_PATH)/user/microwin/src/include
INC_GRAPHICS = /usr/local/arm-linux/sys-include/config/ep93xx
INCLUDE = -I$(INC_NANOX) -I$(INC_GRAPHICS)

LIB_PATH_NANOX = $(DISTRIB_PATH)/user/microwin/src/lib
LIBS_PATH = -L$(LIB_PATH_NANOX)

LIB_NANOX = nano-X
LIBS = -l$(LIB_NANOX)

SRC_WR = testButton.c
OBJ_WR = testButton.o
EXEC   = testButton


all :
	$(CC) -c $(SRC_WR) -o $(OBJ_WR) $(INCLUDE)
	$(CC) -Wl, -elf2flt $(OBJ_WR) -o $(EXEC) $(LIBS_PATH) $(LIBS)

clean :
	rm -f *.o *.gdb $(EXEC)
