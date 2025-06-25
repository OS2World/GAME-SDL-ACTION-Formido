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

#ifndef CRITTER_H
#define CRITTER_H

#include "SDL.h"
#include "SDL_mixer.h"

// Critter graphics
extern SDL_Surface *critter_gfx[3][35];
extern SDL_Surface *cblood[3];

// Critter gibs
extern SDL_Surface *cgibs[15];

// Sounds
extern Mix_Chunk *snd_cdie[5];


// Critter class
class CRITTER {
public:

	float x,y;					// Position
	float sx,sy;				// "Safe" position (doesn't collide with anything)
	float dirx,diry;			// Direction

	bool ok;					// Does this critter exist?
	bool remove;				// Remove from the list

	float angle;				// Angle of heading

	float frame;				// Animation frame

	int health;					// Critter health

	// Functions
	void clear();				// Clearing function
	void move();	// Movement function
	void draw(SDL_Surface *bmp);// Drawing function
	void die(float bul_angle);	// Die (pass the bullet angle)
	void blood(int am, float bul_angle);

	CRITTER();
	~CRITTER();
	CRITTER *next;				// Next critter in the list

};


// Critter list
extern CRITTER *critter_first;
extern CRITTER *critter_last;
extern int num_critters;

CRITTER *add_critter(float x, float y, float angle);
void draw_critters(SDL_Surface *screen);
void move_critters();
void clear_critters();
void load_critters();
void unload_critters();

//////////////////////

// Critter gibs
class GIB {
public:

	float x,y;					// Position
	float dirx, diry;			// Direction

	bool remove;				// Remove from the list

	int sprite;					// Gib sprite

	// Functions
	void clear();				// Clearing function
	void move();	// Movement function
	void draw(SDL_Surface *bmp);// Drawing function

	GIB();
	~GIB();
	GIB *next;					// Next gib in the list
};


// Critter gib list
extern GIB *gib_first;
extern GIB *gib_last;
extern int num_gibs;

GIB *add_gib(float x, float y, float angle, int type);
void draw_gibs(SDL_Surface *screen);
void move_gibs();
void clear_gibs();


#endif

