/**
 * \file edit.h
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
 * $Id: edit.h,v 1.4 2005/02/15 05:00:08 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_EDIT_H
#define _MGUI_CTRL_EDIT_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_edit Edit/MEdit control
     *
     * \bug You can not pass caption argument for multi-line edit control
     *      when you create it, just use null string:
     *
     * \code
     *  CreateWindowEx (CTRL_MEDIT, ..., "", ...);
     * \endcode
     *
     * @{
     */

/**
 * \def CTRL_EDIT
 * \brief The class name of simple single-line editor box.
 *
 * This edit control uses the system default fixed logical font.
 */
#define CTRL_EDIT           ("edit")

/**
 * \def CTRL_SLEDIT
 * \brief The class name of single-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font..
 */
#define CTRL_SLEDIT         ("sledit")

/**
 * \def CTRL_MLEDIT
 * \brief The class name of multiple-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font..
 */
#define CTRL_MLEDIT         ("mledit")

/**
 * \def CTRL_MEDIT
 * \brief Another class name of multiple-line editor box.
 *
 * This edit control uses the system logical font for control,
 * which may be variable-width font..
 */
#define CTRL_MEDIT          ("medit")

#define CTRL_OLDMEDIT       ("oldmedit")

    /**
     * \defgroup ctrl_edit_styles Styles of edit control
     * @{
     */

/**
 * \def ES_LEFT
 * \brief Left-aligns text.
 */
#define ES_LEFT             0x00000000L

#define ES_CENTER           0x00000001L
#define ES_RIGHT            0x00000002L
#define ES_MULTILINE        0x00000004L

/**
 * \def ES_UPPERCASE
 * \brief Converts all characters to uppercase as they are typed into the edit control.
 */
#define ES_UPPERCASE        0x00000008L

/**
 * \def ES_LOWERCASE
 * \brief Converts all characters to lowercase as they are typed into the edit control.
 */
#define ES_LOWERCASE        0x00000010L

/**
 * \def ES_PASSWORD
 * \brief Displays an asterisk (*) for each character typed into the edit control.
 */
#define ES_PASSWORD         0x00000020L

#define ES_AUTOVSCROLL      0x00000040L
#define ES_AUTOHSCROLL      0x00000080L

/**
 * \def ES_NOHIDESEL
 * \brief Edit control with this style will remain selected when focus is lost
 */
#define ES_NOHIDESEL        0x00000100L

/**
 * \def ES_AUTOSELECT
 * \brief Selects all text when getting focus
 */
#define ES_AUTOSELECT       0x00000400L
//#define ES_OEMCONVERT       0x00000400L

/**
 * \def ES_READONLY
 * \brief Prevents the user from typing or editing text in the edit control.
 */
#define ES_READONLY         0x00000800L

/**
 * \def ES_BASELINE
 * \brief Draws base line under input area instead of frame border.
 */
#define ES_BASELINE         0x00001000L

/**
 * \def ES_AUTOWRAP
 * \brief Automatically wraps against border when inputting.
 */
#define ES_AUTOWRAP         0x00002000L

/**
 * \def ES_TITLE
 * \brief Shows specified title texts.
 */
#define ES_TITLE            0x00004000L

/**
 * \def ES_TIP
 * \brief Shows specified tip texts.
 */
#define ES_TIP              0x00008000L

    /** @} end of ctrl_edit_styles */

    /**
     * \defgroup ctrl_edit_msgs Messages of edit control
     * @{
     */

/**
 * \def EM_GETSEL
 * \brief Gets the selected string in the edit control.
 *
 * \code
 * EM_GETSEL
 *
 * char *buffer;
 * int len;
 *
 * wParam = len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len Length of buffer.
 * \param buffer Pointer to the string buffer
 *
 * \return length of the selected string
 */
#define EM_GETSEL               0xF0B0

/**
 * \def EM_SETSEL
 * \brief Sets the selected point in the edit control and makes
 *        the text between insertion point and selection point selected.
 *
 *        Generally, you should send EM_SETCARETPOS first to set insertion
 *        point before you use EM_SETSEL to select text.
 *
 * \code
 * EM_SETSEL
 *
 * int line_pos;
 * int char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos line position of the selection point.
 *                 For single line editor, it is always zero.
 *                 Note : For multi-line editor, "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos character(wide character) position of the selection point.
 *
 * \return length of the selected string
 */
