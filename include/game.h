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

#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "SDL_mixer.h"

// Array containing currently pressed keys
extern Uint8 key[SDLK_LAST];

// Current background surface
struct SDL_Surface;
extern SDL_Surface *cur_bground;

// Clean background
extern SDL_Surface *orig_bground;

// Collision map
extern SDL_Surface *col_map;

// Pause surface
extern SDL_Surface *pause_bmp;

// Drugs distortion surface
extern SDL_Surface *distort_tmp;



// Current kills
extern int kills;

// Load background images
void load_bgrounds();

// Start the game
void start_game();

// Does point (x,y) collide with geometry
bool check_collision(int x, int y);


// Does bullet at (x,y) collide with geometry
bool check_bul_collision(int x, int y);

// Handle SDL events
// Return true if we're going to quit
bool handle_event(SDL_Event &event);


#endif

