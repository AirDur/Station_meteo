
#define align_32_bits(b) (((b) + 3) & -4)

/************************* Icon support **************************************/
// Icon creating and destroying.
HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename, int which)
{
    FILE* fp;
    WORD wTemp;
    BYTE bTemp;
    int  w, h, colornum;
    DWORD size, offset;
    DWORD imagesize, imagew, imageh;
    BYTE* image;
    HICON icon;
    
    icon = 0;
    
    if( !(fp = fopen(filename, "rb")) ) return 0;

    fseek(fp, sizeof(WORD), SEEK_SET);

    // the cbType of struct ICONDIR.
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;

    // get ICON images count.
    wTemp = MGUI_ReadLE16FP (fp);
    if (which >= wTemp)
        which = wTemp - 1;
    if (which < 0)
        which = 0;

    // seek to the right ICONDIRENTRY if needed.
    if (which != 0)
        fseek (fp, SIZEOF_ICONDIRENTRY * which, SEEK_CUR);

    // cursor info, read the members of struct ICONDIRENTRY.
    w = fgetc (fp);       // the width of first cursor
    h = fgetc (fp);       // the height of first cursor
    if ((w%16) != 0 || (h%16) != 0) goto error;
    bTemp = fgetc (fp);   // the bColorCount
    if(bTemp != 2 && bTemp != 16) goto error;
    fseek(fp, sizeof(BYTE), SEEK_CUR); // skip the bReserved
    wTemp = MGUI_ReadLE16FP (fp);   // the wPlanes
    if(wTemp != 0) goto error;
    wTemp = MGUI_ReadLE16FP (fp);   // the wBitCount
    if(wTemp > 4) goto error;
    size = MGUI_ReadLE32FP (fp);
    offset = MGUI_ReadLE32FP (fp);

    // read the cursor image info.
    fseek(fp, offset, SEEK_SET);
    fseek(fp, sizeof(DWORD), SEEK_CUR); // skip the biSize member.
    imagew = MGUI_ReadLE32FP (fp);
    imageh = MGUI_ReadLE32FP (fp);
    // check the biPlanes member;
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;
    // check the biBitCount member;
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp > 4) goto error;
    colornum = (int)wTemp;
    fseek(fp, sizeof(DWORD), SEEK_CUR); // skip the biCompression members.

    if (colornum == 1)
        imagesize = align_32_bits(w>>3) * h;
    else
        imagesize = align_32_bits(w>>1) * h;

    imagesize += align_32_bits(w>>3) * h;
    fseek(fp, sizeof(DWORD), SEEK_CUR);

    // skip the rest members and the color table.
    fseek(fp, sizeof(DWORD)*4 + sizeof(BYTE)*(4<<colornum), SEEK_CUR);
    
    // allocate memory for image.
#ifdef HAVE_ALLOCA
    // Use alloca, the stack may be enough for the image.
    if ((image = (BYTE*) alloca (imagesize)) == NULL)
#else
    if ((image = (BYTE*) malloc (imagesize)) == NULL)
#endif
        goto error;

    // read image
    fread (image, imagesize, 1, fp);

    icon = CreateIcon (hdc, w, h, 
                    image + (imagesize - (align_32_bits(w>>3) * h)), image, colornum);

#ifndef HAVE_ALLOCA
    free (image);
#endif

error:
    fclose (fp);
    return icon;
}

HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which)
{
    Uint8* p;
    WORD wTemp;
    BYTE bTemp;

    int  w, h, colornum;
    DWORD size, offset;
    DWORD imagesize, imagew, imageh;
    
    p = (Uint8*)area;
    p += sizeof (WORD);

    // the cbType of struct ICONDIR.
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp != 1) goto error;

    // get ICON images count.
    wTemp = MGUI_ReadLE16Mem (&p);
    if (which >= wTemp)
        which = wTemp - 1;
    if (which < 0)
        which = 0;
   
    // seek to the right ICONDIRENTRY if needed.
    if (which != 0)
        p += SIZEOF_ICONDIRENTRY * which;

    // cursor info, read the members of struct ICONDIRENTRY.
    w = *p++;       // the width of first cursor
    h = *p++;       // the height of first cursor
    if ((w%16) != 0 || (h%16) != 0) goto error;
    bTemp = *p++;   // the bColorCount
    if(bTemp != 2 && bTemp != 16) goto error;
    // skip the bReserved
    p ++;
    wTemp = MGUI_ReadLE16Mem (&p);   // the wPlanes