#define EM_SETSEL               0xF0B1
#define EM_SETSELECTION         EM_SETSEL

/**
 * \def EM_SELECTALL
 * \brief Selects all the texts, the same meaning as ctrl+a
 *        
 * \code
 * EM_SELECTALL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 */
#define EM_SELECTALL              0xF0B2

/**
 * \def EM_GETSELPOS
 * \brief Gets the position of the selection point.
 *
 * \code
 * EM_GETSELPOS
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the selection line position.
 *                 For single line editor, it is always zero.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Pointer to a integer buffer to save the selection character position.
 *
 * \return The string length of the text from the beginning to the selection point.
 */
#define EM_GETSELPOS              0xF0B3

/**
 * \def EM_INSERTCBTEXT
 * \brief Inserts the text in the clipboard to the current caret position
 *
 * \code
 * EM_INSERTCBTEXT
 * int len;
 * const char *string;
 *
 * wParam = len;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param len Length of string
 * \param string Pointer to the text string
 */
#define EM_INSERTCBTEXT           0xF0B4

/**
 * \def EM_COPYTOCB
 * \brief Copies the currently selected text to the clipboard
 *
 * \code
 * EM_COPYTOCB
 *
 * wParam = 0;
 * lParam = 0
 * \endcode
 *
 * \return Length of the text which is really copied to clipboard.
 */
#define EM_COPYTOCB               0xF0B5

/**
 * \def EM_CUTTOCB
 * \brief Cuts the currently selected text to the clipboard
 *
 * \code
 * EM_CUTTOCB
 *
 * wParam = 0;
 * lParam = 0
 * \endcode
 *
 * \return Length of the text which is really copied to clipboard.
 */
#define EM_CUTTOCB               0xF0B6

/**
 * \def EM_SETLFDISPCHAR
 * \brief Sets the char used to represent the line seperator.
 *        
 * In default case, the line sperator will not be shown.
 * If the char used to represent the line seperator is not zero,
 *    this char will be shown in place of line seperator.
 *
 * \code
 * EM_SETLFDISPCHAR
 * unsigned char ch;
 *
 * wParam = 0;
 * lParam = ch;
 * \endcode
 *
 * \param ch the char used to represent the line seperator
 */
#define EM_SETLFDISPCHAR          0xF0B7

/**
 * \def EM_SETLINESEP
 * \brief Sets the line seperator.
 *        
 * In default case, the line sperator is '\n'.
 *
 * \code
 * EM_SETLINESEP
 * unsigned char ch;
 *
 * wParam = 0;
 * lParam = ch;
 * \endcode
 *
 * \param ch the new line seperator
 */
#define EM_SETLINESEP             0xF0B8

//#define EM_GETRECT              0xF0B2
//#define EM_SETRECT              0xF0B3
//#define EM_SETRECTNP            0xF0B4
//#define EM_SCROLL               0xF0B5

/**
 * \def EM_GETCARETPOS
 * \brief Gets the position of the caret.
 *
 * \code
 * EM_GETCARETPOS
 * int* line_pos;
 * int* char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos Pointer to a integer buffer to save the caret line position.
 *                 For single line editor, it is always zero.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos Pointer to a integer buffer to save the caret character position.
 *
 * \return The string length of the text from the beginning to the caret pos.
 */
#define EM_GETCARETPOS          0xF0B9

/**
 * \def EM_SETCARETPOS
 * \brief Sets the position of the caret.
 *
 * \code
 * EM_SETCARETPOS
 * int line_pos;
 * int char_pos;
 *
 * wParam = (WPARAM)line_pos;
 * lParam = (LPARAM)char_pos;
 * \endcode
 *
 * \param line_pos The new caret line position. For single line editor, it will be ignored.
 *                 Note : Here "line" means a text string ended with a line
 *                 seperator, not a single text line in wrap mode. So, char_pos
 *                 means the character position in a text string.
 * \param char_pos The new caret character position.
 *
 * \return length of the string from the beginning to the caret position 
 *         on success, otherwise -1.
 */
#define EM_SETCARETPOS          0xF0BA
#define EM_SETINSERTION         EM_SETCARETPOS

