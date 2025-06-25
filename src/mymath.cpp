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

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mymath.h"

// Look-up tables
float sintable[3601];
float costable[3601];
float tantable[3601];


// Return a random floating point number
float RANDF(float min, float max) {
	float range = max - min;
	float num = range * rand() / RAND_MAX;
	return(num+min);
}


// Initialise math tables
void init_math() {

	for(int f=0; f<3601; f++) {
		float a = (float)f;
		sintable[f] = (float)sin(((a/10)/360)*2*M_PI);
		costable[f] = (float)cos(((a/10)/360)*2*M_PI);
		tantable[f] = (float)tan(degree_to_radian(a/10));
	}

	srand(time(NULL));
}


// Add value to a angle, return the result
// Wraps if necessary.
float add_angle(float angle, float add) {
	
	angle += add;
	if(angle > 360)
		angle -= 360;
	else if(angle < 0)
		angle += 360;
	
	return angle;
}


// Distance between two points
float distance(float x1, float y1, float x2, float y2) {
	
	float dx = x2-x1;
	float dy = y2-y1;

	return (float)sqrt((dx*dx)+(dy*dy));
}


// Move x and y along angle at given speed
void move_xy(float speed, float angle, float *x, float *y) {

	float ans;
	if(angle <= 90) {
		ans = COS(angle);
		ans *= speed;
		*y = -ans;
		ans = SIN(angle);
		ans *= speed;
		*x = ans;
	}
	else if(angle <= 180) {
		angle -= 90;
		ans = SIN(angle);
		ans *= speed;
		*y = ans;
		ans = COS(angle);
		ans *= speed;
		*x = ans;
	}
	else if(angle <= 270) {
		angle -= 180;
		ans = COS(angle);
		ans *= speed;
		*y = ans;
		ans = SIN(angle);
		ans *= speed;
		*x = -ans;
	}
	else {
		angle -= 270;
		ans = SIN(angle);
		ans *= speed;
		*y = -ans;
		ans = COS(angle);
		ans *= speed;
		*x = -ans;
	}
}


const double pi2 = M_PI/2;
const double x2pi = M_PI*2;
const double x2pi_minus_pi2 = x2pi - pi2;

// Compute angle between two points
float compute_angle(float x1, float y1, float x2, float y2) {

	double dx,dy;
	double ans = 0;

	dx = x1 - x2;
	dy = y2 - y1;
	if(dx == 0 && dy == 0)
		return 0;

	if(dx >= 0 && dy >= 0) {
		ans = atan2(dy, dx);
		ans = pi2 - ans;
	}
	else if(dx >= 0 && dy < 0) {
		dy = -dy;
		ans = atan2(dy, dx);
		ans += pi2;
	}
	else if(dx < 0 && dy < 0) {
		dx = -dx;
		dy = -dy;
		ans = atan2(dx, dy);
		ans += M_PI;
	}
	else if(dx < 0 && dy >= 0) {
		dx = -dx;
		ans = atan2(dy, dx);
		ans += x2pi_minus_pi2;
	}

	return radian_to_degree(ans);
}


// Compute distance between two angles
float compute_angle_distance(float angle1, float angle2) {

	float ans1,ans2;

	if(angle1 == angle2)
		return 0;
	else if(angle1 > angle2) {
		ans1 = angle1 - angle2;
		angle2 += 360;
		ans2 = angle2 - angle1;
		if(ans1 < ans2)
			return ans1;
		else
			return ans2;
	}
	else {
		ans1 = angle2 - angle1;
		angle1 += 360;
		ans2 = angle1 - angle2;
		if(ans1 < ans2)
			return ans1;
		else
			return ans2;
	}
}


const float r2d = (360.0f / x2pi);
const float d2r = (x2pi / 360.0f);

// Conversions
float radian_to_degree(float r) {
	return r * r2d;
}


float degree_to_radian(float d) {
	return d * d2r;
}

