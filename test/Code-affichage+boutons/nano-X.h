#ifndef	_NANO_X_H
#define	_NANO_X_H
/* Copyright (c) 1999, 2000 Greg Haerr <greg@censoft.com>
 * Copyright (c) 2000 Alex Holden <alex@linuxhacker.org>
 * Copyright (c) 1991 David I. Bell
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X public definition header file:  user applications should
 * include only this header file.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "mwtypes.h"			/* exported engine MW* types*/

/*
 * The following typedefs are inherited from the Microwindows
 * engine layer.
 */
typedef MWCOORD 	GR_COORD;	/* coordinate value */
typedef MWCOORD 	GR_SIZE;	/* size value */
typedef MWCOLORVAL 	GR_COLOR;	/* full color value */
typedef MWPIXELVAL 	GR_PIXELVAL;	/* hw pixel value*/
typedef MWIMAGEBITS 	GR_BITMAP;	/* bitmap unit */
typedef MWUCHAR 	GR_CHAR;	/* filename, window title */
typedef MWKEY	 	GR_KEY;		/* keystroke value*/
typedef MWSCANCODE	GR_SCANCODE;	/* oem keystroke scancode value*/
typedef MWKEYMOD	GR_KEYMOD;	/* keystroke modifiers*/
typedef MWSCREENINFO	GR_SCREEN_INFO;	/* screen information*/
typedef MWFONTINFO	GR_FONT_INFO;	/* font information*/
typedef MWIMAGEINFO	GR_IMAGE_INFO;	/* image information*/
typedef MWIMAGEHDR	GR_IMAGE_HDR;	/* multicolor image representation*/
typedef MWLOGFONT	GR_LOGFONT;	/* logical font descriptor*/
typedef MWPALENTRY	GR_PALENTRY;	/* palette entry*/
typedef MWPOINT		GR_POINT;	/* definition of a point*/
typedef MWTIMEOUT	GR_TIMEOUT;	/* timeout value */

/* Basic typedefs. */
typedef int 		GR_COUNT;	/* number of items */
typedef unsigned char	GR_CHAR_WIDTH;	/* width of character */
typedef unsigned int	GR_ID;		/* resource ids */
typedef GR_ID		GR_DRAW_ID;	/* drawable id */
typedef GR_DRAW_ID	GR_WINDOW_ID;	/* window or pixmap id */
typedef GR_ID		GR_GC_ID;	/* graphics context id */
typedef GR_ID		GR_REGION_ID;	/* region id */
typedef GR_ID		GR_FONT_ID;	/* font id */
typedef GR_ID		GR_IMAGE_ID;	/* image id */
typedef unsigned short	GR_BOOL;	/* boolean value */
typedef int		GR_ERROR;	/* error types*/
typedef int		GR_EVENT_TYPE;	/* event types */
typedef int		GR_UPDATE_TYPE;	/* window update types */
typedef unsigned long	GR_EVENT_MASK;	/* event masks */
typedef char		GR_FUNC_NAME[25];/* function name */
typedef unsigned long	GR_WM_PROPS;	/* window property flags */
typedef unsigned long	GR_SERIALNO;	/* Selection request ID number */
typedef unsigned short	GR_MIMETYPE;	/* Index into mime type list */
typedef unsigned long	GR_LENGTH;	/* Length of a block of data */
typedef unsigned int	GR_BUTTON;	/* mouse button value*/

/* Nano-X rectangle, different from MWRECT*/
typedef struct {
	GR_COORD x;
	GR_COORD y;
	GR_SIZE  width;
	GR_SIZE  height;
} GR_RECT;

/* The root window id. */
#define	GR_ROOT_WINDOW_ID	((GR_WINDOW_ID) 1)

/* GR_COLOR color constructor*/
#define GR_RGB(r,g,b)		MWRGB(r,g,b)

/* Drawing modes for GrSetGCMode*/
#define	GR_MODE_SET		MWMODE_SET
#define	GR_MODE_XOR		MWMODE_XOR
#define	GR_MODE_OR		MWMODE_OR
#define	GR_MODE_AND		MWMODE_AND
#define GR_MODE_DRAWMASK	0x00FF
#define GR_MODE_EXCLUDECHILDREN	0x0100		/* exclude children on clip*/

/* builtin font std names*/
#define GR_FONT_SYSTEM_VAR	MWFONT_SYSTEM_VAR
#define GR_FONT_GUI_VAR		MWFONT_GUI_VAR
#define GR_FONT_OEM_FIXED	MWFONT_OEM_FIXED
#define GR_FONT_SYSTEM_FIXED	MWFONT_SYSTEM_FIXED

/* GrText/GrGetTextSize encoding flags*/
#define GR_TFASCII		MWTF_ASCII
#define GR_TFUTF8		MWTF_UTF8
#define GR_TFUC16		MWTF_UC16
#define GR_TFUC32		MWTF_UC32
#define GR_TFPACKMASK		MWTF_PACKMASK

/* GrText alignment flags*/
#define GR_TFTOP		MWTF_TOP
#define GR_TFBASELINE		MWTF_BASELINE
#define GR_TFBOTTOM		MWTF_BOTTOM

/* GrSetFontAttr flags*/
#define GR_TFKERNING		MWTF_KERNING
#define GR_TFANTIALIAS		MWTF_ANTIALIAS
#define GR_TFUNDERLINE		MWTF_UNDERLINE

/* GrArc, GrArcAngle types*/
#define GR_ARC		MWARC		/* arc only*/
#define GR_ARCOUTLINE	MWARCOUTLINE	/* arc + outline*/
#define GR_PIE		MWPIE		/* pie (filled)*/

/* Booleans */
#define	GR_FALSE		0
#define	GR_TRUE			1

/* Loadable Image support definition */
#define GR_IMAGE_MAX_SIZE	(-1)

/* Button flags */
#define	GR_BUTTON_R		MWBUTTON_R 	/* right button*/
#define	GR_BUTTON_M		MWBUTTON_M	/* middle button*/
#define	GR_BUTTON_L		MWBUTTON_L	/* left button */
#define	GR_BUTTON_ANY		(MWBUTTON_R|MWBUTTON_M|MWBUTTON_L) /* any*/

/* GrSetBackgroundPixmap flags */
#define GR_BACKGROUND_TILE	0	/* Tile across the window */
#define GR_BACKGROUND_CENTER	1	/* Draw in center of window */
#define GR_BACKGROUND_TOPLEFT	2	/* Draw at top left of window */
#define GR_BACKGROUND_STRETCH	4	/* Stretch image to fit window*/
#define GR_BACKGROUND_TRANS	8	/* Don't fill in gaps */

/* GrNewPixmapFromData flags*/
#define GR_BMDATA_BYTEREVERSE	01	/* byte-reverse bitmap data*/
#define GR_BMDATA_BYTESWAP	02	/* byte-swap bitmap data*/

#if 0 /* don't define unimp'd flags*/
/* Window property flags */
#define GR_WM_PROP_NORESIZE	0x04	/* don't let user resize window */
#define GR_WM_PROP_NOICONISE	0x08	/* don't let user iconise window */
#define GR_WM_PROP_NOWINMENU	0x10	/* don't display a window menu button */
#define GR_WM_PROP_NOROLLUP	0x20	/* don't let user roll window up */
#define GR_WM_PROP_ONTOP	0x200	/* try to keep window always on top */
#define GR_WM_PROP_STICKY	0x400	/* keep window after desktop change */
#define GR_WM_PROP_DND		0x2000	/* accept drag and drop icons */
#endif

/* Window properties*/
#define GR_WM_PROPS_NOBACKGROUND 0x00000001L/* Don't draw window background*/
#define GR_WM_PROPS_NOFOCUS	 0x00000002L /* Don't set focus to this window*/
#define GR_WM_PROPS_NOMOVE	 0x00000004L /* Don't let user move window*/
#define GR_WM_PROPS_NORAISE	 0x00000008L /* Don't let user raise window*/
#define GR_WM_PROPS_NODECORATE	 0x00000010L /* Don't redecorate window*/
#define GR_WM_PROPS_NOAUTOMOVE	 0x00000020L /* Don't move window on 1st map*/
#define GR_WM_PROPS_NOAUTORESIZE 0x00000040L /* Don't resize window on 1st map*/

