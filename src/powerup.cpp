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
#include "game.h"
#include "powerup.h"
#include "mymath.h"
#include "init.h"
#include "blend.h"
#include "particle.h"

// Power-up array
POWER_UP power_up[MAX_POWER_UPS];
int cur_power_up;

// GFX
SDL_Surface *gfx_pu_health[6];
SDL_Surface *gfx_pu_bomb[18];
SDL_Surface *gfx_pu_mine[19];
SDL_Surface *gfx_pu_mush[20];
SDL_Surface *pu_glow;

// Power-up sound
Mix_Chunk *snd_power_up;

// Mushroom sound
Mix_Chunk *snd_mushroom;


// Create a power-up
void create_power_up() {

	int f=cur_power_up;
	power_up[f].x = RAND(0,SCREEN_W);
	power_up[f].y = RAND(0,SCREEN_H);
	power_up[f].ok = true;
	power_up[f].frame = 0;
	power_up[f].life = 60*10;
	power_up[f].type = RAND(POWER_UP_HEALTH, POWER_UP_MUSHROOM);

	// Check that the power-up isn't inside walls
	while(check_collision(power_up[f].x, power_up[f].y)) {
		power_up[f].x = RAND(0,SCREEN_W);
		power_up[f].y = RAND(0,SCREEN_H);
	}
	
	cur_power_up++;
	if(cur_power_up > MAX_POWER_UPS-1)
		cur_power_up = 0;
}


// Draw the power-ups
void draw_power_ups(SDL_Surface *bmp) {

	for(int f=0; f<MAX_POWER_UPS; f++) {
		if(power_up[f].ok)
			power_up[f].draw(bmp);
	}
}


// Animate the power-ups
void move_power_ups() {

	for(int f=0; f<MAX_POWER_UPS; f++) {
		if(power_up[f].ok)
			power_up[f].move();
	}
}


// Clear the power-ups
void clear_power_ups() {

	cur_power_up = 0;
	for(int f=0; f<MAX_POWER_UPS; f++)
		power_up[f].clear();
}


// Load the power-ups
void load_power_ups() {

	int f=0;
	// Health power-up
	for(f=0; f<6; f++) {
		char file[100] = "";
		sprintf(file, "medkit%02d.dat", f+1);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load power-up graphics!\n%s", IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to power-up sprite!\n%s", SDL_GetError());

		gfx_pu_health[f] = SDL_DisplayFormat(tmp);
		if(gfx_pu_health[f] == NULL)
			error_msg("Unable to load power-up graphics!\n%s", SDL_GetError());

		SDL_FreeSurface(tmp);
	}

	// Bomb power-up
	for(f=0; f<18; f++) {
		char file[100] = "";
		sprintf(file, "bomb_%04d.dat", f);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load power-up graphics!\n%s", IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to power-up sprite!\n%s", SDL_GetError());

		gfx_pu_bomb[f] = SDL_DisplayFormat(tmp);
		if(gfx_pu_bomb[f] == NULL)
			error_msg("Unable to load power-up graphics!\n%s", SDL_GetError());

		SDL_FreeSurface(tmp);
	}

	// Mine power-up
	for(f=0; f<19; f++) {
		char file[100] = "";
		sprintf(file, "mine%02d.dat", f+1);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load power-up graphics!\n%s", IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to power-up sprite!\n%s", SDL_GetError());

		gfx_pu_mine[f] = SDL_DisplayFormat(tmp);
		if(gfx_pu_mine[f] == NULL)
			error_msg("Unable to load power-up graphics!\n%s", SDL_GetError());

		SDL_FreeSurface(tmp);
	}

	// Mushroom "power-up"
	for(f=0; f<20; f++) {
		char file[100] = "";
		sprintf(file, "mush_%04d.dat", f);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load power-up graphics!\n%s", IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to power-up sprite!\n%s", SDL_GetError());

		gfx_pu_mush[f] = SDL_DisplayFormat(tmp);
		if(gfx_pu_mush[f] == NULL)
			error_msg("Unable to load power-up graphics!\n%s", SDL_GetError());

		SDL_FreeSurface(tmp);
	}

	// Power-up glow
	SDL_Surface *tmp;
	tmp = IMG_Load(DAT("glow.dat"));
	if(tmp == NULL)
		error_msg("Unable to load power-up glow!\n%s", IMG_GetError());
	pu_glow = SDL_DisplayFormat(tmp);
	if(pu_glow == NULL)
		error_msg("Unable to load power-up glow!\n%s", SDL_GetError());
	SDL_FreeSurface(tmp);

}


