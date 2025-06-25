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


// Note:
// BOMB code handles also the mines

#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#include "bomb.h"
#include "game.h"
#include "init.h"
#include "mymath.h"
#include "explo.h"
#include "critter.h"
#include "particle.h"

// Bomb graphics
SDL_Surface *gfx_bomb;
SDL_Surface *gfx_mine[2];
SDL_Surface *gfx_burn;

// Bomb array
BOMB bomb[MAX_BOMBS];
int cur_bomb;


// Explode a bomb
void explode_bomb(BOMB *b) {

	if(num_critters == 0)
		return;

	// Loop through critters and damage them
	CRITTER *temp = critter_first;

	while(temp) {
		// Check distance
		float dist = distance(temp->x, temp->y, b->x, b->y);

		// Do damage based on distance
		if(dist <= 100) {
			float d = (float)(1.0f-dist/100.0f);
			int dam = (int)(300.0f * d);

			if(dam > 0) {
				temp->health -= dam;
				if(temp->health <= 0)
					temp->die(compute_angle(temp->x, temp->y, b->x, b->y));
			}

			// Push critters
			float px,py;
			move_xy(RANDF(6,8)*d, compute_angle(temp->x, temp->y, b->x, b->y), &px,&py);
			temp->dirx += px;
			temp->diry += py;

		}

		temp = temp->next;
	}
}


// Create a bomb
void create_bomb(float x, float y, float angle) {

	int f=cur_bomb;

	bomb[f].x = x;
	bomb[f].y = y;
	bomb[f].type = TYP_BOMB;

	float dx,dy;
	move_xy(RANDF(7,9), angle, &dx,&dy);
	bomb[f].dirx = dx;
	bomb[f].diry = dy;
	bomb[f].ok = true;
	
	cur_bomb++;
	if(cur_bomb > MAX_BOMBS-1)
		cur_bomb = 0;
}


// Create a mine
void create_mine(float x, float y, float angle) {

	int f=cur_bomb;

	bomb[f].x = x;
	bomb[f].y = y;
	bomb[f].type = TYP_MINE;
	bomb[f].life = 8*60;	// Eight secs

	float dx,dy;
	move_xy(RANDF(7,9), angle, &dx,&dy);
	bomb[f].dirx = dx;
	bomb[f].diry = dy;
	bomb[f].ok = true;
	
	cur_bomb++;
	if(cur_bomb > MAX_BOMBS-1)
		cur_bomb = 0;
}


// Move bombs
void move_bombs() {

	for(int f=0; f<MAX_BOMBS; f++)
		if(bomb[f].ok)
			bomb[f].move();
}


// Draw bombs
void draw_bombs(SDL_Surface *bmp) {

	for(int f=0; f<MAX_BOMBS; f++)
		if(bomb[f].ok)
			bomb[f].draw(bmp);
}


// Clear bombs
void clear_bombs() {

	cur_bomb = 0;
	for(int f=0; f<MAX_BOMBS; f++)
		bomb[f].clear();
}