/* default decoration style*/
#define GR_WM_PROPS_APPWINDOW	0x00000000L /* Leave appearance to WM*/
#define GR_WM_PROPS_APPMASK	0xF0000000L /* Appearance mask*/
#define GR_WM_PROPS_BORDER	0x80000000L /* Single line border*/
#define GR_WM_PROPS_APPFRAME	0x40000000L /* 3D app frame (overrides border)*/
#define GR_WM_PROPS_CAPTION	0x20000000L /* Title bar*/
#define GR_WM_PROPS_CLOSEBOX	0x10000000L /* Close box*/
#define GR_WM_PROPS_MAXIMIZE	0x08000000L /* Application is maximized*/

/* Flags for indicating valid bits in GrSetWMProperties call*/
#define GR_WM_FLAGS_PROPS	0x0001	/* Properties*/
#define GR_WM_FLAGS_TITLE	0x0002	/* Title*/
#define GR_WM_FLAGS_BACKGROUND	0x0004	/* Background color*/
#define GR_WM_FLAGS_BORDERSIZE	0x0008	/* Border size*/
#define GR_WM_FLAGS_BORDERCOLOR	0x0010	/* Border color*/

/* Window manager properties used by the Gr[GS]etWMProperties calls. */
/* NOTE: this struct must be hand-packed to a DWORD boundary for nxproto.h*/
typedef struct {
  GR_WM_PROPS flags;		/* Which properties valid in struct for set*/
  GR_WM_PROPS props;		/* Window property bits*/
  GR_CHAR *title;		/* Window title*/
  GR_COLOR background;		/* Window background color*/
  GR_SIZE bordersize;		/* Window border size*/
  GR_COLOR bordercolor;		/* Window border color*/
} GR_WM_PROPERTIES;

/* Window properties returned by the GrGetWindowInfo call. */
typedef struct {
  GR_WINDOW_ID wid;		/* window id (or 0 if no such window) */
  GR_WINDOW_ID parent;		/* parent window id */
  GR_WINDOW_ID child;		/* first child window id (or 0) */
  GR_WINDOW_ID sibling;		/* next sibling window id (or 0) */
  GR_BOOL inputonly;		/* TRUE if window is input only */
  GR_BOOL mapped;		/* TRUE if window is mapped */
  GR_COUNT unmapcount;		/* reasons why window is unmapped */
  GR_COORD x;			/* absolute x position of window */
  GR_COORD y;			/* absolute y position of window */
  GR_SIZE width;		/* width of window */
  GR_SIZE height;		/* height of window */
  GR_SIZE bordersize;		/* size of border */
  GR_COLOR bordercolor;		/* color of border */
  GR_COLOR background;		/* background color */
  GR_EVENT_MASK eventmask;	/* current event mask for this client */
  GR_WM_PROPS props;		/* window properties */
} GR_WINDOW_INFO;

/* Graphics context properties returned by the GrGetGCInfo call. */
typedef struct {
  GR_GC_ID gcid;		/* GC id (or 0 if no such GC) */
  int mode;			/* drawing mode */
  GR_REGION_ID region;		/* user region */
  GR_FONT_ID font;		/* font number */
  GR_COLOR foreground;		/* foreground color */
  GR_COLOR background;		/* background color */
  GR_BOOL usebackground;	/* use background in bitmaps */
} GR_GC_INFO;

/* color palette*/
typedef struct {
  GR_COUNT count;		/* # valid entries*/
  GR_PALENTRY palette[256];	/* palette*/
} GR_PALETTE;

/* Error codes */
#define	GR_ERROR_BAD_WINDOW_ID		1
#define	GR_ERROR_BAD_GC_ID		2
#define	GR_ERROR_BAD_CURSOR_SIZE	3
#define	GR_ERROR_MALLOC_FAILED		4
#define	GR_ERROR_BAD_WINDOW_SIZE	5
#define	GR_ERROR_KEYBOARD_ERROR		6
#define	GR_ERROR_MOUSE_ERROR		7
#define	GR_ERROR_INPUT_ONLY_WINDOW	8
#define	GR_ERROR_ILLEGAL_ON_ROOT_WINDOW	9
#define	GR_ERROR_TOO_MUCH_CLIPPING	10
#define	GR_ERROR_SCREEN_ERROR		11
#define	GR_ERROR_UNMAPPED_FOCUS_WINDOW	12
#define	GR_ERROR_BAD_DRAWING_MODE	13

/* Event types.
 * Mouse motion is generated for every motion of the mouse, and is used to
 * track the entire history of the mouse (many events and lots of overhead).
 * Mouse position ignores the history of the motion, and only reports the
 * latest position of the mouse by only queuing the latest such event for
 * any single client (good for rubber-banding).
 */
#define	GR_EVENT_TYPE_ERROR		(-1)
#define	GR_EVENT_TYPE_NONE		0
#define	GR_EVENT_TYPE_EXPOSURE		1
#define	GR_EVENT_TYPE_BUTTON_DOWN	2
#define	GR_EVENT_TYPE_BUTTON_UP		3
#define	GR_EVENT_TYPE_MOUSE_ENTER	4
#define	GR_EVENT_TYPE_MOUSE_EXIT	5
#define	GR_EVENT_TYPE_MOUSE_MOTION	6
#define	GR_EVENT_TYPE_MOUSE_POSITION	7
#define	GR_EVENT_TYPE_KEY_DOWN		8
#define	GR_EVENT_TYPE_KEY_UP		9
#define	GR_EVENT_TYPE_FOCUS_IN		10
#define	GR_EVENT_TYPE_FOCUS_OUT		11
#define GR_EVENT_TYPE_FDINPUT		12
#define GR_EVENT_TYPE_UPDATE		13
#define GR_EVENT_TYPE_CHLD_UPDATE	14	/* never sent*/
#define GR_EVENT_TYPE_CLOSE_REQ		15
#define GR_EVENT_TYPE_TIMEOUT		16
#define GR_EVENT_TYPE_SCREENSAVER	17
#define GR_EVENT_TYPE_CLIENT_DATA_REQ	18
#define GR_EVENT_TYPE_CLIENT_DATA	19
#define GR_EVENT_TYPE_SELECTION_CHANGED 20

/* Event masks */
#define	GR_EVENTMASK(n)			(((GR_EVENT_MASK) 1) << (n))