//#define EM_SCROLLCARET          0xF0B9
//#define EM_GETMODIFY            0xF0BA
//#define EM_SETMODIFY            0xF0BB

/**
 * \def EM_GETLINECOUNT
 * \brief Gets the line number.
 *
 * \code
 * EM_GETLINECOUNT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Line number on success, otherwise -1.
 * \note Implemented for TextEdit control.
 */
#define EM_GETLINECOUNT         0xF0BC

/**
 * \def EM_GETLINEHEIGHT
 * \brief Gets the height of a line.
 *
 * \code
 * EM_GETLINEHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height value.
 * \note Implemented for TextEdit control.
 */
#define EM_GETLINEHEIGHT        0xF0BD

/**
 * \def EM_SETLINEHEIGHT
 * \brief Sets the height of a line.
 *
 * \code
 * EM_SETLINEHEIGHT
 *
 * wParam = height;
 * lParam = 0;
 * \endcode
 *
 * \return the old height value.
 * \note Implemented for TextEdit control.
 */
#define EM_SETLINEHEIGHT        0xF0BE


//#define EM_LINEINDEX            0xF0BD
//#define EM_GETTHUMB             0xF0BE

/* internal used now */
#define EM_LINESCROLL           0xF0BF
 
/**
 * \def EM_INSERTTEXT
 * \brief Inserts the specified text to the current caret position
 *
 * Normally used to input a long string.
 *
 * \code
 * EM_INSERTTEXT
 * int len;
 * const char *string;
 *
 * wParam = len;
 * lParam = (LPARAM)string;
 * \endcode
 *
 * \param len Length of string
 * \param string Pointer to the text string
 */
#define EM_INSERTTEXT           0xF0C0

#define EM_LINELENGTH           0xF0C1
#define EM_REPLACESEL           0xF0C2
#define EM_GETLINE              0xF0C4

/**
 * \def EM_LIMITTEXT
 * \brief Set text limit of an edit control.
 *
 * \code
 * EM_LIMITTEXT
 * int newLimit;
 *
 * wParam = (WPARAM)newLimit;
 * lParam = 0;
 * \endcode
 *
 * \param newLimit The new text limit of an edit control.
 */
#define EM_LIMITTEXT            0xF0C5

/**
 * \def EM_REDO
 * \brief Redo operation.
 *
 * \code
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 */
#define EM_REDO                 0xF0C6
//#define EM_CANUNDO              0xF0C6
 
/**
 * \def EM_UNDO
 * \brief Undo operation.
 *
 * \code
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 */
#define EM_UNDO                 0xF0C7

#define EM_FMTLINES             0xF0C8
#define EM_LINEFROMCHAR         0xF0C9
#define EM_SETTABSTOPS          0xF0CB

/**
 * \def EM_SETPASSWORDCHAR
 * \brief Defines the character that edit control uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_SETPASSWORDCHAR
 * char passwdChar;
 *
 * wParam = (WPARAM)passwdChar;
 * lParam = 0;
 * \endcode
 */
#define EM_SETPASSWORDCHAR      0xF0CC

#define EM_EMPTYUNDOBUFFER      0xF0CD
#define EM_GETFIRSTVISIBLELINE  0xF0CE

/**
 * \def EM_SETREADONLY
 * \brief Sets or removes the read-only style (ES_READONLY) in an edit control.
 *
 * \code
 * EM_SETREADONLY
 * int readonly;
 *
 * wParam = (WPARAM)readonly;
 * lParam = 0;
 * \endcode
 *
 * \param readonly Indicates whether the edit control is read-only:
 *      - Zero\n
 *        Not read-only.
 *      - Non zero\n
 *        Read-only.
 */
#define EM_SETREADONLY          0xF0CF

/**
 * \var typedef int (*ED_DRAWSEL_FUNC) (HWND hWnd, HDC hdc, int startx, int starty, const char* content, int len, int selout)
 * \brief Type of the edit control callback function on drawing selected strings.
 *
 * \param hWnd Handle of the edit control.
 * \param startx x value of the beginning drawing position.
 * \param starty y value of the beginning drawing position.
 * \param content the string which will be drawed.
 * \param len Length of the string which should be drawed by this callback.
 * \param selout Length of the selected string that have been drawed before calling this callback function.
 * 
 * \return Width of the outputed strings.
 */
