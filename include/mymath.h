/*
    Formido - a shooting game (see README for additional information)
    Copyright (C) 2002, 2003 Mika Halttunen

    You can contact me by email: lsoft@mbnet.fi
    -----------------------------------------------------------------

    This file is part of Formido.

    Formido is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Formido is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Formido; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MYMATH_H
#define MYMATH_H

#include <math.h>

#ifdef _MSC_VER
#pragma warning(disable : 4305 4244)
#endif

// Macros for table access
#define SIN(x) sintable[(int)(x*10)]
#define COS(x) costable[(int)(x*10)]
#define TAN(x) tantable[(int)(x*10)]

// Look-up tables
extern float sintable[3601];
extern float costable[3601];
extern float tantable[3601];

// PI....
#ifndef M_PI
#define M_PI 3.1415926536f
#endif

// If two rectangles overlap (=collide)
#define rects_collide(x1,y1,w1,h1,x2,y2,w2,h2) !(x1>x2+w2-1 || x2>x1+w1-1 || y1>y2+h2-1 || y2>y1+h1-1)

// Return a random number
#define RAND(min, max) rand()%((max)+1-(min))+(min)

// Return a random floating point number
float RANDF(float min, float max);

// Initialise math tables
void init_math();

// General math routines
float add_angle(float angle, float add);

// Distance between two points
float distance(float x1, float y1, float x2, float y2);

// Move x and y along angle at given speed
void move_xy(float speed, float angle, float *x, float *y);

// Compute angle between two points
float compute_angle(float x1, float y1, float x2, float y2);

// Compute distance between two angles
float compute_angle_distance(float angle1, float angle2);

// Conversions
float radian_to_degree(float r);
float degree_to_radian(float d);


#endif

