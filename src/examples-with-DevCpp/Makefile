# Project: Station_Meteo
# Makefile created by hand

CPP  = arm-elf-g++
CC   = arm-elf-gcc
#WINDRES = windres.exe
#RES  = 
OBJ  = obj/api_flash.o obj/api_iic.o obj/api_iis.o obj/api_keyboard.o obj/api_lcd.o obj/api_led.o obj/api_serial.o obj/44blib.o obj/uhal.o obj/api_timers.o obj/api_rtc.o obj/utils.o obj/demo.o obj/trackbar.o obj/button.o obj/combobox.o obj/ctrlmisc.o obj/edit.o obj/listbox.o obj/listmodel.o obj/medit.o obj/menubutton.o obj/newtoolbar.o obj/progressbar.o obj/propsheet.o obj/scrolled.o obj/scrollview.o obj/scrollwnd.o obj/simedit.o obj/static.o obj/textedit.o obj/toolbar.o obj/charset.o obj/devfont.o obj/fontname.o obj/rawbitmap.o obj/sysfont.o obj/varbitmap.o obj/gal.o obj/scr_fb.o obj/fb.o obj/fblin1.o obj/fblin2.o obj/fblin4.o obj/fblin8.o obj/fblin16.o obj/fblin32.o obj/fblin-1.o obj/fblin-2.o obj/fblin-4.o obj/genfunc.o obj/genfunc_clipline.o obj/native.o obj/qvfb.o obj/text.o obj/attr.o obj/bitmap.o obj/clip.o obj/cliprect.o obj/coor.o obj/draw.o obj/drawtext.o obj/gdi.o obj/icon.o obj/logfont.o obj/map.o obj/palette.o obj/readbmp.o obj/rect.o obj/screen.o obj/window.o obj/accelkey.o obj/caret.o obj/ctrlclass.o obj/defkeymap.o obj/de-kmap.o obj/de-latin1-kmap.o obj/de-latin1-nodeadkeys-kmap.o obj/dialog.o obj/element.o obj/es-cp850-kmap.o obj/es-kmap.o obj/fr-kmap.o obj/fr-pc-kmap.o obj/it-kmap.o obj/keyboard.o obj/menu.o obj/systext.o obj/about.o obj/clipboard.o obj/endianrw.o obj/math3d.o obj/math.o obj/nposix.o obj/rwops.o obj/winbmp.o obj/gif.o obj/jpeg.o obj/mybmp.o obj/png.o obj/syscfg.o obj/resource.o obj/init-lite.o obj/blockheap.o obj/event.o obj/fixstr.o obj/message.o obj/timer-lite.o obj/misc.o obj/desktop-lite.o obj/cursor-sa.o obj/ial.o obj/dummy.o obj/standalone.o $(RES)
LINKOBJ  = obj/api_flash.o obj/api_iic.o obj/api_iis.o obj/api_keyboard.o obj/api_lcd.o obj/api_led.o obj/api_serial.o obj/44blib.o obj/uhal.o obj/api_timers.o obj/api_rtc.o obj/utils.o obj/demo.o obj/trackbar.o obj/button.o obj/combobox.o obj/ctrlmisc.o obj/edit.o obj/listbox.o obj/listmodel.o obj/medit.o obj/menubutton.o obj/newtoolbar.o obj/progressbar.o obj/propsheet.o obj/scrolled.o obj/scrollview.o obj/scrollwnd.o obj/simedit.o obj/static.o obj/textedit.o obj/toolbar.o obj/charset.o obj/devfont.o obj/fontname.o obj/rawbitmap.o obj/sysfont.o obj/varbitmap.o obj/gal.o obj/scr_fb.o obj/fb.o obj/fblin1.o obj/fblin2.o obj/fblin4.o obj/fblin8.o obj/fblin16.o obj/fblin32.o obj/fblin-1.o obj/fblin-2.o obj/fblin-4.o obj/genfunc.o obj/genfunc_clipline.o obj/native.o obj/qvfb.o obj/text.o obj/attr.o obj/bitmap.o obj/clip.o obj/cliprect.o obj/coor.o obj/draw.o obj/drawtext.o obj/gdi.o obj/icon.o obj/logfont.o obj/map.o obj/palette.o obj/readbmp.o obj/rect.o obj/screen.o obj/window.o obj/accelkey.o obj/caret.o obj/ctrlclass.o obj/defkeymap.o obj/de-kmap.o obj/de-latin1-kmap.o obj/de-latin1-nodeadkeys-kmap.o obj/dialog.o obj/element.o obj/es-cp850-kmap.o obj/es-kmap.o obj/fr-kmap.o obj/fr-pc-kmap.o obj/it-kmap.o obj/keyboard.o obj/menu.o obj/systext.o obj/about.o obj/clipboard.o obj/endianrw.o obj/math3d.o obj/math.o obj/nposix.o obj/rwops.o obj/winbmp.o obj/gif.o obj/jpeg.o obj/mybmp.o obj/png.o obj/syscfg.o obj/resource.o obj/init-lite.o obj/blockheap.o obj/event.o obj/fixstr.o obj/message.o obj/timer-lite.o obj/misc.o obj/desktop-lite.o obj/cursor-sa.o obj/ial.o obj/dummy.o obj/standalone.o $(RES)
LIBS =  -L"/usr/local/arm-elf/lib" -Wl,-gc-sections -Ttarget.ld -nostdlib -lc -lgcc -lm  -nostdlib 
INCS =  -I"/usr/local/arm-elf/include" -I"./src/minigui/src/include" -I"./src/minigui/include" -I"./src/minigui/src/include/control" -I"./src/api" 
CXXINCS =  -I"/usr/local/arm-elf/include" -I"./src/minigui/src/include" -I"./src/minigui/include" -I"./src/minigui/src/include/control" -I"./src/api" 
BIN  = bin/Station_Meteo
CXXFLAGS = $(CXXINCS)   -nostdlib
CFLAGS = $(INCS) -mcpu=arm7tdmi -mlittle-endian -g -fno-builtin -DBOARD_FS44BOX -D_LITE_VERSION -DLCD_COLOR -D_STAND_ALONE -DDEMO_MINIGUI -nostdlib
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ./bin/Station_Meteo all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "./bin/Station_Meteo" $(LIBS)