// Load bomb graphics
void load_bombs() {

	// Load the graphic, set the color key and convert it to display format
	// Bomb sprite
	SDL_Surface *tmp;
	char file[100] = "";
	sprintf(file, "bomb.dat");
	tmp = IMG_Load(DAT(file));
	if(tmp == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to bomb sprite!\n%s", SDL_GetError());

	gfx_bomb = SDL_DisplayFormat(tmp);
	if(gfx_bomb == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	SDL_FreeSurface(tmp);

	// Mine sprite
	sprintf(file, "mine.dat");
	tmp = IMG_Load(DAT(file));
	if(tmp == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to bomb sprite!\n%s", SDL_GetError());

	gfx_mine[0] = SDL_DisplayFormat(tmp);
	if(gfx_mine[0] == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	SDL_FreeSurface(tmp);

	// Disabled mine sprite
	sprintf(file, "minedis.dat");
	tmp = IMG_Load(DAT(file));
	if(tmp == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to bomb sprite!\n%s", SDL_GetError());

	gfx_mine[1] = SDL_DisplayFormat(tmp);
	if(gfx_mine[1] == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	SDL_FreeSurface(tmp);


	// Burn mark
	sprintf(file, "burn.dat");
	tmp = IMG_Load(DAT(file));
	if(tmp == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	gfx_burn = SDL_DisplayFormatAlpha(tmp);
	if(gfx_burn == NULL)
		error_msg("Unable to load bomb graphics!\n%s", IMG_GetError());

	SDL_FreeSurface(tmp);

}


// Unload bombs
void unload_bombs() {

	if(gfx_bomb)
		SDL_FreeSurface(gfx_bomb);

	if(gfx_mine[0])
		SDL_FreeSurface(gfx_mine[0]);
	if(gfx_mine[1])
		SDL_FreeSurface(gfx_mine[1]);

	if(gfx_burn)
		SDL_FreeSurface(gfx_burn);
}


// Move the bomb
void BOMB::move() {

	// Model some friction
	if(type == TYP_BOMB) {
		dirx *= 0.96;
		diry *= 0.96;
	}
	else {
		dirx *= 0.94;
		diry *= 0.94;
	}

	// Move the bomb (and check collisions)
	float sx,sy;
	sx = x;
	sy = y;

	x += dirx;
	if(check_collision((int)x,(int)y)) {
		dirx = -dirx;
		x = sx;
	}

	y += diry;
	if(check_collision((int)x,(int)y)) {
		diry = -diry;
		y = sy;
	}

	// Create particles
	if(type == TYP_BOMB && RAND(0,100) > 50) {
		float px,py;
		move_xy(RANDF(0.1,2), RAND(315,325), &px,&py);
		add_particle(x, y-8, px, px, 255,128,10, RAND(5,15));
	}

	// Check collisions with critters
	if(num_critters > 0) {
		CRITTER *temp = critter_first;
		while(temp) {
			if(rects_collide(x,y,1,1, temp->x-16, temp->y-16, 32,32)) {
				// Bomb bounces
				if(type == TYP_BOMB) {
					dirx *= -0.7;
					diry *= -0.7;
					x = sx;
					y = sy;
					break;
				}
				else {
					// Mine explodes (only if it has activated == stopped moving)
					if(fabs(dirx) <= 0.1 && fabs(diry) <= 0.1) {
						ok = false;

						// Create explosion
						create_explosion((int)x,(int)y, RAND(0,1), RANDF(0.3,0.5));

						// Create burn mark
						SDL_Rect r;
						r.x = (int)x - gfx_burn->w/2;
						r.y = (int)y - gfx_burn->h/2;
						r.w = gfx_burn->w;
						r.h = gfx_burn->h;
						SDL_BlitSurface(gfx_burn, NULL, cur_bground, &r);

						explode_bomb(this);
					}
					break;
				}
			}

			temp = temp->next;
		}
	}


	// Explode the bomb if it has stopped moving
	// Or reduce life is it's mine
	//if(distance(dirx, diry, 0,0) <= 0.1) {
	if(fabs(dirx) <= 0.1 && fabs(diry) <= 0.1) {
		if(type == TYP_BOMB) {
			ok = false;

			// Create explosion
			create_explosion((int)x,(int)y, RAND(0,1), RANDF(0.3,0.5));

			// Create burn mark
			SDL_Rect r;
			r.x = (int)x - gfx_burn->w/2;
			r.y = (int)y - gfx_burn->h/2;
			r.w = gfx_burn->w;
			r.h = gfx_burn->h;
			SDL_BlitSurface(gfx_burn, NULL, cur_bground, &r);

			explode_bomb(this);
		}
		else {
			life--;
			if(life <= 0) {
				// The mine is now disabled
				SDL_Rect r;
				r.x = (int)x - gfx_mine[1]->w/2;
				r.y = (int)y - gfx_mine[1]->h/2;
				r.w = gfx_mine[1]->w;
				r.h = gfx_mine[1]->h;

				SDL_BlitSurface(gfx_mine[1], NULL, cur_bground, &r);
				ok = false;
			}
		}
	}

}


// Draw the bomb
void BOMB::draw(SDL_Surface *bmp) {

	SDL_Surface *fram = (type == TYP_BOMB) ? gfx_bomb : gfx_mine[0];

	SDL_Rect r;
	r.x = (int)x - fram->w/2;
	r.y = (int)y - fram->h/2;
	r.w = fram->w;
	r.h = fram->h;

	SDL_BlitSurface(fram, NULL, bmp, &r);
}


// Clear the bomb
void BOMB::clear() {
	x = y = 0;
	dirx = diry = 0;
	ok = false;
	type = 0;
	life = 0;
}