#define	GR_EVENT_MASK_NONE		GR_EVENTMASK(GR_EVENT_TYPE_NONE)
#define	GR_EVENT_MASK_ERROR		GR_EVENTMASK(GR_EVENT_TYPE_ERROR)
#define	GR_EVENT_MASK_EXPOSURE		GR_EVENTMASK(GR_EVENT_TYPE_EXPOSURE)
#define	GR_EVENT_MASK_BUTTON_DOWN	GR_EVENTMASK(GR_EVENT_TYPE_BUTTON_DOWN)
#define	GR_EVENT_MASK_BUTTON_UP		GR_EVENTMASK(GR_EVENT_TYPE_BUTTON_UP)
#define	GR_EVENT_MASK_MOUSE_ENTER	GR_EVENTMASK(GR_EVENT_TYPE_MOUSE_ENTER)
#define	GR_EVENT_MASK_MOUSE_EXIT	GR_EVENTMASK(GR_EVENT_TYPE_MOUSE_EXIT)
#define	GR_EVENT_MASK_MOUSE_MOTION	GR_EVENTMASK(GR_EVENT_TYPE_MOUSE_MOTION)
#define	GR_EVENT_MASK_MOUSE_POSITION	GR_EVENTMASK(GR_EVENT_TYPE_MOUSE_POSITION)
#define	GR_EVENT_MASK_KEY_DOWN		GR_EVENTMASK(GR_EVENT_TYPE_KEY_DOWN)
#define	GR_EVENT_MASK_KEY_UP		GR_EVENTMASK(GR_EVENT_TYPE_KEY_UP)
#define	GR_EVENT_MASK_FOCUS_IN		GR_EVENTMASK(GR_EVENT_TYPE_FOCUS_IN)
#define	GR_EVENT_MASK_FOCUS_OUT		GR_EVENTMASK(GR_EVENT_TYPE_FOCUS_OUT)
#define	GR_EVENT_MASK_FDINPUT		GR_EVENTMASK(GR_EVENT_TYPE_FDINPUT)
#define	GR_EVENT_MASK_UPDATE		GR_EVENTMASK(GR_EVENT_TYPE_UPDATE)
#define	GR_EVENT_MASK_CHLD_UPDATE	GR_EVENTMASK(GR_EVENT_TYPE_CHLD_UPDATE)
#define	GR_EVENT_MASK_CLOSE_REQ		GR_EVENTMASK(GR_EVENT_TYPE_CLOSE_REQ)
#define	GR_EVENT_MASK_TIMEOUT		GR_EVENTMASK(GR_EVENT_TYPE_TIMEOUT)
#define GR_EVENT_MASK_SCREENSAVER	GR_EVENTMASK(GR_EVENT_TYPE_SCREENSAVER)
#define GR_EVENT_MASK_CLIENT_DATA_REQ	GR_EVENTMASK(GR_EVENT_TYPE_CLIENT_DATA_REQ)
#define GR_EVENT_MASK_CLIENT_DATA	GR_EVENTMASK(GR_EVENT_TYPE_CLIENT_DATA)
#define GR_EVENT_MASK_SELECTION_CHANGED GR_EVENTMASK(GR_EVENT_TYPE_SELECTION_CHANGED)
#define	GR_EVENT_MASK_ALL		((GR_EVENT_MASK) -1L)

/* update event types */
#define GR_UPDATE_MAP		1
#define GR_UPDATE_UNMAP		2
#define GR_UPDATE_MOVE		3
#define GR_UPDATE_SIZE		4
#define GR_UPDATE_UNMAPTEMP	5	/* unmap during window move/resize*/
#define GR_UPDATE_ACTIVATE	6	/* toplevel window [de]activate*/
#define GR_UPDATE_DESTROY	7

/* Event for errors detected by the server.
 * These events are not delivered to GrGetNextEvent, but instead call
 * the user supplied error handling function.  Only the first one of
 * these errors at a time is saved for delivery to the client since
 * there is not much to be done about errors anyway except complain
 * and exit.
 */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_FUNC_NAME name;		/* function name which failed */
  GR_ERROR code;		/* error code */
  GR_ID id;			/* resource id (maybe useless) */
} GR_EVENT_ERROR;

/* Event for a mouse button pressed down or released. */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* window id event delivered to */
  GR_WINDOW_ID subwid;		/* sub-window id (pointer was in) */
  GR_COORD rootx;		/* root window x coordinate */
  GR_COORD rooty;		/* root window y coordinate */
  GR_COORD x;			/* window x coordinate of mouse */
  GR_COORD y;			/* window y coordinate of mouse */
  GR_BUTTON buttons;		/* current state of all buttons */
  GR_BUTTON changebuttons;	/* buttons which went down or up */
  GR_KEYMOD modifiers;		/* modifiers (MWKMOD_SHIFT, etc)*/
  GR_TIMEOUT time;		/* tickcount time value*/
} GR_EVENT_BUTTON;

/* Event for a keystroke typed for the window with has focus. */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* window id event delived to */
  GR_WINDOW_ID subwid;		/* sub-window id (pointer was in) */
  GR_COORD rootx;		/* root window x coordinate */
  GR_COORD rooty;		/* root window y coordinate */
  GR_COORD x;			/* window x coordinate of mouse */
  GR_COORD y;			/* window y coordinate of mouse */
  GR_BUTTON buttons;		/* current state of buttons */
  GR_KEYMOD modifiers;		/* modifiers (MWKMOD_SHIFT, etc)*/
  GR_KEY ch;			/* 16-bit unicode key value, MWKEY_xxx */
  GR_SCANCODE scancode;		/* OEM scancode value if available*/
} GR_EVENT_KEYSTROKE;

/* Event for exposure for a region of a window. */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* window id */
  GR_COORD x;			/* window x coordinate of exposure */
  GR_COORD y;			/* window y coordinate of exposure */
  GR_SIZE width;		/* width of exposure */
  GR_SIZE height;		/* height of exposure */
} GR_EVENT_EXPOSURE;

/* General events for focus in or focus out for a window, or mouse enter
 * or mouse exit from a window, or window unmapping or mapping, etc.
 */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* window id */
  GR_WINDOW_ID otherid;		/* new/old focus id for focus events*/
} GR_EVENT_GENERAL;

/* Events for mouse motion or mouse position. */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* window id event delivered to */
  GR_WINDOW_ID subwid;		/* sub-window id (pointer was in) */
  GR_COORD rootx;		/* root window x coordinate */
  GR_COORD rooty;		/* root window y coordinate */
  GR_COORD x;			/* window x coordinate of mouse */
  GR_COORD y;			/* window y coordinate of mouse */
  GR_BUTTON buttons;		/* current state of buttons */
  GR_KEYMOD modifiers;		/* modifiers (MWKMOD_SHIFT, etc)*/
} GR_EVENT_MOUSE;

/* GrRegisterInput event*/
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  int		fd;		/* input fd*/
} GR_EVENT_FDINPUT;

/* GR_EVENT_TYPE_UPDATE */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* select window id*/
  GR_WINDOW_ID subwid;		/* update window id (=wid for UPDATE event)*/
  GR_COORD x;			/* new window x coordinate */
  GR_COORD y;			/* new window y coordinate */
  GR_SIZE width;		/* new width */
  GR_SIZE height;		/* new height */
  GR_UPDATE_TYPE utype;		/* update_type */
} GR_EVENT_UPDATE;

/* GR_EVENT_TYPE_SCREENSAVER */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_BOOL activate;		/* true = activate, false = deactivate */
} GR_EVENT_SCREENSAVER;

/* GR_EVENT_TYPE_CLIENT_DATA_REQ */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* ID of requested window */
  GR_WINDOW_ID rid;		/* ID of window to send data to */
  GR_SERIALNO serial;		/* Serial number of transaction */
  GR_MIMETYPE mimetype;		/* Type to supply data as */
} GR_EVENT_CLIENT_DATA_REQ;

/* GR_EVENT_TYPE_CLIENT_DATA */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID wid;		/* ID of window data is destined for */
  GR_WINDOW_ID rid;		/* ID of window data is from */
  GR_SERIALNO serial;		/* Serial number of transaction */
  unsigned long len;		/* Total length of data */
  unsigned long datalen;	/* Length of following data */
  void *data;			/* Pointer to data (filled in on client side) */
} GR_EVENT_CLIENT_DATA;

/* GR_EVENT_TYPE_SELECTION_CHANGED */
typedef struct {
  GR_EVENT_TYPE type;		/* event type */
  GR_WINDOW_ID new_owner;	/* ID of new selection owner */
} GR_EVENT_SELECTION_CHANGED;

/*
 * Union of all possible event structures.
 * This is the structure returned by the GrGetNextEvent and similar routines.
 */
typedef union {
  GR_EVENT_TYPE type;			/* event type */
  GR_EVENT_ERROR error;			/* error event */
  GR_EVENT_GENERAL general;		/* general window events */
  GR_EVENT_BUTTON button;		/* button events */
  GR_EVENT_KEYSTROKE keystroke;		/* keystroke events */
  GR_EVENT_EXPOSURE exposure;		/* exposure events */
  GR_EVENT_MOUSE mouse;			/* mouse motion events */
  GR_EVENT_FDINPUT fdinput;		/* fd input events*/
  GR_EVENT_UPDATE update;		/* window update events */
  GR_EVENT_SCREENSAVER screensaver; 	/* Screen saver events */
  GR_EVENT_CLIENT_DATA_REQ clientdatareq; /* Request for client data events */
  GR_EVENT_CLIENT_DATA clientdata;	/* Client data events */
  GR_EVENT_SELECTION_CHANGED selectionchanged; /* Selection owner changed */
} GR_EVENT;