#obj/44binit.o: ./src/init/44binit.s
#	arm-elf-as -mcpu=arm7tdmi -EL -mfpu=softfpa ./src/init/44binit.s -o ./src/init/44binit.o

#obj/os_cpu_a.o: ./src/init/os_cpu_a.s
#	arm-elf-as -mcpu=arm7tdmi -EL -mfpu=softfpa ./src/init/os_cpu_a.s -o ./src/init/os_cpu_a.o

obj/api_flash.o: ./src/api/api_flash.c
	$(CC) -c ./src/api/api_flash.c -o obj/api_flash.o $(CFLAGS)

obj/api_iic.o: ./src/api/api_iic.c
	$(CC) -c ./src/api/api_iic.c -o obj/api_iic.o $(CFLAGS)

obj/api_iis.o: ./src/api/api_iis.c
	$(CC) -c ./src/api/api_iis.c -o obj/api_iis.o $(CFLAGS)

obj/api_keyboard.o: ./src/api/api_keyboard.c
	$(CC) -c ./src/api/api_keyboard.c -o obj/api_keyboard.o $(CFLAGS)

obj/api_lcd.o: ./src/api/api_lcd.c
	$(CC) -c ./src/api/api_lcd.c -o obj/api_lcd.o $(CFLAGS)

obj/api_led.o: ./src/api/api_led.c
	$(CC) -c ./src/api/api_led.c -o obj/api_led.o $(CFLAGS)

obj/api_serial.o: ./src/api/api_serial.c
	$(CC) -c ./src/api/api_serial.c -o obj/api_serial.o $(CFLAGS)

obj/44blib.o: ./src/common/44blib.c
	$(CC) -c ./src/common/44blib.c -o obj/44blib.o $(CFLAGS)

obj/uhal.o: ./src/common/uhal.c
	$(CC) -c ./src/common/uhal.c -o obj/uhal.o $(CFLAGS)