typedef int (*ED_DRAWSEL_FUNC) (HWND, HDC, int, int, const char*, int, int);


/**
 * \def EM_SETDRAWSELECTFUNC
 * \brief Sets the callback function on drawing selected chars
 *
 * \code
 * EM_SETDRAWSELECTFUNC
 * ED_DRAWSEL_FUNC drawsel;
 *
 * wParam = 0;
 * lParam = (LPARAM)drawsel;
 * \endcode
 *
 * \param drawsel The callback function used to draw selected strings.
 */
#define EM_SETDRAWSELECTFUNC    0xF0D0

/**
 * \def EM_SETGETCARETWIDTHFUNC
 * \brief Sets the callback function on getting caret width
 *
 * \code
 * EM_SETGETCARETWIDTHFUNC
 * int (*get_caret_width) (HWND, int);
 *
 * wParam = 0;
 * lParam = (LPARAM)get_caret_width;
 * \endcode
 *
 * \param get_caret_width The callback function used to get caret width.
 *                        The window handle and the maximum caret width are passed as arguments.
 *
 * \return The desired caret width.
 */
#define EM_SETGETCARETWIDTHFUNC 0xF0D1

/*
#define EM_SETWORDBREAKPROC     0xF0D0
#define EM_GETWORDBREAKPROC     0xF0D1
*/

/**
 * \def EM_GETPASSWORDCHAR
 * \brief Returns the character that edit controls uses in conjunction with 
 * the ES_PASSWORD style.
 *
 * \code
 * EM_GETPASSWORDCHAR
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return The currently used password character
 */
#define EM_GETPASSWORDCHAR      0xF0D2

#define EM_SETLIMITTEXT         EM_LIMITTEXT

/**
 * \def ED_CARETSHAPE_LINE
 * \brief Line-shaped caret
 */
#define ED_CARETSHAPE_LINE      0

/**
 * \def ED_CARETSHAPE_BLOCK
 * \brief Block-shaped caret
 */
#define ED_CARETSHAPE_BLOCK     1

/**
 * \def EM_CHANGECARETSHAPE
 * \brief Changes the shape of the caret
 *
 * \code
 * EM_CHANGECARETSHAPE
 *
 * int caret_shape;
 * 
 * wParam = caret_shape;
 * lParam = 0;
 * \endcode
 *
 * \param caret_shape Shape index of the caret, can be ED_CARETSHAPE_LINE or ED_CARETSHAPE_BLOCK.
 *
 * \return The old create shape
 */
#define EM_CHANGECARETSHAPE     0xF0D3

/**
 * \def EM_REFRESHCARET
 * \brief Refresh caret of the edit control
 */
#define EM_REFRESHCARET         0xF0D4

/**
 * \def EM_ENABLECARET
 * \brief To enable or disable the input caret
 *
 * \code
 * EM_ENABLECARET
 *
 * BOOL bEnable;
 *
 * wParam = bEnable;
 * lParam = 0;
 * \endcode
 *
 * \param bEnable TRUE to enable caret.
 *
 * \return The previous caret enabled status.
 */
#define EM_ENABLECARET          0xF0D5

/*
#define EM_SETMARGINS           0xF0D3
#define EM_GETMARGINS           0xF0D4
#define EM_GETLIMITTEXT         0xF0D5
#define EM_POSFROMCHAR          0xF0D6
#define EM_CHARFROMPOS          0xF0D7
#define EM_SETIMESTATUS         0xF0D8
#define EM_GETIMESTATUS         0xF0D9

#define MEM_SCROLLCHANGE        0xF0DB
*/

#define MED_STATE_YES           0x0
#define MED_STATE_NOUP          0x1
#define MED_STATE_NODN          0x2
#define MED_STATE_NO            0x3

/**
 * \def EM_SETTITLETEXT
 * \brief Sets the title text displayed before content text. 
 *
 * \code
 * EM_SETTITLETEXT
 * const char *title;
 * int len;
 *
 * wParam = len;
 * lParam = (LPARAM)title;
 * \endcode
 *
 * \note Implemented for TextEdit control.
 */
#define EM_SETTITLETEXT         0xF0DC