typedef void (*GR_FNCALLBACKEVENT)(GR_EVENT *);

/* Pixel packings within words. */
#define	GR_BITMAPBITS	(sizeof(GR_BITMAP) * 8)
#define	GR_ZEROBITS	((GR_BITMAP) 0x0000)
#define	GR_ONEBITS	((GR_BITMAP) 0xffff)
#define	GR_FIRSTBIT	((GR_BITMAP) 0x8000)
#define	GR_LASTBIT	((GR_BITMAP) 0x0001)
#define	GR_BITVALUE(n)	((GR_BITMAP) (((GR_BITMAP) 1) << (n)))
#define	GR_SHIFTBIT(m)	((GR_BITMAP) ((m) << 1))
#define	GR_NEXTBIT(m)	((GR_BITMAP) ((m) >> 1))
#define	GR_TESTBIT(m)	(((m) & GR_FIRSTBIT) != 0)

/* Size of bitmaps. */
#define	GR_BITMAP_SIZE(width, height)	((height) * \
  (((width) + sizeof(GR_BITMAP) * 8 - 1) / (sizeof(GR_BITMAP) * 8)))

#define	GR_MAX_BITMAP_SIZE \
  GR_BITMAP_SIZE(MAX_CURSOR_SIZE, MAX_CURSOR_SIZE)

/* GrGetSysColor colors*/
/* desktop background*/
#define GR_COLOR_DESKTOP           0

/* caption colors*/
#define GR_COLOR_ACTIVECAPTION     1
#define GR_COLOR_ACTIVECAPTIONTEXT 2
#define GR_COLOR_INACTIVECAPTION   3
#define GR_COLOR_INACTIVECAPTIONTEXT 4

/* 3d border shades*/
#define GR_COLOR_WINDOWFRAME       5
#define GR_COLOR_BTNSHADOW         6
#define GR_COLOR_3DLIGHT           7
#define GR_COLOR_BTNHIGHLIGHT      8

/* top level application window backgrounds/text*/
#define GR_COLOR_APPWINDOW         9
#define GR_COLOR_APPTEXT           10

/* button control backgrounds/text (usually same as app window colors)*/
#define GR_COLOR_BTNFACE           11
#define GR_COLOR_BTNTEXT           12

/* edit/listbox control backgrounds/text, selected highlights*/
#define GR_COLOR_WINDOW            13
#define GR_COLOR_WINDOWTEXT        14
#define GR_COLOR_HIGHLIGHT         15
#define GR_COLOR_HIGHLIGHTTEXT     16
#define GR_COLOR_GRAYTEXT          17

/* menu backgrounds/text*/
#define GR_COLOR_MENUTEXT          18
#define GR_COLOR_MENU              19

/* Error strings per error number*/
#define GR_ERROR_STRINGS		\
	"",				\
	"Bad window id: %d\n",		\
	"Bad graphics context: %d\n",	\
	"Bad cursor size\n",		\
	"Out of server memory\n",	\
	"Bad window size: %d\n",	\
	"Keyboard error\n",		\
	"Mouse error\n",		\
	"Input only window: %d\n",	\
	"Illegal on root window: %d\n",	\
	"Clipping overflow\n",		\
	"Screen error\n",		\
	"Unmapped focus window: %d\n",	\
	"Bad drawing mode gc: %d\n"

extern char *nxErrorStrings[];

/* Public graphics routines. */
/**force execution of GL commands in finite time
The GrFlush function flushs graphics request buffer to the device. Flushing the graphics buffer causes the server to execute all graphics requests. You only need to flush the buffer if you do not check the event queue frequently since checking the queue automatically flushes the buffer.**/
void		GrFlush(void);

/**opens a client connection to the Nano-X server. This must be the first Nano-X function called by your program.  **/
int		GrOpen(void);

/**flushes any buffered function calls and closes the connection created with the GrOpen function.  **/
void		GrClose(void);

/**retrieves the configuration of the current screen such as number of rows, number of columns, and bits per pixel. **/
void		GrGetScreenInfo(GR_SCREEN_INFO *sip);

/** finds a color in the system pallete with index index, returns the RGB value of that color; otherwise, returns zero.  **/
GR_COLOR	GrGetSysColor(int index);

/**The GrNewWindow function creates a new input-output window as a child of the parent window. The new window inherits the cursor of its parent. Also, the GrNewWindow function does not draw the new window on the client. To draw the new window, use the GrMapWindow function.

The x, y, wihth, and height parameters specify the drawable area of the window. The border draws outside the drawable area. Thus, if you want window, including the border to appear within a particular area, you must adjust the dimensions of the drawing area accordingly.

The new window inherits the cursor of its parent window and initially has no selected events.
You cannot make an input-output window as the child of an input-only window.
x, y-------------------->

    The coordinates of the new window relative to the parent window.
width, height-------------------->

    The dimension of the new window.
bordersize-------------------->

    The thickness of the new window's border in pixels.
background-------------------->

    The background color as an RGB value.
bordercolor-------------------->

    The border color as an RGB value.  **/
GR_WINDOW_ID	GrNewWindow(GR_WINDOW_ID parent, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height, GR_SIZE bordersize,
			GR_COLOR background, GR_COLOR bordercolor);

/**Parameters===================

width, height ----------------->

    The dimension of the new pixmap.
addr --------------------------->

    Reserved for future use to support shared memory. For now, use zero or NULL.

Return Value=======================

Returns the window ID of the new pixmap.  **/
GR_WINDOW_ID    GrNewPixmap(GR_SIZE width, GR_SIZE height, void * addr);

/**Parameters==============

parent---------------->

    The window ID of new window's parent window.
x, y----------------->

    The coordinates of the new window relative to the parent window.
width, height----------->

    The dimension of the new window.

Return Value=============

Returns the window ID of the new input window.
Description

The GrNewInputWindow function creates a window with special focus
rules to help with user input.
 An input-only window is invisible and cannot be drawn into. You can select events for an input-only window and set the window's cursor. Like input-output windows, input-only windows start out unmapped, and you must map the window to make it effective. The new window inherits the cursor of the parent window, and initially is set to select no events.  **/
GR_WINDOW_ID	GrNewInputWindow(GR_WINDOW_ID parent, GR_COORD x, GR_COORD y,
				GR_SIZE width, GR_SIZE height);

/**Parameters

wid---------------->

    The window ID of the window to destroy
 removes a window from the screen and frees all resources allocated for that window and all that window's children.  **/
void		GrDestroyWindow(GR_WINDOW_ID wid);

/**allocates a new graphic context with all parameters set to the default values.

Member	Default Value
mode	GR_MODE_SET
font	0
foreground	WHITE
background	BLACK  **/
GR_GC_ID	GrNewGC(void);

/**Parameters========

gc ---------------->

    The graphics context you want to copy.

Return Value=============

Returns the ID of the new graphic context.
Description============

The GrCopyGC function creates a new graphics context and copies all the members from the gc graphics context into the newly created graphics context **/
GR_GC_ID	GrCopyGC(GR_GC_ID gc);

/**Parameters============

gc---------------------->

    The graphics context ID number.
gcip-------------------->

    A pointer to a GR_GC_INFO structure that will store the requested graphics context information.

Description==============

The GrGetGCInfo function retrieves information about the specified graphics context, including the graphics context id, the current font, the foreground color, and the background colors. If the gc graphics context does not exist, the GrGetGCInfo sets the returned ID to 0 and the other GR_GC_INFO information is undefined.   **/
void		GrGetGCInfo(GR_GC_ID gc, GR_GC_INFO *gcip);