obj/api_timers.o: ./src/api/api_timers.c
	$(CC) -c ./src/api/api_timers.c -o obj/api_timers.o $(CFLAGS)

obj/api_rtc.o: ./src/api/api_rtc.c
	$(CC) -c ./src/api/api_rtc.c -o obj/api_rtc.o $(CFLAGS)

obj/utils.o: ./src/common/utils.c
	$(CC) -c ./src/common/utils.c -o obj/utils.o $(CFLAGS)

obj/demo.o: ./src/demo/demo.c
	$(CC) -c ./src/demo/demo.c -o obj/demo.o $(CFLAGS)

obj/trackbar.o: ./src/minigui/src/control/trackbar.c
	$(CC) -c ./src/minigui/src/control/trackbar.c -o obj/trackbar.o $(CFLAGS)

obj/button.o: ./src/minigui/src/control/button.c
	$(CC) -c ./src/minigui/src/control/button.c -o obj/button.o $(CFLAGS)

obj/combobox.o: ./src/minigui/src/control/combobox.c
	$(CC) -c ./src/minigui/src/control/combobox.c -o obj/combobox.o $(CFLAGS)

obj/ctrlmisc.o: ./src/minigui/src/control/ctrlmisc.c
	$(CC) -c ./src/minigui/src/control/ctrlmisc.c -o obj/ctrlmisc.o $(CFLAGS)

obj/edit.o: ./src/minigui/src/control/edit.c
	$(CC) -c ./src/minigui/src/control/edit.c -o obj/edit.o $(CFLAGS)

obj/listbox.o: ./src/minigui/src/control/listbox.c
	$(CC) -c ./src/minigui/src/control/listbox.c -o obj/listbox.o $(CFLAGS)

obj/listmodel.o: ./src/minigui/src/control/listmodel.c
	$(CC) -c ./src/minigui/src/control/listmodel.c -o obj/listmodel.o $(CFLAGS)

obj/medit.o: ./src/minigui/src/control/medit.c
	$(CC) -c ./src/minigui/src/control/medit.c -o obj/medit.o $(CFLAGS)

obj/menubutton.o: ./src/minigui/src/control/menubutton.c
	$(CC) -c ./src/minigui/src/control/menubutton.c -o obj/menubutton.o $(CFLAGS)

obj/newtoolbar.o: ./src/minigui/src/control/newtoolbar.c
	$(CC) -c ./src/minigui/src/control/newtoolbar.c -o obj/newtoolbar.o $(CFLAGS)

obj/progressbar.o: ./src/minigui/src/control/progressbar.c
	$(CC) -c ./src/minigui/src/control/progressbar.c -o obj/progressbar.o $(CFLAGS)

obj/propsheet.o: ./src/minigui/src/control/propsheet.c
	$(CC) -c ./src/minigui/src/control/propsheet.c -o obj/propsheet.o $(CFLAGS)

obj/scrolled.o: ./src/minigui/src/control/scrolled.c
	$(CC) -c ./src/minigui/src/control/scrolled.c -o obj/scrolled.o $(CFLAGS)

obj/scrollview.o: ./src/minigui/src/control/scrollview.c
	$(CC) -c ./src/minigui/src/control/scrollview.c -o obj/scrollview.o $(CFLAGS)

obj/scrollwnd.o: ./src/minigui/src/control/scrollwnd.c
	$(CC) -c ./src/minigui/src/control/scrollwnd.c -o obj/scrollwnd.o $(CFLAGS)

obj/simedit.o: ./src/minigui/src/control/simedit.c
	$(CC) -c ./src/minigui/src/control/simedit.c -o obj/simedit.o $(CFLAGS)

obj/static.o: ./src/minigui/src/control/static.c
	$(CC) -c ./src/minigui/src/control/static.c -o obj/static.o $(CFLAGS)

obj/textedit.o: ./src/minigui/src/control/textedit.c
	$(CC) -c ./src/minigui/src/control/textedit.c -o obj/textedit.o $(CFLAGS)

