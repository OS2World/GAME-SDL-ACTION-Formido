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

#ifndef BOMB_H
#define BOMB_H

// Note:
// BOMB code handles also the mines

#include "SDL.h"

// Maximum number of bombs
#define MAX_BOMBS		64

// Bomb graphics
extern SDL_Surface *gfx_bomb;
extern SDL_Surface *gfx_mine[2];
extern SDL_Surface *gfx_burn;

// Type
#define TYP_BOMB		0
#define TYP_MINE		1


class BOMB {
public:
	float x,y;					// Position
	float dirx, diry;			// Direction

	bool ok;					// Does the bomb exist?
	int type;					// Bomb or mine?

	int life;					// Counter for mines

	// Funcs
	void clear();				// Clear the bomb
	void draw(SDL_Surface *bmp);// Draw the bomb
	void move();				// Move the bomb
};

// Bomb array
extern BOMB bomb[MAX_BOMBS];
extern int cur_bomb;


void create_bomb(float x, float y, float angle);
void create_mine(float x, float y, float angle);
void clear_bombs();
void draw_bombs(SDL_Surface *bmp);
void move_bombs();
void load_bombs();
void unload_bombs();

#endif

