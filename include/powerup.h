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

#ifndef POWER_UP_H
#define POWER_UP_H

#include "player.h"
#include "SDL_mixer.h"
struct SDL_Surface;

// Maximum number of power-ups
#define MAX_POWER_UPS	6

// Power-up types
#define POWER_UP_HEALTH		0
#define POWER_UP_BOMB		1
#define POWER_UP_MINE		2
#define POWER_UP_MUSHROOM	3


// GFX
extern SDL_Surface *gfx_pu_health[6];
extern SDL_Surface *gfx_pu_bomb[18];
extern SDL_Surface *gfx_pu_mine[19];
extern SDL_Surface *gfx_pu_mush[20];
extern SDL_Surface *pu_glow;

// Power-up sound
extern Mix_Chunk *snd_power_up;

// Mushroom sound
extern Mix_Chunk *snd_mushroom;


// Power-up class
class POWER_UP {
public:
	int x,y;						// Position
	float frame;					// Animation frame
	int type;						// Power-up type

	bool ok;						// Does the power-up exist
	int life;						// Life left

	float glow_anim;				// Glow animation

	// Funcs
	void clear();					// Clear the power-up
	void draw(SDL_Surface *bmp);	// Draw the power-up
	void move();					// Animate the power-up
	void pick_up(PLAYER *p);		// Player picks power-up
};

// Power-up array
extern POWER_UP power_up[MAX_POWER_UPS];
extern int cur_power_up;


void create_power_up();
void load_power_ups();
void unload_power_ups();
void clear_power_ups();
void draw_power_ups(SDL_Surface *bmp);
void move_power_ups();

#endif