obj/toolbar.o: ./src/minigui/src/control/toolbar.c
	$(CC) -c ./src/minigui/src/control/toolbar.c -o obj/toolbar.o $(CFLAGS)

obj/charset.o: ./src/minigui/src/font/charset.c
	$(CC) -c ./src/minigui/src/font/charset.c -o obj/charset.o $(CFLAGS)

obj/devfont.o: ./src/minigui/src/font/devfont.c
	$(CC) -c ./src/minigui/src/font/devfont.c -o obj/devfont.o $(CFLAGS)

obj/fontname.o: ./src/minigui/src/font/fontname.c
	$(CC) -c ./src/minigui/src/font/fontname.c -o obj/fontname.o $(CFLAGS)

obj/rawbitmap.o: ./src/minigui/src/font/rawbitmap.c
	$(CC) -c ./src/minigui/src/font/rawbitmap.c -o obj/rawbitmap.o $(CFLAGS)

obj/sysfont.o: ./src/minigui/src/font/sysfont.c
	$(CC) -c ./src/minigui/src/font/sysfont.c -o obj/sysfont.o $(CFLAGS)

obj/varbitmap.o: ./src/minigui/src/font/varbitmap.c
	$(CC) -c ./src/minigui/src/font/varbitmap.c -o obj/varbitmap.o $(CFLAGS)

obj/gal.o: ./src/minigui/src/gal/gal.c
	$(CC) -c ./src/minigui/src/gal/gal.c -o obj/gal.o $(CFLAGS)

obj/scr_fb.o: ./src/minigui/src/gal/native/scr_fb.c
	$(CC) -c ./src/minigui/src/gal/native/scr_fb.c -o obj/scr_fb.o $(CFLAGS)

obj/fb.o: ./src/minigui/src/gal/native/fb.c
	$(CC) -c ./src/minigui/src/gal/native/fb.c -o obj/fb.o $(CFLAGS)

obj/fblin1.o: ./src/minigui/src/gal/native/fblin1.c
	$(CC) -c ./src/minigui/src/gal/native/fblin1.c -o obj/fblin1.o $(CFLAGS)

obj/fblin2.o: ./src/minigui/src/gal/native/fblin2.c
	$(CC) -c ./src/minigui/src/gal/native/fblin2.c -o obj/fblin2.o $(CFLAGS)

obj/fblin4.o: ./src/minigui/src/gal/native/fblin4.c
	$(CC) -c ./src/minigui/src/gal/native/fblin4.c -o obj/fblin4.o $(CFLAGS)

obj/fblin8.o: ./src/minigui/src/gal/native/fblin8.c
	$(CC) -c ./src/minigui/src/gal/native/fblin8.c -o obj/fblin8.o $(CFLAGS)

obj/fblin16.o: ./src/minigui/src/gal/native/fblin16.c
	$(CC) -c ./src/minigui/src/gal/native/fblin16.c -o obj/fblin16.o $(CFLAGS)

obj/fblin32.o: ./src/minigui/src/gal/native/fblin32.c
	$(CC) -c ./src/minigui/src/gal/native/fblin32.c -o obj/fblin32.o $(CFLAGS)

obj/fblin-1.o: ./src/minigui/src/gal/native/fblin-1.c
	$(CC) -c ./src/minigui/src/gal/native/fblin-1.c -o obj/fblin-1.o $(CFLAGS)

obj/fblin-2.o: ./src/minigui/src/gal/native/fblin-2.c
	$(CC) -c ./src/minigui/src/gal/native/fblin-2.c -o obj/fblin-2.o $(CFLAGS)

obj/fblin-4.o: ./src/minigui/src/gal/native/fblin-4.c
	$(CC) -c ./src/minigui/src/gal/native/fblin-4.c -o obj/fblin-4.o $(CFLAGS)

obj/genfunc.o: ./src/minigui/src/gal/native/genfunc.c
	$(CC) -c ./src/minigui/src/gal/native/genfunc.c -o obj/genfunc.o $(CFLAGS)

