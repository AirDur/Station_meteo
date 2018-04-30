/**
 * \file textedit.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim
    Copyright (C) 2002-2005 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you are using MiniGUI for developing commercial, proprietary, or other
    software not covered by the GPL terms, you must have a commercial license
    for MiniGUI. Please see http://www.minigui.com/product/index.html for 
    how to obtain this. If you are interested in the commercial MiniGUI 
    licensing, please write to sales@minigui.com. 

 \endverbatim
 */

/*
 * $Id: textedit.h,v 1.2 2005/02/15 05:00:08 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_TEXTEDIT_H
#define _MGUI_CTRL_TEXTEDIT_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_textedit TextEdit control
     * @{
     */

/**
 * \def CTRL_TEXTEDIT
 * \brief The class name of textedit control.
 */
#define CTRL_TEXTEDIT           ("textedit")


/* text document/buffer object */
typedef GHANDLE HTEXTDOC;


    /**
     * \defgroup ctrl_textedit_styles Styles of textedit control
     * @{
     */

#define TES_BASELINE            ES_BASELINE
#define TES_AUTOWRAP            ES_AUTOWRAP
#define TES_TITLE               ES_TITLE

    /** @} end of ctrl_textedit_styles */

    /**
     * \defgroup ctrl_textedit_msgs Messages of textedit control
     * @{
     */

#define TEM_RESETCONTENT        0xF400
#define TEM_ADDLINE             0xF401

    /** @} end of ctrl_textedit_msgs */

    /**
     * \defgroup ctrl_textedit_ncs Notification codes of textedit control
     * @{
     */

#define TEN_CLICK               1


    /** @} end of ctrl_textedit_ncs */

    /** @} end of ctrl_textedit */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_TEXTEDIT_H */

