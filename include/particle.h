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

#ifndef PARTICLE_H
#define PARTICLE_H

struct SDL_Surface;

// Maximum number of particles
#define MAX_PARTICLES	50

class PARTICLE {
public:
	float x,y;					// Position
	float dx,dy;				// Direction

	int life, maxlife;			// Current and maximum life

	int r,g,b;					// Color
	bool ok;					// Does the particle exist?

	// Funcs
	void clear();				// Clear the particle
	void move();				// Move the particle
	void draw(SDL_Surface *bmp);// Draw the particle
};

// Particle array
extern PARTICLE parts[MAX_PARTICLES];
extern int cur_part;


void add_particle(float x, float y, float xs, float ys, int r, int g, int b, int maxlife);
void clear_particles();
void move_particles();
void draw_particles(SDL_Surface *bmp);

#endif

