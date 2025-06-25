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

#ifndef FONT_H
#define FONT_H

// Number of fonts
#define FONTS			4

// Font characters (for each font)
struct SDL_Surface;
extern SDL_Surface *fnt_char[FONTS][128];

// Define font styles
#define FONT_BOLD		0
#define FONT_NORMAL		1
#define FONT_NORMAL2	2
#define FONT_BOLD2		3


// Load fonts
void load_fonts();

// Unload fonts
void unload_fonts();

// Write a string using a specifig font
void font_write(SDL_Surface *bmp, int font, int x, int y, char *str);

// Return string length using a specifig font
int font_length(int font, char *str);

#endif

