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

#ifndef EXPLO_H
#define EXPLO_H

#include "SDL.h"
#include "SDL_mixer.h"

// Graphics
extern SDL_Surface *gfx_explo[2][22];

// Sounds
extern Mix_Chunk *snd_explo[2];

// Maximum number of explosions
#define MAX_EXPLOS		64

// Explosion class
class EXPLO {
public:

	int x,y;					// Position
	float frame;				// Current frame
	float speed;				// Animation speed

	int type;					// Explosion type (0 or 1)
	bool ok;					// Does the explosion exist?

	// Funcs
	void clear();				// Clear the explosion
	void draw(SDL_Surface *bmp);// Draw the explosion
	void move();				// Animate the explosion
};

// Explosion array
extern EXPLO explo[MAX_EXPLOS];
extern int cur_explo;


void load_explosions();
void unload_explosions();
void create_explosion(int x, int y, int type, float speed);
void clear_explosions();
void draw_explosions(SDL_Surface *bmp);
void move_explosions();


#endif

