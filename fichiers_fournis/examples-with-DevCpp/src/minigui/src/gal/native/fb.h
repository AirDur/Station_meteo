/*
** $Id: fb.h,v 1.6 2003/08/01 09:50:42 weiym Exp $
**
** fb.h: Framebuffer drivers header file for MiniGUI Screen and QVFB Drivers
**
** Copyright (C) 2003 Feynman Software
*/

#ifndef GUI_GAL_FB_H
    #define GUI_GAL_FB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* subdriver entry points: one required for each draw function*/
typedef struct {
	int	      (*Init)(PSD psd);
	void 	  (*DrawPixel)(PSD psd, int x, int y, gal_pixel c);
	gal_pixel (*ReadPixel)(PSD psd, int x, int y);

	void	  (*DrawHLine)(PSD psd, int x, int y, int w, gal_pixel c);
	void	  (*DrawVLine)(PSD psd, int x, int y, int w, gal_pixel c);
	void 	  (*Blit)(PSD dstpsd, int dstx, int dsty, int w, int h, PSD srcpsd, int srcx, int srcy);
	void	  (*PutBox)( PSD psd, int x, int y, int w, int h, void* buf );
	void	  (*GetBox)( PSD psd, int x, int y, int w, int h, void* buf );
	void	  (*PutBoxMask)( PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx);
	void 	  (*CopyBox)(PSD psd,int x1, int y1, int w, int h, int x2, int y2);
} SUBDRIVER, *PSUBDRIVER;

/* scr_fb.c*/
void ioctl_getpalette (int start, int len, short *red, short *green,short *blue);
void ioctl_setpalette (int start, int len, short *red, short *green,short *blue);

/* fb.c*/
PSUBDRIVER select_fb_subdriver(PSD psd);
int fb_mapmemgc(PSD mempsd,int w,int h,int planes,int bpp, int linelen,int size,void *addr);
int set_subdriver(PSD psd, PSUBDRIVER subdriver, int init);
void get_subdriver(PSD psd, PSUBDRIVER subdriver);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_GAL_FB_H */