/**Destroy a graphics context ---> gc --> The graphics context you want to destroy. **/
void		GrDestroyGC(GR_GC_ID gc);

/**Allocate a new region  === Returns the ID of the new region.**/
GR_REGION_ID	GrNewRegion(void);

/**Parameters===========

mode----------------->

    The fill mode for the polygon. Accepted values: MWPOLY_EVENODD, MWPOLY_WINDING.
count---------------->

    The number of points that define the polygon.
points-------------->

    A pointer to an array of GR_POINT coordinate pairs.

Return Value============

Returns the ID of the new region.
Description============

The GrNewPolygonRegion function creates a region from an array of points that define a polygon.  **/
GR_REGION_ID	GrNewPolygonRegion(int mode, GR_COUNT count, GR_POINT *points);

/** Parameters====

region------------->

    The region you want to destroy.

Description======

The GrDestroyRegion function destroys the region with the region ID you specify. **/
void		GrDestroyRegion(GR_REGION_ID region);

/**Parameters==

region-->

    The ID of the region you want to modify.
rect-->

    A pointer to the GR_RECT rectangle you want to add to the region region.

Description==

The GrUnionRectWithRegion function adds the area of a rectangle to the region region.   **/
void		GrUnionRectWithRegion(GR_REGION_ID region, GR_RECT *rect);

/**Parameters==

dst_rgn-->

    A GR_REGION_ID region that will hold the union of src_rgn1 and src_rgn2.
src_rgn1-->

    The first of two regions to combine.
src_rgn2-->

    The second of two regions to combine.

Description==

The GrUnionRegion function adds the area of two other regions.   **/
void		GrUnionRegion(GR_REGION_ID dst_rgn, GR_REGION_ID src_rgn1,
			GR_REGION_ID src_rgn2);

/**Parameters==

dst_rgn -->

    A GR_REGION_ID region that will hold the intersection of src_rgn1 and src_rgn2.
src_rgn1 -->

    The first of two regions to intersect.
src_rgn2 -->

    The second of two regions to intersect.

Description==

The GrIntersectRegion function intersects the area of two regions.   **/
void		GrIntersectRegion(GR_REGION_ID dst_rgn, GR_REGION_ID src_rgn1,
			GR_REGION_ID src_rgn2);

/**Parameters==

dst_rgn-->

    A GR_REGION_ID region that will hold the difference of src_rgn1 minus src_rgn2.
src_rgn1-->

    The minuend region.
src_rgn2-->

    The subtrahend region.

Description==

The GrSubtractRegion function removes the area of src_rgn2 from src_rgn1 and returns the difference in dst_rgn.   **/
void		GrSubtractRegion(GR_REGION_ID dst_rgn, GR_REGION_ID src_rgn1,
			GR_REGION_ID src_rgn2);

/** Parameters==

dst_rgn -->

    A GR_REGION_ID region that will hold the exclusive or of src_rgn1 and src_rgn2.
src_rgn1-->

    The first of two source regions.
src_rgn2-->

    The second of two source regions.

Description==

The GrXorRegion function calculates the exclusive or of src_rgn2 and src_rgn1 and returns the result in dst_rgn.  **/
void		GrXorRegion(GR_REGION_ID dst_rgn, GR_REGION_ID src_rgn1,
			GR_REGION_ID src_rgn2);

/**Parameters==

gc-->

    The graphics context ID of the graphics context for which you want to set a clip region.
region-->

    The ID of the clip region to use with this graphics context.

Description==

The GrSetGCRegion function sets the clip region to use with the specified graphics context. Any drawing functions will only draw to the screen if the drawing takes place within an area described by the region clip region.   **/
void		GrSetGCRegion(GR_GC_ID gc, GR_REGION_ID region);

/**Parameters==

region-->

    The region in which you want to test for a point.
x, y-->

    The coordinates of the point to test.

Return Value==

Returns one if the region containts the point; otherwise returns zero.
Description==

The GrPointInRegion function checks to see if the region region contains the point (x, y).  **/
GR_BOOL		GrPointInRegion(GR_REGION_ID region, GR_COORD x, GR_COORD y);

/**Parameters==

region -->

    The region in which you want to test for a point.
x, y -->

    The upper left corner of the rectangle to test.
widht, height -->

    The dimensions of the rectangle to test.

Return Value==

Returns one of the following values:

-->Return Value	Description
-->MWRECT_ALLIN	The region contains the entire rectangle
-->MWRECT_PARTIN	The region contains part of the rectangle
-->MWRECT_OUT	No point of the rectangle resides within the region

Description==

The GrRectInRegion function checks to see if the region region contains the rectangle defined by the x, y, width, and height parameters, and returns a value based on whether the rectangle resides entirely within the region, only partly in the region or completely outside the region.  **/
int		GrRectInRegion(GR_REGION_ID region, GR_COORD x, GR_COORD y,
			GR_COORD w, GR_COORD h);

/**Parameters==

region -->

    The ID of the region you want to test.

Return Value==

Returns one if the region is empty; otherwise returns zero.
Description==

The GrEmptyRegion function checks to see if the region region is empty.  **/
GR_BOOL		GrEmptyRegion(GR_REGION_ID region);

/**Parameters==

--> rgn1, rgn2

    The IDs of the regions you want to test.

Return Value==

Returns one if the regions are equal; otherwise returns zero.
Description==

The GrEqualRegion function checks to see if the two specified regions are equal.  **/
GR_BOOL		GrEqualRegion(GR_REGION_ID rgn1, GR_REGION_ID rgn2);

/**Parameters==

region -->

    The region you want to move.
dx-->

    The number of pixels to move the region in the direction parallel to the x-axis.
dy-->

    The number of pixels to move the region in the direction parallel to the y-axis.

Description==

The GrOffsetRegion function moves the specified region. If the region starts at ( x, y ), then after using the GrOffsetRegion function, the top left corner of the region will reside at ( x + dx, y + dy ).  **/
void		GrOffsetRegion(GR_REGION_ID region, GR_SIZE dx, GR_SIZE dy);

/**Parameters==

region-->

    The region you want to move.
rect-->

    A pointer to rectangle structure.

Description==

The GrGetRegionBox function returns a pointer to a GR_RECT scructure that describes the specified region's starting coordinates, height, and width.  **/
int		GrGetRegionBox(GR_REGION_ID region, GR_RECT *rect);

/**Make a window visible on the screen
Parameters==

wid -->

    The ID of the window you want to map.

Description==

The GrMapWindow function maps a window and any unmapped children of that window. For each window mapped, the GrMapWindow function draws the window's border, fills the background area with the window's background color, and creates a GR_EVENT_TYPE_EXPOSURE event for the window.   **/
void		GrMapWindow(GR_WINDOW_ID wid);

/**Remove a window from the screen
wid -->

    The ID of the window you want to unmap.

Description==

The GrUnmapWindow function unmaps a window and any children of that window.   **/
void		GrUnmapWindow(GR_WINDOW_ID wid);

/**Parameters==

wid -->

    The ID of the window you want to raise.
Description==
The GrRaiseWindow function raises a window to the highest level among its siblings -- windows that have the same parent as the wid window. A raised window appears above all its siblings.   **/
void		GrRaiseWindow(GR_WINDOW_ID wid);

/**Parameters==

wid-->

    The ID of the window you want to lower.

Description==

The GrLowerWindow function lowers a window to the lowest level among its siblings -- windows that have the same parent as the wid window. A lowered window appears below all its siblings.
  **/
void		GrLowerWindow(GR_WINDOW_ID wid);

/**wid-->

    The ID of the window you want to move.
x, y-->

    The coordinates of the window's new location.

Description==

The GrMoveWindow function moves a window to the new coordinates (x, y) relative to its parent window.   **/
void		GrMoveWindow(GR_WINDOW_ID wid, GR_COORD x, GR_COORD y);

