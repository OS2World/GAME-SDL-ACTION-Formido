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

#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"
#include "SDL_mixer.h"

// Player graphics (walking)
extern SDL_Surface *player_gfx[3][36];
// .. (shooting)
extern SDL_Surface *player_gfxs[2][36];
// .. (dead)
extern SDL_Surface *player_dead[36];

// Gun flames
extern SDL_Surface *gunflame[2];

// Blood
extern SDL_Surface *pblood[3];

// Energy bar
extern SDL_Surface *energy_bar[2];
extern SDL_Surface *weapon_bar[2];

// Player sounds
extern Mix_Chunk *snd_hit[5];

// Bullet sounds
extern Mix_Chunk *snd_bullet[4];

// Shooting sound
extern Mix_Chunk *snd_shoot;
extern Mix_Chunk *snd_throw;

// Dying sound
extern Mix_Chunk *snd_die[3];


// Player class
class PLAYER {
public:

	float x,y;					// Position
	float sx,sy;				// "Safe" position (doesn't collide with anything)
	float dirx,diry;			// Direction

	bool ok;					// Is the player alive?

	float angle;				// Angle of heading
	float mass;					// Mass of the player

	float frame;				// Animation frame

	int radius;					// Player radius

	int health;					// Player health
	int reload;					// Reloading time left
	int reload2;				// "Reloading" for special weapon
	int special_weapon;			// Current special weapon (0 for none)
	int swp_amount;				// Special weapon amount

	int drugs;					// Drugs amount

	int shoot_chan;				// Shooting sound channel

	int shout_counter;

	// Keys
	int key_up;
	int key_down;
	int key_left;
	int key_right;
	int key_shoot;
	int key_special;

	// Functions
	void clear();				// Clearing function
	void move();				// Movement function
	void draw(SDL_Surface *bmp);// Drawing function
	void shoot();				// Shoot
	void blood(int am, float angle);	// Pour some blood
	void shout();				// Shout
	bool col_critter();			// Collisions with critters
	void die();					// Player dies
};


// Players
extern PLAYER player;

void load_player_gfx();
void unload_player_gfx();

#endif