// Unload the power-ups
void unload_power_ups() {

	int f=0;
	for(f=0; f<6; f++)
		if(gfx_pu_health[f])
			SDL_FreeSurface(gfx_pu_health[f]);

	for(f=0; f<18; f++)
		if(gfx_pu_bomb[f])
			SDL_FreeSurface(gfx_pu_bomb[f]);

	for(f=0; f<19; f++)
		if(gfx_pu_mine[f])
			SDL_FreeSurface(gfx_pu_mine[f]);

	for(f=0; f<20; f++)
		if(gfx_pu_mush[f])
			SDL_FreeSurface(gfx_pu_mush[f]);

	if(pu_glow)
		SDL_FreeSurface(pu_glow);
}


// Player picks power-up
void POWER_UP::pick_up(PLAYER *p) {

	switch(type) {
		case POWER_UP_HEALTH:	// Health
			p->health += 100;
			if(p->health > 200)
				p->health = 200;
			break;
		case POWER_UP_BOMB:		// Bomb
			p->special_weapon = POWER_UP_BOMB;
			p->swp_amount = 10;
			break;
		case POWER_UP_MINE:		// Mine
			p->special_weapon = POWER_UP_MINE;
			p->swp_amount = 10;
			break;
		case POWER_UP_MUSHROOM:	// Mushroom
			p->drugs = RAND(10,15) * 60;
			p->health = 200;
			break;
	}

	ok = false;

	// Play sound
	if(config.sound) {
		int chan = Mix_PlayChannel(-1, (type != POWER_UP_MUSHROOM) ? snd_power_up : snd_mushroom, 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		if(pan < 0) pan = 0;
		if(pan > 255) pan = 255;
		Mix_SetPanning(chan, 255-pan, pan);
	}

	// Create some particles
	for(int f=0; f<RAND(20,30); f++)
		add_particle(x, y, RANDF(-2,2), RANDF(-2,2), RAND(100,200), 0, 255, RAND(20,60));
}


// Draw the power-up
void POWER_UP::draw(SDL_Surface *bmp) {

	int f = (int)frame;

	SDL_Rect rect;
	SDL_Surface *fram;

	if(type == POWER_UP_HEALTH)
		fram = gfx_pu_health[f];
	else if(type == POWER_UP_BOMB)
		fram = gfx_pu_bomb[f];
	else if(type == POWER_UP_MINE)
		fram = gfx_pu_mine[f];
	else if(type == POWER_UP_MUSHROOM)
		fram = gfx_pu_mush[f];

	rect.x = x - fram->w/2;
	rect.y = y - fram->h/2;
	rect.w = fram->w;
	rect.h = fram->h;

	// Draw the glow
	if(SDL_MUSTLOCK(bmp))
		SDL_LockSurface(bmp);
	if(!screen->format->palette)
		add_blit(pu_glow, bmp, x-pu_glow->w/2, y-pu_glow->h/2, (0.5+SIN(glow_anim)*0.5)*255);
	else
		add_blit8(pu_glow, bmp, x-pu_glow->w/2, y-pu_glow->h/2, (0.5+SIN(glow_anim)*0.5)*255);

	if(SDL_MUSTLOCK(bmp))
		SDL_UnlockSurface(bmp);


	// Draw the sprite
	SDL_BlitSurface(fram, NULL, bmp, &rect); 

}


// Move the power-up
void POWER_UP::move() {

	life--;
	if(life <= 0)
		ok = false;

	// Advance the animation
	frame += 0.2;
	if(type == POWER_UP_HEALTH && (int)frame > 5)
		frame = 0;
	else if(type == POWER_UP_BOMB && (int)frame > 17)
		frame = 0;
	else if(type == POWER_UP_MINE && (int)frame > 18)
		frame = 0;
	else if(type == POWER_UP_MUSHROOM && (int)frame > 19)
		frame = 0;

	glow_anim = add_angle(glow_anim, 10);
}


// Clear the power-up
void POWER_UP::clear() {

	x = y = 0;
	frame = 0;
	type = 0;
	ok = false;
	life = 0;
}