/**Parameters==

wid-->

    The ID of the window you want to move.
width, height

    The new dimentions for the window.

Description==

The GrResizeWindow function changes a window's dimensions to those specified (width, height).   **/
void		GrResizeWindow(GR_WINDOW_ID wid, GR_SIZE width, GR_SIZE height);

/**Parameters==

wid-->

    The ID of the window to get a new parent.
pwid-->

    The ID of the new parent window.
x, y-->

    The coordinates of the window's new location within the new parent window.

Description==

The GrReparentWindow function changes a windows parent to pwid and locates the window at the coordinates (x, y) relative to its new parent window.   **/
void		GrReparentWindow(GR_WINDOW_ID wid, GR_WINDOW_ID pwid,
			GR_COORD x, GR_COORD y);

/**Parameters==

wid-->

    The window number.
wip-->

    A pointer to a GR_WINDOW_INFO structure that will store the requested window information.

Description==

The GrGetWindowInfo function retrieves information about the specified font, including the parent window id, the id of the window's first child window, the absolute position of the window in its parent window, and the window's event mask. If the GrGetWindowInfo function cannot find a window with the number wid, it sets the returned window number to zero and the remainder of the information is undefined.   **/
void		GrGetWindowInfo(GR_WINDOW_ID wid, GR_WINDOW_INFO *infoptr);

/**  **/
void		GrSetWMProperties(GR_WINDOW_ID wid, GR_WM_PROPERTIES *props);

/**Parameters==

wid-->

    The window number.
wip-->

    A pointer to a GR_WM_PROPERTIES structure that will store the requested window information.

Description==

The GrGetWMProperties function retrieves window management data about the specified window, including property flags and bits, the window title, background color and border size. If the GrGetWMProperties function cannot find a window with the number wid, it sets the returned flags to zero and the remainder of the information is undefined.   **/
void		GrGetWMProperties(GR_WINDOW_ID wid, GR_WM_PROPERTIES *props);

/**  **/
GR_FONT_ID	GrCreateFont(GR_CHAR *name, GR_COORD height,
			GR_LOGFONT *plogfont);

/**  **/
void		GrSetFontSize(GR_FONT_ID fontid, GR_COORD size);
/**  **/
void		GrSetFontRotation(GR_FONT_ID fontid, int tenthsdegrees);
/**  **/
void		GrSetFontAttr(GR_FONT_ID fontid, int setflags, int clrflags);
/**  **/
void		GrDestroyFont(GR_FONT_ID fontid);

/**Parameters==

font-->

    The font number.
fip-->

    A pointer to a GR_FONT_INFO structure that will store the requested font information.

Description==

The GrGetFontInfo function retrieves information about the specified font, including the font number, the height of the font, the maximum width of any character in the font, the height of the baseline, a flag indicating whether or not the font is fixed-width, and a table of the individual widths of each character in the font. If the GrGetFontInfo function cannot find a font with the number font, it sets the returned font number to zero and the remainder of the information is undefined.  **/
void		GrGetFontInfo(GR_FONT_ID font, GR_FONT_INFO *fip);

/**  **/
GR_WINDOW_ID	GrGetFocus(void);

/**Parameters==

wid-->

    The window ID of the window to get focus.

Description==

The GrSetFocus function sets the focus to the wid window. When you set the focust to a window, only that window (or its children) will receive keyboard events. If you set the focus to the root window, then the input focus follows the pointer.   **/
void		GrSetFocus(GR_WINDOW_ID wid);

/**Parameters==

wid-->

    The window ID of the window for which you want to change the border color.
color-->

    The new color for the window border.

Description==

The GrSetBorderColor function sets the border color of the specified window.   **/
void		GrSetBorderColor(GR_WINDOW_ID wid, GR_COLOR color);

/**Parameters==

wid-->

    The window id of the window you want to clear.
exposeflag

    If you set exposeflag to a nonzero value, the GrClearWindot function generates an exposure event for the window.

Description==

The GrClearWindow function clears the wid window by filling the entire drawable area of the window with its background color. If you set exposeflag to a nonzero value, the window will also receive a GR_EVENT_TYPE_EXPOSE event.
  **/
void		GrClearWindow(GR_WINDOW_ID wid, GR_BOOL exposeflag);

/** Parameters==

wid-->

    The window id of the window from which this client wants events.
eventmask-->

    The bitmask of the events.

Description==

The GrSelectEvents function tells the server which events the client want to receive for the wid window. The GrSelectEvents replaces any previously selected event mask for the window. **/
void		GrSelectEvents(GR_WINDOW_ID wid, GR_EVENT_MASK eventmask);

/** Wait for and retrieve the next event from the event queue
Parameters==

ep-->

    A pointer to a GR_EVENT structure that will hold the event returned from the queue.

Description==

The GrGetNextEvent function flushes any buffered graphics commands and returns the next event from the event queue and waits for the event, if necessary. If a graphics error occurs, the server calls the error handler at this point. The GR_EVENT is a union of all the possible events. The type field of the union indicates which of the possible events took place, and then you can check the correct element of the union to access that particular event type's data.   **/
void		GrGetNextEvent(GR_EVENT *ep);

/**  **/
void		GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout);

/** Get the next event from the event queue
Parameters==

ep-->

    A pointer to a GR_EVENT structure that will hold the event returned from the queue.

Description==

The GrCheckNextEvent function flushes any buffered graphics commands and returns the next event from the event queue if one exists. If no event exists, the GrCheckNextEvent function returns GR_EVENT_TYPE_NONE. **/
void		GrCheckNextEvent(GR_EVENT *ep);

/**Copy the next event from the event queue
Parameters==

ep-->

    A pointer to a GR_EVENT structure that will hold the event returned from the queue.

Return Value==

Returns 1 if an event exists in the queue and zero if the queue is empty.
Description==

The GrPeekEvent function flushes any buffered graphics commands and returns the next event from the event queue, if one exists, and leaves the event on the queue. If no event exists, the GrPeekEvent function returns GR_EVENT_TYPE_NONE. **/
int		GrPeekEvent(GR_EVENT *ep);

/** Draw a line
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the line.
x1, y1-->

    The first point of the line segment.
x2, y2-->

    The last point of the line segment.

Description==

The GrLine function draws a line from the first point to the second point using the settings in the gc grapics context.
 **/
void		GrLine(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x1, GR_COORD y1,
			GR_COORD x2, GR_COORD y2);

/**Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the point.
x, y-->

    The location to draw the point.

Description==

The GrPoint function draws a point at the specified coordinates (x, y) using the settings in the gc grapics context.  **/
void		GrPoint(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y);

/** Draw a set of points
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the points.
count-->

    The number of points in the pointtable.
pointtable-->

    An array of GR_POINT point.

Description==

The GrPoints function draws count points at the coordinates listed in the pointtable array using the settings in the gc grapics context. **/
void		GrPoints(GR_DRAW_ID id, GR_GC_ID gc, GR_COUNT count,
			GR_POINT *pointtable);

/**Draw the perimeter of a rectangle
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the rectangle.
x, y-->

    The upper left corner of the rectangle relative to the drawable area.
widht, height-->

    The dimensions of the rectangle.

Description==

The GrRect function draws the perimeter of a rectangle with its upper left corner at ( x, y) with dimensions width and height. **/
void		GrRect(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height);

/** Draw a filled rectangle
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the rectangle.
x, y-->

    The upper left corner of the rectangle relative to the drawable area.
widht, height-->

    The dimensions of the rectangle.

Description==

The GrFillRect function draws a filled rectangle (including the permiter that would draw with the GrRect function). The new rectangle has its upper left corner at ( x, y) and dimensions of width and height.**/
void		GrFillRect(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height);

/**Draw the perimeter of a polygon
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the point.
count-->

    The number of points in the polygon.
pointtable-->

    An array of GR_POINT coordinates.

Description==

The GrPoly function draws a polygon defined by the pointtabel array. To draw a closed polygon, you must set the first and last points to the same coordinates. **/
void		GrPoly(GR_DRAW_ID id, GR_GC_ID gc, GR_COUNT count,
			GR_POINT *pointtable);

