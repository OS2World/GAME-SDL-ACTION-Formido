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

#ifndef BLEND_H
#define BLEND_H

struct SDL_Surface;

// Simple additive blender
// Destination surface must be locked!
// Doesn't work with 8-bit modes!
void add_blit(SDL_Surface *src, SDL_Surface *dst, int x, int y);

// Same as above, just with variable alpha (0-255)
void add_blit(SDL_Surface *src, SDL_Surface *dst, int x, int y, int alpha);

// Additive blended pixel
// Destination surface must be locked!
void add_pixel(SDL_Surface *dst, int x, int y, Uint8 sr, Uint8 sg, Uint8 sb, int alpha);

// Initialize look-up tables
void init_blend();


/////////////////////
// 8-bit versions
/////////////////////

void add_blit8(SDL_Surface *src, SDL_Surface *dst, int x, int y);
void add_blit8(SDL_Surface *src, SDL_Surface *dst, int x, int y, int alpha);
void add_pixel8(SDL_Surface *dst, int x, int y, Uint8 sr, Uint8 sg, Uint8 sb, int alpha);

#endif