//    if (wTemp != 0) goto error;
    wTemp = MGUI_ReadLE16Mem (&p);   // the wBitCount
//    if (wTemp > 4) goto error;
    size = MGUI_ReadLE32Mem (&p);
    offset = MGUI_ReadLE32Mem (&p);
    
    // read the cursor image info.
    p = (Uint8*)area + offset;
    // skip the bReserved
    p += sizeof (DWORD);
    imagew = MGUI_ReadLE32Mem (&p);
    imageh = MGUI_ReadLE32Mem (&p);
    // check the biPlanes member;
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp != 1) goto error;
    // check the biBitCount member;
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp > 4) goto error;
    colornum = (int)wTemp;
    // skip the biCompression members.
    p += sizeof(DWORD);
   
    if (colornum == 1)
        imagesize = align_32_bits(w>>3) * h;
    else
        imagesize = align_32_bits(w>>1) * h;

    imagesize += align_32_bits(w>>3) * h;
    p += sizeof(DWORD);

    // skip the rest members and the color table.
    p += sizeof(DWORD)*4 + sizeof(BYTE)*(4<<colornum);
    
    return CreateIcon (hdc, w, h, 
                    p + (imagesize - (align_32_bits(w>>3) * h)), p, colornum);

error:
    return 0;
}

HICON GUIAPI CreateIcon (HDC hdc, int w, int h, const BYTE* pAndBits, 
                        const BYTE* pXorBits, int colornum)
{
    PICON picon;
    int bpp, pitch;

    if ((w%16) != 0 || (h%16) != 0) return 0;

    bpp = GAL_BytesPerPixel (dc_HDC2PDC (hdc)->gc);
    pitch = bpp * w;

    // allocate memory.
    if (!(picon = (PICON)malloc(sizeof(ICON))) ) return 0;
    if (!(picon->AndBits = malloc (pitch * h)) ) {
        free (picon);
        return 0;
    }
    if( !(picon->XorBits = malloc (pitch * h)) ) {
        free (picon->AndBits);
        free (picon);
        return 0;
    }

    picon->width = w;
    picon->height = h;

    if(colornum == 1) {
        ExpandMonoBitmap (hdc, w, h, pAndBits, align_32_bits (w >> 3), MYBMP_FLOW_UP, 
                        picon->AndBits, pitch, 0, 0xFFFFFFFF);
        ExpandMonoBitmap (hdc, w, h, pXorBits, align_32_bits (w >> 3), MYBMP_FLOW_UP,
                        picon->XorBits, pitch, 0, 0xFFFFFFFF);
    }
    else if(colornum == 4) {
        ExpandMonoBitmap (hdc, w, h, pAndBits, align_32_bits (w >> 3), MYBMP_FLOW_UP, 
                        picon->AndBits, pitch, 0, 0xFFFFFFFF);
        Expand16CBitmap (hdc, w, h, pXorBits,  align_32_bits (w >> 1), MYBMP_FLOW_UP,
                        picon->XorBits, pitch, NULL);
    }

    return (HICON)picon;
}


BOOL GUIAPI DestroyIcon (HICON hicon)
{
    PICON picon;
    picon = (PICON)hicon;

    if (!picon)
        return FALSE;

    free (picon->AndBits);
    free (picon->XorBits);
    free (picon);
    return TRUE;
}

BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h)
{
    PICON picon;
    picon = (PICON)hicon;

    if (!picon)
        return FALSE;

    if (w) *w = picon->width;
    if (h) *h = picon->height;
    return TRUE;
}

