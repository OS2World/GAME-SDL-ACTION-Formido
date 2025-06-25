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

#include <stdlib.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "explo.h"
#include "blend.h"
#include "init.h"
#include "mymath.h"


// Graphics
SDL_Surface *gfx_explo[2][22];

// Sounds
Mix_Chunk *snd_explo[2];


// Explosion array
EXPLO explo[MAX_EXPLOS];
int cur_explo;


// Load the explosions
void load_explosions() {

	int i,f;

	// Load explosion gfx
	for(i=0; i<2; i++) {
		for(f=0; f<22; f++) {
			char file[100] = "";
			sprintf(file, "explo%d_%04d.dat", i+1, f+1);

			// Load the graphic and convert it to display format
			SDL_Surface *tmp;
			tmp = IMG_Load(DAT(file));
			if(tmp == NULL)
				error_msg("Unable to load explosion graphics!\n%s", IMG_GetError());

			gfx_explo[i][f] = SDL_DisplayFormat(tmp);
			if(gfx_explo[i][f] == NULL)
				error_msg("Unable to load explosion graphics!\n%s", IMG_GetError());

			SDL_FreeSurface(tmp);
		}
	}

}


// Unload explosions
void unload_explosions() {

	for(int i=0; i<2; i++)
		for(int f=0; f<22; f++)
			if(gfx_explo[i][f])
				SDL_FreeSurface(gfx_explo[i][f]);
}


// Create an explosion
void create_explosion(int x, int y, int type, float speed) {
	
	int f=cur_explo;
	explo[f].x = x;
	explo[f].y = y;
	explo[f].type = type;
	explo[f].frame = 0;
	explo[f].speed = speed;
	explo[f].ok = true;

	cur_explo++;
	if(cur_explo > MAX_EXPLOS-1)
		cur_explo = 0;

	// Play sound
	if(config.sound) {
		int chan = Mix_PlayChannel(-1, snd_explo[RAND(0,1)], 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		if(pan < 0) pan = 0;
		if(pan > 255) pan = 255;
		Mix_SetPanning(chan, 255-pan, pan);
	}
}


// Animate the explosions
void move_explosions() {

	for(int f=0; f<MAX_EXPLOS; f++)
		if(explo[f].ok)
			explo[f].move();

}


// Draw the explosions
void draw_explosions(SDL_Surface *bmp) {

	if(SDL_MUSTLOCK(bmp))
		SDL_LockSurface(bmp);

	for(int f=0; f<MAX_EXPLOS; f++)
		if(explo[f].ok)
			explo[f].draw(bmp);

	if(SDL_MUSTLOCK(bmp))
		SDL_UnlockSurface(bmp);
}


// Clear the explosions
void clear_explosions() {

	cur_explo = 0;
	for(int f=0; f<MAX_EXPLOS; f++)
		explo[f].clear();
}


// Draw the explosion
void EXPLO::draw(SDL_Surface *bmp) {

	if(!screen->format->palette)
		add_blit(gfx_explo[type][(int)frame], bmp, x-32,y-32);
	else
		add_blit8(gfx_explo[type][(int)frame], bmp, x-32,y-32);

}


// Animate the explosion
void EXPLO::move() {

	// Advance the animation
	frame += speed;
	if((int)frame > 21)
		ok = false;

}


// Clear the explosion
void EXPLO::clear() {

	x = y = 0;
	frame = speed = 0;

	ok = false;
	type = 0;
}

