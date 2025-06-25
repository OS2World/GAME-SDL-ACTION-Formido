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
#include "SDL.h"
#include "particle.h"
#include "game.h"
#include "blend.h"
#include "mymath.h"
#include "init.h"

// Particle array
PARTICLE parts[MAX_PARTICLES];
int cur_part;


// Add a particle
void add_particle(float x, float y, float xs, float ys, int r, int g, int b, int maxlife) {

	int f=cur_part;
	parts[f].x = x;
	parts[f].y = y;
	parts[f].dx = xs;
	parts[f].dy = ys;
	parts[f].r = r;
	parts[f].g = g;
	parts[f].b = b;
	parts[f].life = 0;
	parts[f].maxlife = maxlife;
	parts[f].ok = true;

	cur_part++;
	if(cur_part > MAX_PARTICLES-1)
		cur_part = 0;
}


// Draw the particles
void draw_particles(SDL_Surface *bmp) {

	if(SDL_MUSTLOCK(bmp))
		SDL_LockSurface(bmp);

	for(int f=0; f<MAX_PARTICLES; f++)
		if(parts[f].ok)
			parts[f].draw(bmp);

	if(SDL_MUSTLOCK(bmp))
		SDL_UnlockSurface(bmp);
}


// Move the particles
void move_particles() {

	for(int f=0; f<MAX_PARTICLES; f++)
		if(parts[f].ok)
			parts[f].move();
}


// Clear the particles
void clear_particles() {

	cur_part = 0;
	for(int f=0; f<MAX_PARTICLES; f++)
		parts[f].clear();
}


// Draw the particle
void PARTICLE::draw(SDL_Surface *bmp) {

	int a = (int)(((float)life / (float)maxlife)*255.0f);

	if(!screen->format->palette)
		add_pixel(bmp, (int)x,(int)y, (Uint8)r, (Uint8)g, (Uint8)b, 255-a);
	else
		add_pixel8(bmp, (int)x,(int)y, (Uint8)r, (Uint8)g, (Uint8)b, 255-a);
}


// Move the particle
void PARTICLE::move() {

	// Move the position
	x += dx;
	y += dy;

	// Advance life
	life++;
	if(life >= maxlife)
		ok = false;
}


// Clear the particle
void PARTICLE::clear() {
	
	x = y = 0;
	dx = dy = 0;

	life = maxlife = 0;
	ok = false;
	r = g = b = 0;
}

