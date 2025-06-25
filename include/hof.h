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

#ifndef HALL_OF_FAME_H
#define HALL_OF_FAME_H

#include "SDL.h"

// Number of entries
#define NUM_ENTRIES		23

// Maximum length of player name (-1)
#define NAME_LEN		24

// Hiscore list
class HISCORE_LIST {
public:
	int kills[NUM_ENTRIES];				// Number of kills (score)
	char names[NUM_ENTRIES][NAME_LEN];	// Player name

	// Functions
	void clear();						// Clear the list
	void save(char *file);				// Save the list to a file
	void load(char *file);				// Load the list from a file
	int add(char *name, int kills);		// Add a name (return the place)
	void draw(SDL_Surface *bmp, int x, int y, int place);	// Draw (hilight 'place')
	void input_name(int place);			// Input name and update the list
};


#endif

