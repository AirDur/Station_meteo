/*
** $Id: endianrw.c,v 1.8 2003/09/04 03:46:47 weiym Exp $
**
** endianrw.c: 
**  Functions for dynamically reading and writing endian-specific values
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** The idea and some code come from SDL - Simple DirectMedia Layer
** Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/09/18
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
** TODO:
*/ 

#include <stdio.h>

#include "common.h"
#include "endianrw.h"

Uint16 MGUI_ReadLE16 (MG_RWops *src)
{
	Uint16 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE16(value));
}

Uint16 MGUI_ReadBE16 (MG_RWops *src)
{
	Uint16 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE16(value));
}

Uint32 MGUI_ReadLE32 (MG_RWops *src)
{
	Uint32 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE32(value));
}

Uint32 MGUI_ReadBE32 (MG_RWops *src)
{
	Uint32 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE32(value));
}

Uint64 MGUI_ReadLE64 (MG_RWops *src)
{
	Uint64 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE64(value));
}

Uint64 MGUI_ReadBE64 (MG_RWops *src)
{
	Uint64 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE64(value));
}

int MGUI_WriteLE16 (MG_RWops *dst, Uint16 value)
{
	value = ArchSwapLE16(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE16 (MG_RWops *dst, Uint16 value)
{
	value = ArchSwapBE16(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteLE32 (MG_RWops *dst, Uint32 value)
{
	value = ArchSwapLE32(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE32 (MG_RWops *dst, Uint32 value)
{
	value = ArchSwapBE32(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteLE64 (MG_RWops *dst, Uint64 value)
{
	value = ArchSwapLE64(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE64 (MG_RWops *dst, Uint64 value)
{
	value = ArchSwapBE64(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

Uint16 MGUI_ReadLE16FP (FILE *src)
{
	Uint16 value;

	fread (&value, (sizeof value), 1, src);
	return(ArchSwapLE16(value));
}

Uint32 MGUI_ReadLE32FP (FILE *src)
{
	Uint32 value;

	fread(&value, (sizeof value), 1, src);
	return(ArchSwapLE32(value));
}

int MGUI_WriteLE16FP (FILE *dst, Uint16 value)
{
	value = ArchSwapLE16(value);
	return(fwrite (&value, (sizeof value), 1, dst));
}

int MGUI_WriteLE32FP (FILE *dst, Uint32 value)
{
	value = ArchSwapLE32(value);
	return(fwrite (&value, (sizeof value), 1, dst));
}

