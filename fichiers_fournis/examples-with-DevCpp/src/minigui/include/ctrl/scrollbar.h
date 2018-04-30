/**
 * \file scrollbar.h
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
 * $Id: scrollbar.h,v 1.2 2005/02/15 05:00:08 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_SCROLLBAR_H
#define _MGUI_CTRL_SCROLLBAR_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_scrollbar ScrollBar control
     *
     * \note The scrollbar is not implemented as a control in current version.
     * @{
     */

/**
 * \def CTRL_SCROLLBAR
 * \brief The class name of scrollbar control.
 */
#define CTRL_SCROLLBAR              ("scrollbar")

    /**
     * \defgroup ctrl_scrollbar_msgs Messages of scrollbar control
     * @{
     */

#define SBM_SETPOS                  0xF0E0
#define SBM_GETPOS                  0xF0E1
#define SBM_SETRANGE                0xF0E2
#define SBM_SETRANGEREDRAW          0xF0E6
#define SBM_GETRANGE                0xF0E3
#define SBM_ENABLE_ARROWS           0xF0E4
#define SBM_SETSCROLLINFO           0xF0E9
#define SBM_GETSCROLLINFO           0xF0EA

#define SBM_MSGMAX                  0xF0F0

    /** @} end of ctrl_scrollbar_msgs */

    /** @} end of ctrl_scrollbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCROLLBAR_H */

