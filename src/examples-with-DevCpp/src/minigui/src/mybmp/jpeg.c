/*
** $Id: jpeg.c,v 1.9 2004/08/03 11:01:57 panweiguo Exp $
** 
** jpg.c: Low-level JPEG file read/save routines.
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming
**
** Current maintainer: Wei Yongming
**
** Create date: 2000/08/29
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * JPEG decompression routine
 *
 * JPEG support must be enabled (see README.txt in contrib/jpeg)
 *
 * SOME FINE POINTS: (from libjpeg)
 * In the below code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.doc for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.doc for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

#ifdef _JPG_FILE_SUPPORT

#include <jpeglib.h>
#include <jerror.h>

#define SIZEOF(object)       ((size_t) sizeof(object))

/* Expanded data source object for stdio input */
typedef struct {
  struct jpeg_source_mgr pub;        /* public fields */

  MG_RWops *infile;                /* source stream */
  JOCTET * buffer;                /* start of buffer */
  boolean start_of_file;        /* have we gotten any data yet? */
} my_source_mgr;

typedef my_source_mgr * my_src_ptr;

#define INPUT_BUF_SIZE  4096        /* choose an efficiently fread'able size */

/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

static void init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 */

static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

  nbytes = MGUI_RWread ((MG_RWops*) src->infile, src->buffer, 1, INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)        /* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 */

static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

static void term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}

/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

void my_jpeg_data_src (j_decompress_ptr cinfo, MG_RWops* infile)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {        /* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  SIZEOF(my_source_mgr));
    src = (my_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                  INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}

int __mg_load_jpg (MG_RWops *area, MYBITMAP* bmp, RGB* pal)
{
    int i;
    int ret = ERR_BMP_LOAD;    /* image load error*/
    unsigned char magic[5];

    /* This struct contains the JPEG decompression parameters
     * and pointers to working space 
     * (which is allocated as needed by the JPEG library).
     */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

#if 0
    if (area->type != RWAREA_TYPE_STDIO)
        return ERR_BMP_ERROR_SOURCE;
    
    /* first determine if JPEG file since decoder will error if not */
    fseek(fp, 0L, 0);
    if(!fread(magic, 2, 1, fp))
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/
    if (magic[0] != 0xFF || magic[1] != 0xD8)
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/
    fread(magic, 4, 1, fp);
    fread(magic, 4, 1, fp);
    if (strncmp(magic, "JFIF", 4) != 0)
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/

    fseek(fp, 0L, 0);
#else
    if (!MGUI_RWread (area, magic, 2, 1))
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/
    if (magic[0] != 0xFF || magic[1] != 0xD8)
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/
    MGUI_RWread (area, magic, 4, 1);
    MGUI_RWread (area, magic, 4, 1);
    magic [4] = '\0';
    if (strncmp(magic, "JFIF", 4) != 0 && strncmp(magic, "Exif", 4) != 0)
        return ERR_BMP_IMAGE_TYPE;        /* not JPEG image*/
    MGUI_RWseek (area, -10, SEEK_CUR);
#endif

    /* Step 1: allocate and initialize JPEG decompression object */

    /* We set up the normal JPEG error routines. */
    cinfo.err = jpeg_std_error (&jerr);

    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress (&cinfo);

    /* Step 2: specify data source */
    my_jpeg_data_src (&cinfo, area);

    /* Step 3: read file parameters with jpeg_read_header() */
    jpeg_read_header (&cinfo, TRUE);

    /* Step 4: set parameters for decompression */
    cinfo.out_color_space = (bmp->flags & MYBMP_LOAD_GRAYSCALE) ? JCS_GRAYSCALE: JCS_RGB;
    cinfo.quantize_colors = FALSE;

    if (!(bmp->flags & MYBMP_LOAD_GRAYSCALE)) {
        if (bmp->depth <= 8) {
            cinfo.quantize_colors = TRUE;
    
            /* Get system palette */
            cinfo.actual_number_of_colors = 0x01 << bmp->depth;
    
            /* Allocate jpeg colormap space */
            cinfo.colormap = (*cinfo.mem->alloc_sarray)
                ((j_common_ptr) &cinfo, JPOOL_IMAGE,
                       (JDIMENSION)cinfo.actual_number_of_colors,
                (JDIMENSION)3);

            /* Set colormap from system palette */
            for(i = 0; i < cinfo.actual_number_of_colors; ++i)
            {
                cinfo.colormap[0][i] = pal[i].r >> 2;
                cinfo.colormap[1][i] = pal[i].g >> 2;
                cinfo.colormap[2][i] = pal[i].b >> 2;
            }
        }
    }
    else {
        /* Grayscale output asked */
        cinfo.quantize_colors = TRUE;
        cinfo.out_color_space = JCS_GRAYSCALE;
        cinfo.desired_number_of_colors = 256;
        for (i = 0; i < 256; i++) {
            pal [i].r = i;
            pal [i].g = i;
            pal [i].b = i;
        }
    }

    jpeg_calc_output_dimensions (&cinfo);

    ret = ERR_BMP_MEM;

    if (bmp->flags & MYBMP_LOAD_GRAYSCALE) {
        bmp->depth = 8;
    }
    else {
        bmp->flags |= MYBMP_TYPE_RGB;
        if (cinfo.output_components == 3)
            bmp->flags |= MYBMP_RGBSIZE_3;
        else if (cinfo.output_components == 4)
            bmp->flags |= MYBMP_RGBSIZE_4;
        else
            bmp->flags = MYBMP_TYPE_NORMAL;

        bmp->depth = cinfo.output_components * 8;
    }

    bmp->w = cinfo.output_width;
    bmp->h = cinfo.output_height;
    bmp->frames = 1;
    bmpComputePitch (bmp->depth, bmp->w, &bmp->pitch, TRUE);
    bmp->flags = MYBMP_TYPE_NORMAL | MYBMP_FLOW_DOWN;

    bmp->bits = malloc(bmp->pitch * bmp->h);
    if (!bmp->bits) goto err;

    /* Step 5: Start decompressor */
    jpeg_start_decompress (&cinfo);

    /* Step 6: while (scan lines remain to be read) */
    while (cinfo.output_scanline < cinfo.output_height) {
        JSAMPROW rowptr[1];
        BYTE* bits;
        bits = bmp->bits + cinfo.output_scanline * bmp->pitch;
        rowptr[0] = (JSAMPROW)(bits);
        jpeg_read_scanlines (&cinfo, rowptr, 1);
    }
    ret = ERR_BMP_OK;

err:
    /* Step 7: Finish decompression */
    jpeg_finish_decompress (&cinfo);

    /* Step 8: Release JPEG decompression object */
    jpeg_destroy_decompress (&cinfo);

    /* May want to check to see whether any corrupt-data
     * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
     */
    return ret;
}

BOOL __mg_check_jpg (MG_RWops* fp)
{
    unsigned char magic [5];

    if (!MGUI_RWread (fp, magic, 2, 1))
        return FALSE;        /* not JPEG image*/
    if (magic[0] != 0xFF || magic[1] != 0xD8)
        return FALSE;        /* not JPEG image*/
    MGUI_RWread (fp, magic, 4, 1);
    MGUI_RWread (fp, magic, 4, 1);
    magic [4] = '\0';
    if (strncmp(magic, "JFIF", 4) != 0 && strncmp(magic, "Exif", 4) != 0)
        return FALSE;        /* not JPEG image*/

    return TRUE;
}

#endif /* _JPG_FILE_SUPPORT */