obj/genfunc_clipline.o: ./src/minigui/src/gal/native/genfunc_clipline.c
	$(CC) -c ./src/minigui/src/gal/native/genfunc_clipline.c -o obj/genfunc_clipline.o $(CFLAGS)

obj/native.o: ./src/minigui/src/gal/native/native.c
	$(CC) -c ./src/minigui/src/gal/native/native.c -o obj/native.o $(CFLAGS)

obj/qvfb.o: ./src/minigui/src/gal/native/qvfb.c
	$(CC) -c ./src/minigui/src/gal/native/qvfb.c -o obj/qvfb.o $(CFLAGS)

obj/text.o: ./src/minigui/src/gdi/text.c
	$(CC) -c ./src/minigui/src/gdi/text.c -o obj/text.o $(CFLAGS)

obj/attr.o: ./src/minigui/src/gdi/attr.c
	$(CC) -c ./src/minigui/src/gdi/attr.c -o obj/attr.o $(CFLAGS)

obj/bitmap.o: ./src/minigui/src/gdi/bitmap.c
	$(CC) -c ./src/minigui/src/gdi/bitmap.c -o obj/bitmap.o $(CFLAGS)

obj/clip.o: ./src/minigui/src/gdi/clip.c
	$(CC) -c ./src/minigui/src/gdi/clip.c -o obj/clip.o $(CFLAGS)

obj/cliprect.o: ./src/minigui/src/gdi/cliprect.c
	$(CC) -c ./src/minigui/src/gdi/cliprect.c -o obj/cliprect.o $(CFLAGS)

obj/coor.o: ./src/minigui/src/gdi/coor.c
	$(CC) -c ./src/minigui/src/gdi/coor.c -o obj/coor.o $(CFLAGS)

obj/draw.o: ./src/minigui/src/gdi/draw.c
	$(CC) -c ./src/minigui/src/gdi/draw.c -o obj/draw.o $(CFLAGS)

obj/drawtext.o: ./src/minigui/src/gdi/drawtext.c
	$(CC) -c ./src/minigui/src/gdi/drawtext.c -o obj/drawtext.o $(CFLAGS)

obj/gdi.o: ./src/minigui/src/gdi/gdi.c
	$(CC) -c ./src/minigui/src/gdi/gdi.c -o obj/gdi.o $(CFLAGS)

obj/icon.o: ./src/minigui/src/gdi/icon.c
	$(CC) -c ./src/minigui/src/gdi/icon.c -o obj/icon.o $(CFLAGS)

obj/logfont.o: ./src/minigui/src/gdi/logfont.c
	$(CC) -c ./src/minigui/src/gdi/logfont.c -o obj/logfont.o $(CFLAGS)

obj/map.o: ./src/minigui/src/gdi/map.c
	$(CC) -c ./src/minigui/src/gdi/map.c -o obj/map.o $(CFLAGS)

obj/palette.o: ./src/minigui/src/gdi/palette.c
	$(CC) -c ./src/minigui/src/gdi/palette.c -o obj/palette.o $(CFLAGS)

obj/readbmp.o: ./src/minigui/src/gdi/readbmp.c
	$(CC) -c ./src/minigui/src/gdi/readbmp.c -o obj/readbmp.o $(CFLAGS)

obj/rect.o: ./src/minigui/src/gdi/rect.c
	$(CC) -c ./src/minigui/src/gdi/rect.c -o obj/rect.o $(CFLAGS)

obj/screen.o: ./src/minigui/src/gdi/screen.c
	$(CC) -c ./src/minigui/src/gdi/screen.c -o obj/screen.o $(CFLAGS)

obj/window.o: ./src/minigui/src/gui/window.c
	$(CC) -c ./src/minigui/src/gui/window.c -o obj/window.o $(CFLAGS)

obj/accelkey.o: ./src/minigui/src/gui/accelkey.c
	$(CC) -c ./src/minigui/src/gui/accelkey.c -o obj/accelkey.o $(CFLAGS)

obj/caret.o: ./src/minigui/src/gui/caret.c
	$(CC) -c ./src/minigui/src/gui/caret.c -o obj/caret.o $(CFLAGS)