/**
 * \def EM_GETTITLETEXT
 * \brief Gets the title text displayed before content text. 
 *
 * \code
 * EM_GETTITLETEXT
 * const char *buffer;
 * int len;
 *
 * wParam = len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len should be length of buffer minus 1, left space for '\\0'
 * \param buffer string buffer
 *
 *
 * \return length of title
 * \note Implemented for TextEdit control.
 */
#define EM_GETTITLETEXT         0xF0DD

/**
 * \def EM_SETTIPTEXT
 * \brief Sets the tip text displayed when content is empty. 
 *
 * \code
 * EM_SETTIPTEXT
 * const char *buffer;
 * int len;
 *
 * wParam = len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 */
#define EM_SETTIPTEXT           0xF0DE

/**
 * \def EM_GETTIPTEXT
 * \brief Gets the tip text displayed when content is empty. 
 *
 * \code
 * EM_GETTIPTEXT
 * const char *buffer;
 * int len;
 *
 * wParam = len;
 * lParam = (LPARAM)buffer;
 * \endcode
 *
 * \param len should be length of buffer minus 1, left space for '\\0'
 * \param buffer string buffer
 *
 * \return length of tip text
 */
#define EM_GETTIPTEXT           0xF0DF

#define EM_MSGMAX               0xF0E0

    /** @} end of ctrl_edit_msgs */

    /**
     * \defgroup ctrl_edit_ncs Notification codes of edit control
     * @{
     */

#define EN_ERRSPACE         255

/**
 * \def EN_CLICKED
 * \brief Notifies a click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user clicks
 * in an edit control.
 */
#define EN_CLICKED          0x0001

/**
 * \def EN_DBLCLK
 * \brief Notifies a double click in an edit control.
 *
 * An edit control sends the EN_CLICKED notification code when the user 
 * double clicks in an edit control.
 */
#define EN_DBLCLK           0x0002

/**
 * \def EN_SETFOCUS
 * \brief Notifies the receipt of the input focus.
 *
 * The EN_SETFOCUS notification code is sent when an edit control receives 
 * the input focus.
 */
#define EN_SETFOCUS         0x0100

/**
 * \def EN_KILLFOCUS
 * \brief Notifies the lost of the input focus.
 *
 * The EN_KILLFOCUS notification code is sent when an edit control loses 
 * the input focus.
 */
#define EN_KILLFOCUS        0x0200

/**
 * \def EN_CHANGE
 * \brief Notifies that the text is altered.
 *
 * An edit control sends the EN_CHANGE notification code when the user takes 
 * an action that may have altered text in an edit control.
 */
#define EN_CHANGE           0x0300

#define EN_UPDATE           0x0400

/**
 * \def EN_MAXTEXT
 * \brief Notifies reach of maximum text limitation.
 *
 * The EN_MAXTEXT notification message is sent when the current text 
 * insertion has exceeded the specified number of characters for the edit control.
 */
#define EN_MAXTEXT          0x0501

#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602

/**
 * \def EN_ENTER
 * \brief Notifies the user has type the ENTER key in a single-line edit control.
 */
#define EN_ENTER            0x0700

    /** @} end of ctrl_edit_ncs */

/* Edit control EM_SETMARGIN parameters */
/**
 * \def EC_LEFTMARGIN
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EC_LEFTMARGIN       0x0001
/**
 * \def EC_RIGHTMARGIN
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EC_RIGHTMARGIN      0x0002
/**
 * \def EC_USEFONTINFO
 * \brief Value of wParam. Specifies the margins to set.
 */
#define EC_USEFONTINFO      0xffff

/* wParam of EM_GET/SETIMESTATUS  */
/**
 * \def EMSIS_COMPOSITIONSTRING
 * \brief Indicates the type of status to retrieve.
 */
#define EMSIS_COMPOSITIONSTRING        0x0001

/* lParam for EMSIS_COMPOSITIONSTRING  */
/**
 * \def EIMES_GETCOMPSTRATONCE
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EIMES_GETCOMPSTRATONCE         0x0001
/**
 * \def EIMES_CANCELCOMPSTRINFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EIMES_CANCELCOMPSTRINFOCUS     0x0002
/**
 * \def EIMES_COMPLETECOMPSTRKILLFOCUS
 * \brief lParam for EMSIS_COMPOSITIONSTRING.
 */
#define EIMES_COMPLETECOMPSTRKILLFOCUS 0x0004

    /** @} end of ctrl_edit */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_EDIT_H */

