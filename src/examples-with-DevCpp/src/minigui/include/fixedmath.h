/**
 * \file fixedmath.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/12
 * 
 *  This file includes fixed point and three-dimension math routines.
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
 * $Id: fixedmath.h,v 1.23 2005/02/15 05:00:07 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 *
 *             Fix point math routins come from Allegro
 *             By Shawn Hargreaves and others.
 *             So thank for their great work and good license.
 *
 *             "Allegro is a gift-software" 
 *
 *         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 */

#ifndef _MGUI_FIXED_MATH_H
#define _MGUI_FIXED_MATH_H

#include <errno.h>
#include <math.h>

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _FIXED_MATH

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup fixed_math_fns Fixed point math functions
     * 
     * You know that the float point mathematics routines are very
     * expensive. If you do not want precision mathematics result, 
     * you can use fixed point. MiniGUI uses a double word (32-bit)
     * integer to represent a fixed point ranged from -32767.0 to 
     * 32767.0, and defines some fixed point mathematics routines for 
     * your application. Some GDI functions need fixed point 
     * math routines, like \a Arc.
     *
     * Example 1:
     * 
     * \include fixed_point.c
     *
     * Example 2:
     * 
     * \include fixedpoint.c
     * @{
     */

/**
 * \fn fixed fsqrt (fixed x)
 * \brief Returns the non-negative square root of a fixed point value.
 *
 * This function returns the non-negative square root of \a x.
 * It fails and sets errno to EDOM, if x is negative.
 *
 * \sa fhypot
 */
fixed fsqrt (fixed x);

/**
 * \fn fixed fhypot (fixed x, fixed y)
 * \brief Returns the Euclidean distance from the origin.
 * 
 * The function returns the \a sqrt(x*x+y*y). This is the length of 
 * the hypotenuse of a right-angle triangle with sides of length \a x and \a y, 
 * or the distance of the point \a (x,y) from the origin.
 *
 * \sa fsqrt
 */
fixed fhypot (fixed x, fixed y);

/**
 * \fn fixed fatan (fixed x)
 * \brief Calculates the arc tangent of a fixed point value.
 *
 * This function calculates the arc tangent of \a x; that is the value 
 * whose tangent is \a x.
 *
 * \return Returns the arc tangent in radians and the value is 
 *         mathematically defined to be between -PI/2 and PI/2 (inclusive).
 *
 * \sa fatan2
 */
fixed fatan (fixed x);

/**
 * \fn fixed fatan2 (fixed y, fixed x)
 * \brief Calclulates the arc tangent of two fixed point variables.
 *
 *  This function calculates the arc tangent of the two variables \a x and \a y.
 *  It is similar to calculating the arc tangent of \a y / \a x, except that 
 *  the signs of both arguments are used to determine the quadrant of the result.
 *
 * \return Returns the result in radians, which is between -PI and PI (inclusive).
 *
 * \sa fatan
 */
fixed fatan2 (fixed y, fixed x);

extern fixed _cos_tbl[];
extern fixed _tan_tbl[];
extern fixed _acos_tbl[];

/************************** inline fixed point math functions *****************/
/* ftofix and fixtof are used in generic C versions of fmul and fdiv */

/**
 * \fn fixed ftofix (double x)
 * \brief Converts a float point value to a fixed point value.
 *
 * This function converts the specified float point value \a x to 
 * a fixed point value.
 *
 * \note The float point should be ranged from -32767.0 to 32767.0.
 * If it runs out of the range, this function sets \a errno to \a ERANGE.
 *
 * \sa fixtof
 */
static inline fixed ftofix (double x)
{ 
   if (x > 32767.0) {
      errno = ERANGE;
      return 0x7FFFFFFF;
   }

   if (x < -32767.0) {
      errno = ERANGE;
      return -0x7FFFFFFF;
   }

   return (long)(x * 65536.0 + (x < 0 ? -0.5 : 0.5)); 
}

/**
 * \fn double fixtof (fixed x)
 * \brief Converts a fixed point value to a float point value.
 *
 * This function converts the specified fixed point value \a x to 
 * a float point value.
 *
 * \sa ftofix
 */
static inline double fixtof (fixed x)
{ 
   return (double)x / 65536.0; 
}


/**
 * \fn fixed fadd (fixed x, fixed y)
 * \brief Returns the sum of two fixed point values.
 *
 * This function adds two fixed point values \a x and \a y, and
 * returns the sum.
 *
 * \param x x,y: Two addends.
 * \param y x,y: Two addends.
 * \return The sum. If the result runs out of range of fixed point, this function
 *         sets \a errno to \a ERANGE.
 *
 * \sa fsub
 */
static inline fixed fadd (fixed x, fixed y)
{
   fixed result = x + y;

   if (result >= 0) {
      if ((x < 0) && (y < 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFF;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y > 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFF;
      }
      else
	 return result;
   }
}

/**
 * \fn fixed fsub (fixed x, fixed y)
 * \brief Subtract a fixed point value from another.
 *
 * This function subtracts the fixed point values \a y from the fixed point value \a x,
 * and returns the difference.
 *
 * \param x The minuend.
 * \param y The subtrahend.
 * \return The difference. If the result runs out of range of fixed point, this function
 *         sets \a errno to \a ERANGE.
 *
 * \sa fadd
 */
static inline fixed fsub (fixed x, fixed y)
{
   fixed result = x - y;

   if (result >= 0) {
      if ((x < 0) && (y > 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFF;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y < 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFF;
      }
      else
	 return result;
   }
}

/**
 * \fn fixed fmul (fixed x, fixed y)
 * \brief Returns the product of two fixed point values.
 * 
 * This function returns the product of two fixed point values \a x and \a y.
 *
 * \param x The faciend.
 * \param y The multiplicato.
 * \return The prodcut. If the result runs out of range of fixed point, this function
 *         sets \a errno to \a ERANGE.
 * 
 * \sa fdiv
 */
static inline fixed fmul (fixed x, fixed y)
{
   return ftofix(fixtof(x) * fixtof(y));
}

/**
 * \fn fixed fdiv (fixed x, fixed y)
 * \brief Returns the quotient of two fixed point values.
 * 
 * This function returns the quotient of two fixed point values \a x and \a y.
 *
 * \param x The dividend.
 * \param y The divisor.
 * \return The quotient. If the result runs out of range of fixed point, this function
 *         sets \a errno to \a ERANGE.
 * 
 * \sa fmul
 */
static inline fixed fdiv (fixed x, fixed y)
{
   if (y == 0) {
      errno = ERANGE;
      return (x < 0) ? -0x7FFFFFFF : 0x7FFFFFFF;
   }
   else
      return ftofix(fixtof(x) / fixtof(y));
}

/**
 * \fn int fceil (fixed x)
 * \brief Rounds a fixed point value to the nearest integer.
 *
 * This function rounds the fixed point value \a x to the nearest integer
 * and returns it.
 *
 * \return The rounded integer value.
 */

static inline int fceil (fixed x)
{
   x += 0xFFFF;
   if (x >= 0x80000000) {
      errno = ERANGE;
      return 0x7FFF;
   }

   return (x >> 16);
}

/**
 * \fn fixed itofix (int x)
 * \brief Converts an integer to a fixed point value.
 *
 * This function converts the integer \a x to a fixed point value.
 *
 * \sa fixtoi
 */
static inline fixed itofix (int x)
{ 
   return x << 16;
}

/**
 * \fn int fixtoi (fixed x)
 * \brief Converts an fixed point value to an integer.
 *
 * This function converts the fixed point \a x to an integer.
 *
 * \sa itofix
 */
static inline int fixtoi (fixed x)
{ 
   return (x >> 16) + ((x & 0x8000) >> 15);
}

/**
 * \fn fixed fcos (fixed x)
 * \brief Returns the cosine of a fixed point.
 *
 * This function returns the cosine of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa facos
 */
static inline fixed fcos (fixed x)
{
   return _cos_tbl[((x + 0x4000) >> 15) & 0x1FF];
}

/**
 * \fn fixed fsin (fixed x)
 * \brief Returns the sine of a fixed point.
 *
 * This function returns the sine of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa fasin
 */
static inline fixed fsin (fixed x)
{ 
   return _cos_tbl[((x - 0x400000 + 0x4000) >> 15) & 0x1FF];
}

/**
 * \fn fixed ftan (fixed x)
 * \brief Returns the tangent of a fixed point.
 *
 * This function returns the tangent of the fixed point \a x, 
 * where \a x is given in radians.
 *
 * \sa fcos, fsin
 */
static inline fixed ftan (fixed x)
{ 
   return _tan_tbl[((x + 0x4000) >> 15) & 0xFF];
}

/**
 * \fn fixed facos (fixed x)
 * \brief Calculates and returns the arc cosine of a fixed point.
 *
 * This function calculates the arc cosine of the fixed point \a x; 
 * that is the value whose cosine is \a x. If \a x falls outside
 * the range -1 to 1, this function fails and \a errno is set to EDOM.
 *
 * \return Returns the arc cosine in radians and the value is mathematically 
 *         defined to be between 0 and PI (inclusive).
 *
 * \sa fcos
 */
static inline fixed facos (fixed x)
{
   if ((x < -65536) || (x > 65536)) {
      errno = EDOM;
      return 0;
   }

   return _acos_tbl[(x+65536+127)>>8];
}

/**
 * \fn fixed fasin (fixed x)
 * \brief Calculates and returns the arc sine of a fixed point.
 *
 * This function calculates the arc sine of the fixed point \a x; 
 * that is the value whose sine is \a x. If \a x falls outside
 * the range -1 to 1, this function fails and \a errno is set to EDOM.
 *
 * \return Returns the arc sine in radians and the value is mathematically 
 *         defined to be between -PI/2 and PI/2 (inclusive).
 *
 * \sa fsin
 */
static inline fixed fasin (fixed x)
{ 
   if ((x < -65536) || (x > 65536)) {
      errno = EDOM;
      return 0;
   }

   return 0x00400000 - _acos_tbl[(x+65536+127)>>8];
}

    /** @} end of fixed_math_fns */

#ifdef _MATH_3D

typedef struct MATRIX            /* transformation matrix (fixed point) */
{
   fixed v[3][3];                /* scaling and rotation */
   fixed t[3];                   /* translation */
} MATRIX;

typedef struct MATRIX_f          /* transformation matrix (floating point) */
{
   float v[3][3];                /* scaling and rotation */
   float t[3];                   /* translation */
} MATRIX_f;

extern MATRIX identity_matrix;
extern MATRIX_f identity_matrix_f;

void get_translation_matrix (MATRIX *m, fixed x, fixed y, fixed z);
void get_translation_matrix_f (MATRIX_f *m, float x, float y, float z);

void get_scaling_matrix (MATRIX *m, fixed x, fixed y, fixed z);
void get_scaling_matrix_f (MATRIX_f *m, float x, float y, float z);

void get_x_rotate_matrix (MATRIX *m, fixed r);
void get_x_rotate_matrix_f (MATRIX_f *m, float r);

void get_y_rotate_matrix (MATRIX *m, fixed r);
void get_y_rotate_matrix_f (MATRIX_f *m, float r);

void get_z_rotate_matrix (MATRIX *m, fixed r);
void get_z_rotate_matrix_f (MATRIX_f *m, float r);

void get_rotation_matrix (MATRIX *m, fixed x, fixed y, fixed z);
void get_rotation_matrix_f (MATRIX_f *m, float x, float y, float z);

void get_align_matrix (MATRIX *m, fixed xfront, fixed yfront, fixed zfront, fixed xup, fixed yup, fixed zup);
void get_align_matrix_f (MATRIX_f *m, float xfront, float yfront, float zfront, float xup, float yup, float zup);

void get_vector_rotation_matrix (MATRIX *m, fixed x, fixed y, fixed z, fixed a);
void get_vector_rotation_matrix_f (MATRIX_f *m, float x, float y, float z, float a);

void get_transformation_matrix (MATRIX *m, fixed scale, fixed xrot, fixed yrot, fixed zrot, fixed x, fixed y, fixed z);
void get_transformation_matrix_f (MATRIX_f *m, float scale, float xrot, float yrot, float zrot, float x, float y, float z);

void get_camera_matrix (MATRIX *m, fixed x, fixed y, fixed z, fixed xfront, fixed yfront, fixed zfront, 
                fixed xup, fixed yup, fixed zup, fixed fov, fixed aspect);
void get_camera_matrix_f (MATRIX_f *m, float x, float y, float z, float xfront, float yfront, float zfront, 
                float xup, float yup, float zup, float fov, float aspect);

void qtranslate_matrix (MATRIX *m, fixed x, fixed y, fixed z);
void qtranslate_matrix_f (MATRIX_f *m, float x, float y, float z);

void qscale_matrix (MATRIX *m, fixed scale);
void qscale_matrix_f (MATRIX_f *m, float scale);

void matrix_mul (AL_CONST MATRIX *m1, AL_CONST MATRIX *m2, MATRIX *out);
void matrix_mul_f (AL_CONST MATRIX_f *m1, AL_CONST MATRIX_f *m2, MATRIX_f *out);

fixed vector_length (fixed x, fixed y, fixed z);
float vector_length_f (float x, float y, float z);

void normalize_vector (fixed *x, fixed *y, fixed *z);
void normalize_vector_f (float *x, float *y, float *z);

void cross_product (fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2, fixed *xout, fixed *yout, fixed *zout);
void cross_product_f (float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout);

fixed polygon_z_normal (AL_CONST V3D *v1, AL_CONST V3D *v2, AL_CONST V3D *v3);
float polygon_z_normal_f (AL_CONST V3D_f *v1, AL_CONST V3D_f *v2, AL_CONST V3D_f *v3);

void apply_matrix_f (AL_CONST MATRIX_f *m, float x, float y, float z, float *xout, float *yout, float *zout);

extern fixed _persp_xscale;
extern fixed _persp_yscale;
extern fixed _persp_xoffset;
extern fixed _persp_yoffset;

extern float _persp_xscale_f;
extern float _persp_yscale_f;
extern float _persp_xoffset_f;
extern float _persp_yoffset_f;

void set_projection_viewport (int x, int y, int w, int h);

typedef struct QUAT
{
   float w, x, y, z;
} QUAT;

extern QUAT, identity_quat;

void quat_mul (AL_CONST QUAT *p, AL_CONST QUAT *q, QUAT *out)
void get_x_rotate_quat (QUAT *q, float r);
void get_y_rotate_quat (QUAT *q, float r);
void get_z_rotate_quat (QUAT *q, float r);
void get_rotation_quat (QUAT *q, float x, float y, float z);
void get_vector_rotation_quat (QUAT *q, float x, float y, float z, float a);
void quat_to_matrix (AL_CONST QUAT *q, MATRIX_f *m);
void matrix_to_quat (AL_CONST MATRIX_f *m, QUAT *q);
void apply_quat (AL_CONST QUAT *q, float x, float y, float z, float *xout, float *yout, float *zout);
void quat_slerp (AL_CONST QUAT *from, AL_CONST QUAT *to, float t, QUAT *out, int how);

#define QUAT_SHORT   0
#define QUAT_LONG    1
#define QUAT_CW      2
#define QUAT_CCW     3
#define QUAT_USER    4

#define quat_interpolate(from, to, t, out)   quat_slerp((from), (to), (t), (out), QUAT_SHORT)

static inline fixed dot_product (fixed x1, fixed y1, fixed z1, fixed x2, fixed y2, fixed z2)
{
   return fmul(x1, x2) + fmul(y1, y2) + fmul(z1, z2);
}


static inline float dot_product_f (float x1, float y1, float z1, float x2, float y2, float z2)
{
   return (x1 * x2) + (y1 * y2) + (z1 * z2);
}


#define CALC_ROW(n)     (fmul(x, m->v[n][0]) +        \
                         fmul(y, m->v[n][1]) +        \
                         fmul(z, m->v[n][2]) +        \
                         m->t[n])
static inline void apply_matrix (MATRIX *m, fixed x, fixed y, fixed z, fixed *xout, fixed *yout, fixed *zout)
{
   *xout = CALC_ROW(0);
   *yout = CALC_ROW(1);
   *zout = CALC_ROW(2);
}
#undef CALC_ROW

static inline void persp_project (fixed x, fixed y, fixed z, fixed *xout, fixed *yout)
{
   *xout = fmul(fdiv(x, z), _persp_xscale) + _persp_xoffset;
   *yout = fmul(fdiv(y, z), _persp_yscale) + _persp_yoffset;
}

static inline void persp_project_f (float x, float y, float z, float *xout, float *yout)
{
   float z1 = 1.0f / z;
   *xout = ((x * z1) * _persp_xscale_f) + _persp_xoffset_f;
   *yout = ((y * z1) * _persp_yscale_f) + _persp_yoffset_f;
}

#endif /* _MATH_3D */

    /** @} end of global_fns */

    /** @} end of fns */

#endif /* _FIXED_MATH */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _MGUI_FIXED_MATH_H */