obj/ctrlclass.o: ./src/minigui/src/gui/ctrlclass.c
	$(CC) -c ./src/minigui/src/gui/ctrlclass.c -o obj/ctrlclass.o $(CFLAGS)

obj/defkeymap.o: ./src/minigui/src/gui/defkeymap.c
	$(CC) -c ./src/minigui/src/gui/defkeymap.c -o obj/defkeymap.o $(CFLAGS)

obj/de-kmap.o: ./src/minigui/src/gui/de-kmap.c
	$(CC) -c ./src/minigui/src/gui/de-kmap.c -o obj/de-kmap.o $(CFLAGS)

obj/de-latin1-kmap.o: ./src/minigui/src/gui/de-latin1-kmap.c
	$(CC) -c ./src/minigui/src/gui/de-latin1-kmap.c -o obj/de-latin1-kmap.o $(CFLAGS)

obj/de-latin1-nodeadkeys-kmap.o: ./src/minigui/src/gui/de-latin1-nodeadkeys-kmap.c
	$(CC) -c ./src/minigui/src/gui/de-latin1-nodeadkeys-kmap.c -o obj/de-latin1-nodeadkeys-kmap.o $(CFLAGS)

obj/dialog.o: ./src/minigui/src/gui/dialog.c
	$(CC) -c ./src/minigui/src/gui/dialog.c -o obj/dialog.o $(CFLAGS)

obj/element.o: ./src/minigui/src/gui/element.c
	$(CC) -c ./src/minigui/src/gui/element.c -o obj/element.o $(CFLAGS)

obj/es-cp850-kmap.o: ./src/minigui/src/gui/es-cp850-kmap.c
	$(CC) -c ./src/minigui/src/gui/es-cp850-kmap.c -o obj/es-cp850-kmap.o $(CFLAGS)

obj/es-kmap.o: ./src/minigui/src/gui/es-kmap.c
	$(CC) -c ./src/minigui/src/gui/es-kmap.c -o obj/es-kmap.o $(CFLAGS)

obj/fr-kmap.o: ./src/minigui/src/gui/fr-kmap.c
	$(CC) -c ./src/minigui/src/gui/fr-kmap.c -o obj/fr-kmap.o $(CFLAGS)

obj/fr-pc-kmap.o: ./src/minigui/src/gui/fr-pc-kmap.c
	$(CC) -c ./src/minigui/src/gui/fr-pc-kmap.c -o obj/fr-pc-kmap.o $(CFLAGS)

obj/it-kmap.o: ./src/minigui/src/gui/it-kmap.c
	$(CC) -c ./src/minigui/src/gui/it-kmap.c -o obj/it-kmap.o $(CFLAGS)

obj/keyboard.o: ./src/minigui/src/gui/keyboard.c
	$(CC) -c ./src/minigui/src/gui/keyboard.c -o obj/keyboard.o $(CFLAGS)

obj/menu.o: ./src/minigui/src/gui/menu.c
	$(CC) -c ./src/minigui/src/gui/menu.c -o obj/menu.o $(CFLAGS)

obj/systext.o: ./src/minigui/src/misc/systext.c
	$(CC) -c ./src/minigui/src/misc/systext.c -o obj/systext.o $(CFLAGS)

obj/about.o: ./src/minigui/src/misc/about.c
	$(CC) -c ./src/minigui/src/misc/about.c -o obj/about.o $(CFLAGS)

obj/clipboard.o: ./src/minigui/src/misc/clipboard.c
	$(CC) -c ./src/minigui/src/misc/clipboard.c -o obj/clipboard.o $(CFLAGS)

obj/endianrw.o: ./src/minigui/src/misc/endianrw.c
	$(CC) -c ./src/minigui/src/misc/endianrw.c -o obj/endianrw.o $(CFLAGS)

obj/math3d.o: ./src/minigui/src/misc/math3d.c
	$(CC) -c ./src/minigui/src/misc/math3d.c -o obj/math3d.o $(CFLAGS)

obj/math.o: ./src/minigui/src/misc/math.c
	$(CC) -c ./src/minigui/src/misc/math.c -o obj/math.o $(CFLAGS)

