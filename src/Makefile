CC = arm-elf-gcc

DEFINE = -DINTERVAL_MAJ=5 -DINTERVAL_TENDANCES=10
MY_LINUX_PATH = /home/ana-maria/Documente/projet_archi/uClinux-pragm-20070119-modified
INC_NANOX=$(MY_LINUX_PATH)/user/microwin/src/include
INC_GRAPHICS=/usr/local/arm-linux/sys-include/config/ep93xx
INCLUDE=-I$(INC_NANOX) -I$(INC_GRAPHICS)

LIB_PATH_NANOX=$(MY_LINUX_PATH)/user/microwin/src/lib
LIBS_PATH=-L$(LIB_PATH_NANOX)

LIB_NANOX = nano-X
LIB_THREAD = pthread
LIBS = -l$(LIB_NANOX) -l$(LIB_THREAD)

SRC_WR = main.c capteurs.c boutons.c menus.c threads.c
OBJ_WR = main.o capteurs.o boutons.o menus.o threads.o
EXEC   = final_meteo1

all :
	$(CC) $(DEFINE) -c $(SRC_WR) $(INCLUDE)
	$(CC) -Wl, -elf2flt $(DEFINE) $(OBJ_WR) -o $(EXEC) $(LIBS_PATH) $(LIBS) -lm
	rm -f *.o *.gdb
clean :
	rm -f *.o *.gdb
