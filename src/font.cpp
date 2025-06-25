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

#include <string.h>
#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "init.h"
#include "font.h"

// Font characters (for each font)
SDL_Surface *fnt_char[FONTS][128];

// Font sizes
const int font_size[] = { 32, 16, 16, 32 };

// Font spacing for each font
const int font_spacing[] = { 20, 10, 10, 20 };


// Write a string using a specifig font
void font_write(SDL_Surface *bmp, int font, int x, int y, char *str) {

	// Loop through each character
	for(unsigned int c=0; c<strlen(str); c++) {
		// Blit character
		SDL_Rect r;
		r.x = x;
		r.y = y;
		r.w = font_size[font];
		r.h = font_size[font];
		SDL_BlitSurface(fnt_char[font][str[c]], NULL, bmp, &r);

		x += font_spacing[font];
	}
}


// Return string length using a specifig font
int font_length(int font, char *str) {

	return (strlen(str) * font_spacing[font]);
}


// Load fonts
void load_fonts() {

	// Choose the color key
	Uint32 color_key = SDL_MapRGB(screen->format, 255,0,255);

	for(int i=0; i<FONTS; i++) {

		// Load the font graphic to a single surface, then grab the
		// invidual characters out of it.
		SDL_Surface *tmp, *tmp2;
		char file[100] = "";
		sprintf(file, "font%d.dat", i+1);
		tmp2 = IMG_Load(DAT(file));
		if(tmp2 == NULL)
			error_msg("Unable to load fonts!\n%s", IMG_GetError());

		// Convert it to display format
		tmp = SDL_DisplayFormat(tmp2);
		if(tmp == NULL)
			error_msg("Unable to load fonts!\n%s", IMG_GetError());

		SDL_FreeSurface(tmp2);

		int x = 0, y = 0;
		for(int f=0; f<128; f++) {
			// Create surface and set the color keying
			fnt_char[i][f] = SDL_CreateRGBSurface(SDL_HWSURFACE, font_size[i],font_size[i],
				screen->format->BitsPerPixel,
				screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, 0);
			
			if(fnt_char[i][f] == NULL)
				error_msg("Unable to create font surfaces!\n%s", SDL_GetError());
			
			if(SDL_SetColorKey(fnt_char[i][f], SDL_SRCCOLORKEY|SDL_RLEACCEL, color_key))
				error_msg("Unable to set font color keys!\n%s", SDL_GetError());

			// Grab the font from tmp
			SDL_Rect r;
			r.x = x;
			r.y = y;
			r.w = font_size[i];
			r.h = font_size[i];
			SDL_BlitSurface(tmp, &r, fnt_char[i][f], NULL);

			x += font_size[i];
			if(x >= font_size[i]*16) {
				x = 0;
				y += font_size[i];
			}
		}

		SDL_FreeSurface(tmp);
	}

}


// Unload fonts
void unload_fonts() {

	for(int i=0; i<FONTS; i++)
		for(int f=0; f<128; f++)
			if(fnt_char[i][f])
				SDL_FreeSurface(fnt_char[i][f]);
}

