CC = arm-elf-gcc

DISTRIB_PATH = /home/thomas-gattaz/Documents/projet_archi/uClinux-pragm-20070119-modified
INC_NANOX = $(DISTRIB_PATH)/user/microwin/src/include
INC_GRAPHICS = /usr/local/arm-linux/sys-include/config/ep93xx
INCLUDE = -I$(INC_NANOX) -I$(INC_GRAPHICS)

LIB_PATH_NANOX = $(DISTRIB_PATH)/user/microwin/src/lib
LIBS_PATH = -L$(LIB_PATH_NANOX)

LIB_NANOX = nano-X
LIB_THREAD = pthread
LIBS = -l$(LIB_NANOX) -l$(LIB_THREAD)

SRC_WR = main.c capteurs.c affichage.c boutons.c
OBJ_WR = main.o capteurs.o affichage.o boutons.o
EXEC   = test_Ana

all :
	$(CC) -c $(SRC_WR) $(INCLUDE)
	$(CC) -Wl, -elf2flt $(OBJ_WR) -o $(EXEC) $(LIBS_PATH) $(LIBS)

clean :
	rm -f *.o *.gdb *.gch $(EXEC)

remake : clean all