obj/nposix.o: ./src/minigui/src/misc/nposix.c
	$(CC) -c ./src/minigui/src/misc/nposix.c -o obj/nposix.o $(CFLAGS)

obj/rwops.o: ./src/minigui/src/misc/rwops.c
	$(CC) -c ./src/minigui/src/misc/rwops.c -o obj/rwops.o $(CFLAGS)

obj/winbmp.o: ./src/minigui/src/mybmp/winbmp.c
	$(CC) -c ./src/minigui/src/mybmp/winbmp.c -o obj/winbmp.o $(CFLAGS)

obj/gif.o: ./src/minigui/src/mybmp/gif.c
	$(CC) -c ./src/minigui/src/mybmp/gif.c -o obj/gif.o $(CFLAGS)

obj/jpeg.o: ./src/minigui/src/mybmp/jpeg.c
	$(CC) -c ./src/minigui/src/mybmp/jpeg.c -o obj/jpeg.o $(CFLAGS)

obj/mybmp.o: ./src/minigui/src/mybmp/mybmp.c
	$(CC) -c ./src/minigui/src/mybmp/mybmp.c -o obj/mybmp.o $(CFLAGS)

obj/png.o: ./src/minigui/src/mybmp/png.c
	$(CC) -c ./src/minigui/src/mybmp/png.c -o obj/png.o $(CFLAGS)

obj/syscfg.o: ./src/minigui/src/sysres/syscfg.c
	$(CC) -c ./src/minigui/src/sysres/syscfg.c -o obj/syscfg.o $(CFLAGS)

obj/resource.o: ./src/minigui/src/sysres/resource.c
	$(CC) -c ./src/minigui/src/sysres/resource.c -o obj/resource.o $(CFLAGS)

obj/init-lite.o: ./src/minigui/src/kernel/init-lite.c
	$(CC) -c ./src/minigui/src/kernel/init-lite.c -o obj/init-lite.o $(CFLAGS)

obj/blockheap.o: ./src/minigui/src/kernel/blockheap.c
	$(CC) -c ./src/minigui/src/kernel/blockheap.c -o obj/blockheap.o $(CFLAGS)

obj/event.o: ./src/minigui/src/kernel/event.c
	$(CC) -c ./src/minigui/src/kernel/event.c -o obj/event.o $(CFLAGS)

obj/fixstr.o: ./src/minigui/src/kernel/fixstr.c
	$(CC) -c ./src/minigui/src/kernel/fixstr.c -o obj/fixstr.o $(CFLAGS)

obj/message.o: ./src/minigui/src/kernel/message.c
	$(CC) -c ./src/minigui/src/kernel/message.c -o obj/message.o $(CFLAGS)

obj/timer-lite.o: ./src/minigui/src/kernel/timer-lite.c
	$(CC) -c ./src/minigui/src/kernel/timer-lite.c -o obj/timer-lite.o $(CFLAGS)

obj/misc.o: ./src/minigui/src/misc/misc.c
	$(CC) -c ./src/minigui/src/misc/misc.c -o obj/misc.o $(CFLAGS)

obj/desktop-lite.o: ./src/minigui/src/kernel/desktop-lite.c
	$(CC) -c ./src/minigui/src/kernel/desktop-lite.c -o obj/desktop-lite.o $(CFLAGS)

obj/cursor-sa.o: ./src/minigui/src/kernel/cursor-sa.c
	$(CC) -c ./src/minigui/src/kernel/cursor-sa.c -o obj/cursor-sa.o $(CFLAGS)

obj/ial.o: ./src/minigui/src/ial/ial.c
	$(CC) -c ./src/minigui/src/ial/ial.c -o obj/ial.o $(CFLAGS)

obj/dummy.o: ./src/minigui/src/ial/dummy.c
	$(CC) -c ./src/minigui/src/ial/dummy.c -o obj/dummy.o $(CFLAGS)

obj/standalone.o: ./src/minigui/src/kernel/standalone.c
	$(CC) -c ./src/minigui/src/kernel/standalone.c -o obj/standalone.o $(CFLAGS)