/** Draw a filled polygon
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the point.
count-->

    The number of points in the polygon.
pointtable-->

    An array of GR_POINT coordinates.

Description==

The GrFillPoly function draws a polygon defined by the pointtabel array. Unlike the GrPoly function, where you must set the first and last points to the same coordinates to get a closed shape, the GrFillPoly function fills in the closed shape including an assumed line from the last point to the first, if necessary.**/
void		GrFillPoly(GR_DRAW_ID id, GR_GC_ID gc, GR_COUNT count,
			GR_POINT *pointtable);

/**Draw the perimeter of an ellipse
Description==
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the ellipse.
x, y-->

    The center of the ellipse relative to the drawable area.
rx, ry-->

    The radii of the ellipse.
The GrEllipse function draws the perimeter of the ellipse with its center at ( x, y) and horizontal radius of rx and a vertical radius of ry.**/
void		GrEllipse(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE rx, GR_SIZE ry);

/**  Draw a filled ellipse
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the ellipse.
x, y-->

    The center of the ellipse relative to the drawable area.
rx, ry-->

    The radii of the ellipse.

Description==

The GrFillEllipse function draws a filled ellipse with its center at ( x, y) and horizontal radius of rx and a vertical radius of ry.**/
void		GrFillEllipse(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x,
			GR_COORD y, GR_SIZE rx, GR_SIZE ry);

/**Draw an arc or pie
id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the arc.
x, y-->

    The center of the arc relative to the drawable area.
rx, ry-->

    The radii of the arc.
ax, ay-->

    The first point on the arc to draw.
bx, by-->

    The last point on the arc to draw.
type-->

    The method of drawinnng the arc. The type parameter has the following acceptable values:

    ==Value	Description==
    -->MWARC	Draws the arc
    -->MWARCOUTLINE	Draws the arc and the radii to the endpoints
    -->MWPIE	Draws the arc filled with the foreground color
The GrArc function draws an arc with its center at (x, y) and horizontal radius of rx and a vertical radius of ry. The function starts drawing the arc from (ax, ay) and draws the arc to (bx, by). When specifying the points on the angle, rember to state these points relative to the center point and that the positive values move away from the upper left corner of the window.**/
void		GrArc(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE rx, GR_SIZE ry, GR_COORD ax, GR_COORD ay,
			GR_COORD bx, GR_COORD by, int type);

/** Draw an arc or pie
The GrArcAngle function draws an arc with its center at (x, y) and horizontal radius of rx and a vertical radius of ry. The function drawing the arc from angle1 to angle2.

    Note: You must have a platform with a floating point processor to use the GrArcAngle function. If you do not have a floating point processor, Use the GrArc function instead.**/
void		GrArcAngle(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE rx, GR_SIZE ry, GR_COORD angle1,
			GR_COORD angle2, int type); /* floating point required*/

/** Set the foreground color in a graphics context
Parameters==

gc-->

    The graphics context ID of the graphics context for which you want to change the foreground color.
color-->

    The new foreground color for the graphics context.

Description==

The GrSetGCForeground function sets the foreground color of the specified graphics context. **/
void		GrSetGCForeground(GR_GC_ID gc, GR_COLOR foreground);

/** Set the background color in a graphics context
Parameters==

gc-->

    The graphics context ID of the graphics context for which you want to change the background color.
color-->

    The new background color for the graphics context.

Description==

The GrSetGCBackground function sets the background color of the specified graphics context.  **/
void		GrSetGCBackground(GR_GC_ID gc, GR_COLOR background);
/**  **/
void		GrSetGCUseBackground(GR_GC_ID gc, GR_BOOL flag);

/** Set the drawing mode for a graphics context
Parameters==

gc-->

    The graphics context ID of the graphics context for which you want to change the drawing mode.
mode-->

    The new drawing mode for the graphics context. The mode parameter has the following acceptable values: GR_MODE_SET, GR_MODE_XOR, GR_MODE_AND, GR_MODE_OR.

Description==

The GrSetGCMode function sets the foreground color of the specified graphics context.  **/
void		GrSetGCMode(GR_GC_ID gc, int mode);
/** Set the font used in a graphics context
Parameters

gc

    The graphics context ID of the graphics context for which you want to set the font.
font

    The number of the font to use for the graphics context.

Description

The GrSetGCFont function sets the font used for text drawing in the specified graphics context. **/
void		GrSetGCFont(GR_GC_ID gc, GR_FONT_ID font);

/** Return the size of a text string for a particular graphics context
Parameters==

gc-->

    The graphics context ID number.
cp-->

    A text string.
len-->

    The number of characters in the cp string.
retwidth-->

    The returned width of the string.
retheight-->

    The returned height of the string.
retbase-->

    The returned height of the baseline above the bottom of the string.

Description==

The GrGetGCTextSize function calculates the size of the first len charecters of the cp string using the font characteristic specified in the gc graphics context. All returned sizes measure pixels. **/
void		GrGetGCTextSize(GR_GC_ID gc, void *str, int count, int flags,
			GR_SIZE *retwidth, GR_SIZE *retheight,GR_SIZE *retbase);

/** Create a bitmap from the content of a drawable area
Parameters==

id-->

    The ID of the window or other drawable area.
x, y-->

    The location in the drawable area to draw the upper left corner of bitmap.
widht, height-->

    The dimensions of the bitmap in pixels.
colortable-->

    A pointer to a packed pixel bitmap large enough to hold the area from the screen.

Description==

The GrReadArea function reads the color values from a specified rectangular area into a buffer. If other windows obscure the drawable area id, then the returned values inclued the values of the covering windows. Regions read outside the screen boundaries or from unmapped windows get set to black.
 **/
void		GrReadArea(GR_DRAW_ID id, GR_COORD x, GR_COORD y, GR_SIZE width,
			GR_SIZE height, GR_PIXELVAL *pixels);

/** Draw a multiple-bit-per-pixel bitmap
draws a bitmap at the (x, y) coordinates of the drawable area using the gc graphics context. Each pixel in the drawable area gets set to the corresponding color value from the bitmap. The pixtype parameter describes the bit depth of the bitmap.
Parameters==========

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the bitmap.
x, y-->

    The location in the drawable area to draw the upper left corner of bitmap.
widht, height-->

    The dimensions of the bitmap in pixels.
pixels-->

    A pointer to a packed pixel bitmap.
pixtype-->

    One of the following acceptable bitmap types:

    Type	Bitmap Description
    MWPF_RGB	Packed 32-bit RGB
    MWPF_PIXELVAL	Packed PIXELVAL
    MWPF_PALETTE	Packed 8 bits, 1, 4, or 8 pallette index
    MWPF_TRUECOLOR0888	Packed 32 bits 8/8/8 truecolor
    MWPF_TRUECOLOR888	Packed 24 bits 8/8/8 truecolor
    MWPF_TRUECOLER565	Packed 16 bits 5/6/5 truecolor
    MWPF_TRUECOLOR332	Packed 8 bits 3/3/2 truecolor
If you set the usebackround mode for the graphics context, then the GrArea function draws pixels with color values equal to the graphics context background color; otherwise, the function does not draw pixels with color values equal to the background color of the graphics context. **/
void		GrArea(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE width,GR_SIZE height,void *pixels,int pixtype);

/**  **/
void            GrCopyArea(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height, GR_DRAW_ID srcid,
			GR_COORD srcx, GR_COORD srcy, int op);

/**  Draw a bitmap
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the bitmap.
x, y-->

    The location in the drawable area to draw the upper left corner of bitmap.
widht, height-->

    The dimensions of the bitmap in pixels.
bitmaptable-->

    A pointer to a one-bit-per-pixel bitmap.

Description==

The GrBitmap function draws a bitmap at the (x, y) coordinates of the drawable area using the gc graphics context. One bits in bitmaptable get drawn with the graphics context foreground color. If you set the usebackround mode for the graphics context, then zero bits in the bitmap get draw with the graphics context background color.**/
void		GrBitmap(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height, GR_BITMAP *imagebits);

/**  **/
void		GrDrawImageBits(GR_DRAW_ID id,GR_GC_ID gc,GR_COORD x,GR_COORD y,
			GR_IMAGE_HDR *pimage);

/**  **/
void		GrDrawImageFromFile(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x,
			GR_COORD y, GR_SIZE width, GR_SIZE height,
			char *path, int flags);

/**  **/
GR_IMAGE_ID	GrLoadImageFromFile(char *path, int flags);

/**  **/
void		GrDrawImageToFit(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x,
			GR_COORD y, GR_SIZE width, GR_SIZE height,
			GR_IMAGE_ID imageid);

/**  **/
void		GrFreeImage(GR_IMAGE_ID id);

/**  **/
void		GrGetImageInfo(GR_IMAGE_ID id, GR_IMAGE_INFO *iip);
/** Draw a text string
Parameters==

id-->

    The ID of the window or other drawable area.
gc-->

    The graphics context to use when drawing the rectangle.
x, y-->

    The location in the drawable area to begin drawing the string.
str-->

    A pointer to a string.
count-->

    The number of characters in the str string.

Description==

The GrText function draws a text string beginning at the coordinates (x, y) using the gc graphics context.
 **/
void		GrText(GR_DRAW_ID id, GR_GC_ID gc, GR_COORD x, GR_COORD y,
			void *str, GR_COUNT count, int flags);

/** Set the cursor for a window
Parameters==

wid-->

    The window ID of the window for which you want to set the cursor.
width, height-->

    The dimensions of the cursor.
hotx, hoty-->

    The position of the hot spot within the cursor relative to the upper left corner of the cursor.
foreground, background-->

    The foreground and background colors of the cursor.
fgbitmap, bgbitmap-->

    Pointers to the foreground and background bitmaps.

Description==

The GrSetCursor function sets a new cursor for a window. The server will display the cursor only in the wid window and its children, unless the children set a different cursor.

If the fgbitmap and the bgbitmap both set a bit at the same position, the foreground gets precedence.  **/
void		GrSetCursor(GR_WINDOW_ID wid, GR_SIZE width, GR_SIZE height,
			GR_COORD hotx, GR_COORD hoty, GR_COLOR foreground,
			GR_COLOR background, GR_BITMAP *fbbitmap,
			GR_BITMAP *bgbitmap);

/**Move the cursor to specified coordinates
x, y-->

    The absolute coordinates of the new cursor location.

Description==

The GrMoveCursor function moves the cursor to the coordinates (x, y) relative to the root window. The coordinates specify the new location of the cursor's hot spot, and the server sets the cursor's appearance to the cursor for the visible window at the new coordinates.  **/
void		GrMoveCursor(GR_COORD x, GR_COORD y);
/**  **/
void		GrGetSystemPalette(GR_PALETTE *pal);
/**  **/
void		GrSetSystemPalette(GR_COUNT first, GR_PALETTE *pal);
/**  **/
void		GrFindColor(GR_COLOR c, GR_PIXELVAL *retpixel);
/**  **/
void		GrReqShmCmds(long shmsize);
/**  **/
void		GrInjectPointerEvent(MWCOORD x, MWCOORD y,
			int button, int visible);
/**  **/
void		GrInjectKeyboardEvent(GR_WINDOW_ID wid, GR_KEY keyvalue,
			GR_KEYMOD modifiers, GR_SCANCODE scancode,
			GR_BOOL pressed);
/**  **/
void		GrCloseWindow(GR_WINDOW_ID wid);
/**  **/
void		GrKillWindow(GR_WINDOW_ID wid);
/**  **/
void		GrSetScreenSaverTimeout(GR_TIMEOUT timeout);
/**  **/
void		GrSetSelectionOwner(GR_WINDOW_ID wid, GR_CHAR *typelist);
/**  **/
GR_WINDOW_ID	GrGetSelectionOwner(GR_CHAR **typelist);
/**  **/
void		GrRequestClientData(GR_WINDOW_ID wid, GR_WINDOW_ID rid,
			GR_SERIALNO serial, GR_MIMETYPE mimetype);
/**  **/
void		GrSendClientData(GR_WINDOW_ID wid, GR_WINDOW_ID did,
			GR_SERIALNO serial, GR_LENGTH len, GR_LENGTH thislen,
			void *data);
/**  **/
void		GrBell(void);
/**  **/
void		GrSetBackgroundPixmap(GR_WINDOW_ID wid, GR_WINDOW_ID pixmap,
			int flags);
/**  **/
void		GrRegisterInput(int fd);
/**  **/
void		GrMainLoop(GR_FNCALLBACKEVENT fncb);
/** Specify the error handling function
Parameters==

func-->

    The function you want to set as the error handler

Return Value

Returns the previous error hander.
Description==

The GrSetErrorHandler function sets an error handling function that the server calls whenever an error occurs while the client has requested an event.

If you set func to zero, the server then calls a default routine, GrDefaultErrorHandler.

When an error occurs, the server calls the error handling function is called with the parameters GR_ERROR, GR_FUNC_NAME, and GR_ID. These are the error code, the name of the function which failed, and a resource id, which is set to zero if there is no meaningful resource ID. The error routine can return if desired, but without corrective action new errors will probably occur.  **/
GR_FNCALLBACKEVENT GrSetErrorHandler(GR_FNCALLBACKEVENT fncb);

/** The default error handler
err-->

    The error the handler should process

Description==

When the server reports an error and the client has not set an error handler for error events (with GrSetErrorHandler), the GrDefaultErrorHandler function handles error events. **/
void		GrDefaultErrorHandler(GR_EVENT *ep);

/* passive library entry points - available with client/server only*/
/**  **/
void		GrPrepareSelect(int *maxfd,void *rfdset);
/**  **/
void		GrServiceSelect(void *rfdset, GR_FNCALLBACKEVENT fncb);

/* nxutil.c - utility routines*/
/**  **/
GR_WINDOW_ID	GrNewWindowEx(GR_WM_PROPS props, GR_CHAR *title,
			GR_WINDOW_ID parent, GR_COORD x, GR_COORD y,
			GR_SIZE width, GR_SIZE height, GR_COLOR background);
/**  **/
void		GrDrawLines(GR_DRAW_ID w, GR_GC_ID gc, GR_POINT *points,
			GR_COUNT count);
/**  **/
GR_WINDOW_ID    GrNewPixmapFromData(GR_SIZE width, GR_SIZE height,
			GR_COLOR foreground, GR_COLOR background, void * bits,
			int flags);

/* useful function macros*/
#define GrSetWindowBackgroundColor(wid,color) \
		{	GR_WM_PROPERTIES props;	\
			props.flags = GR_WM_FLAGS_BACKGROUND; \
			props.background = color; \
			GrSetWMProperties(wid, &props); \
		}
#define GrSetWindowBorderSize(wid,width) \
		{	GR_WM_PROPERTIES props;	\
			props.flags = GR_WM_FLAGS_BORDERSIZE; \
			props.bordersize = width; \
			GrSetWMProperties(wid, &props); \
		}
#define GrSetWindowBorderColor(wid,color) \
		{	GR_WM_PROPERTIES props;	\
			props.flags = GR_WM_FLAGS_BORDERCOLOR; \
			props.bordercolor = color; \
			GrSetWMProperties(wid, &props); \
		}
#define GrSetWindowTitle(wid,name) \
		{	GR_WM_PROPERTIES props;	\
			props.flags = GR_WM_FLAGS_TITLE; \
			props.title = (GR_CHAR *)name; \
			GrSetWMProperties(wid, &props); \
		}

#ifdef __cplusplus
}
#endif

/* RTEMS requires rtems_main()*/
#if __rtems__
#define main	rtems_main
#endif

#endif /* _NANO_X_H*/
