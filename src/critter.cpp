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

// Instability caused by my bugs was fixed
// with a patch by Julian Peterson

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "critter.h"
#include "init.h"
#include "mymath.h"
#include "game.h"
#include "player.h"

// Critter graphics
SDL_Surface *critter_gfx[3][35];
SDL_Surface *cblood[3];

// Critter gibs
SDL_Surface *cgibs[15];

// Sounds
Mix_Chunk *snd_cdie[5];


// Critter list
CRITTER *critter_first;
CRITTER *critter_last;
int num_critters;

// Critter gib list
GIB *gib_first;
GIB *gib_last;
int num_gibs;


// Load critter graphics
void load_critters() {

	int i;
	// Sprites
	for(i=0; i<3; i++) {
		for(int f=0; f<35; f++) {
			char file[100] = "";
			sprintf(file, "f%dcritter1_%04d.dat", i+1, f);

			// Load the graphic, set the color key and convert it to display format
			SDL_Surface *tmp;
			tmp = IMG_Load(DAT(file));
			if(tmp == NULL)
				error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

			if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
				error_msg("Unable to set color key to critter sprite!\n%s", SDL_GetError());

			critter_gfx[i][f] = SDL_DisplayFormat(tmp);
			if(critter_gfx[i][f] == NULL)
				error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

			SDL_FreeSurface(tmp);
		}
	}

	// Critter gibs
	for(i=0; i<15; i++) {
		char file[100] = "";
		sprintf(file, "critter_piece%d.dat", i+1);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to critter sprite!\n%s", SDL_GetError());

		cgibs[i] = SDL_DisplayFormat(tmp);
		if(cgibs[i] == NULL)
			error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

		SDL_FreeSurface(tmp);
	}

	// Blood
	for(i=0; i<3; i++) {
		char file[100] = "";
		sprintf(file, "cblood%d.dat", i+1);

		// Load the graphic and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

		cblood[i] = SDL_DisplayFormatAlpha(tmp);
		if(cblood[i] == NULL)
			error_msg("Unable to load critter graphics from %s!\n%s", file, IMG_GetError());

		SDL_FreeSurface(tmp);
	}

}


// Unload critter graphics
void unload_critters() {

	int i;
	for(i=0; i<3; i++) {
		for(int f=0; f<35; f++) {
			if(critter_gfx[i][f])
				SDL_FreeSurface(critter_gfx[i][f]);
		}
	}

	for(i=0; i<15; i++)
		if(cgibs[i])
			SDL_FreeSurface(cgibs[i]);

	for(i=0; i<3; i++)
		if(cblood[i])
			SDL_FreeSurface(cblood[i]);
}


// Create a critter
CRITTER *add_critter(float x, float y, float angle) {

	CRITTER *new_critter;
	new_critter = new CRITTER;
	if(!new_critter)
		error_msg("add_critter():\nUnable to add a new critter!\n");

	new_critter->x = x;
	new_critter->y = y;
	new_critter->sx = x;
	new_critter->sy = y;
	new_critter->angle = angle;

	// Append the critter to the list
	if(!critter_first)	// The list is empty
		critter_first = critter_last = new_critter;
	else {
		critter_last->next = new_critter;
		critter_last = new_critter;
	}
	num_critters++;

	return new_critter;
}



// Draw the critters
void draw_critters(SDL_Surface *screen) {

	if(num_critters == 0)
		return;

	CRITTER *temp = critter_first;
	
	while(temp) {
		temp->draw(screen);
		temp = temp->next;
	}
}


// Move the critters
void move_critters() {

	if(num_critters == 0)
		return;

	CRITTER *temp, *prev;
	temp = critter_first;
	prev = temp;

	while(temp) {
		temp->move();
		// Should we remove this critter?
		if(temp->remove) {
			if(critter_first == critter_last) {
				delete critter_first;
				critter_first = critter_last = temp=NULL;
				num_critters--;
			}
			else if(temp == critter_first) {
				critter_first = critter_first->next;
				temp->next = NULL;
				delete temp;
				temp = critter_first;
				num_critters--;
			}
			else if(temp == critter_last) {
				prev->next = NULL;
				temp->next = NULL;
				delete temp;
				critter_last = prev;
				temp = critter_last;
				num_critters--;
			}
			else {
				prev->next = temp->next;
				temp->next = NULL;
				delete temp;
				temp = prev;
				num_critters--;
			}
		}
		prev = temp;
		if(temp != NULL)
			temp = temp->next;

	}
}


// Clear the critters
void clear_critters() {

	if(critter_first)
		delete critter_first;
	critter_first = NULL;
	critter_last = NULL;
	num_critters = 0;
}


// Check critter collisions
bool critter_collisions(CRITTER *c) {

	bool col = false;

	// Note! We don't check collisions if the critter is out of screen
	if(c->x < 0 || c->y < 0 || c->x > screen->w || c->y > screen->h)
		return col;

	CRITTER *temp = critter_first;

	// Loop through all critters
	while(temp) {
		if(temp != c) {
			// Check for collisions

			// First I used distance() to check the collision, but the
			// game slowed down and practically freezed with high critter amounts.
			// So now I'm using bounding boxes.

			if(rects_collide(c->x-8, c->y-8, 16,16, temp->x-8, temp->y-8, 16,16)) {
				// Compute the normal vector
				float nabx = temp->x - c->x;
				float naby = temp->y - c->y;
				float len = sqrt(nabx*nabx + naby*naby);

				// Compute the response coordinate system axes
				nabx /= len;
				naby /= len;

				// Compute the tangential vector perpendicular to normal,
				// rotate the vector 90 degrees
				float tabx = -naby;
				float taby = nabx;

				// Compute all the initial velocities
				float vait = (temp->dirx * tabx + temp->diry * taby);
				float vain = (temp->dirx * nabx + temp->diry * naby);
				float vbit = (c->dirx * tabx + c->diry * taby);
				float vbin = (c->dirx * nabx + c->diry * naby);

				// Now we have the initial velocities in terms of the N and T axes
				// Compute the final velocities

				float vafn = (vbin);
				float vbfn = (vain);

				float vaft = vait;
				float vbft = vbit;

				// Translate back to original coordinate system
				temp->dirx = (vafn*nabx + vaft*tabx);
				temp->diry = (vafn*naby + vaft*taby);

				c->dirx = (vbfn*nabx + vbft*tabx);
				c->diry = (vbfn*naby + vbft*taby);
				col = true;
				break;
			}
		}
		temp = temp->next;
	}

	return col;
}


// Move the critter
void CRITTER::move() {

	// Advance the animation
	frame += 0.15;
	if((int)frame > 3)
		frame = 0;

	if(!player.ok)
		return;

	// Head for the player
	angle = compute_angle(player.x,player.y,x,y);

	// Move to player (and check collisions)
	//////
	// Update "safe-position"
	sx = x;
	sy = y;

	// Move
	float dx,dy;
	move_xy(0.25, angle, &dx, &dy);

	dirx += dx;
	diry += dy;

	// Check wall collisions
	x += dirx;
	if(check_collision((int)x,(int)sy)) {
		dirx = -dirx;
		dirx += RANDF(-2,2);
		diry += RANDF(-3,3);
		x = sx;
	}
	y += diry;
	if(check_collision((int)x,(int)y)) {
		diry = -diry;
		dirx += RANDF(-2,2);
		diry += RANDF(-2,2);
		y = sy;
	}

	// Check collisions with other critters
	if(critter_collisions(this)) {
		x = sx;
		y = sy;
	}

	// Attack if near player
	//if(distance(player.x, player.y, x,y) < 27) {
	if(rects_collide(x-16, y-16, 32,32, player.x-20, player.y-20, 40,40)) {
		player.blood(RAND(1,2), add_angle(angle, RANDF(-5,5)));

		x = sx;
		y = sy;

		// Player shouts
		player.shout();

		if(player.drugs == 0)
			player.health--;
		else if(player.drugs && RAND(0,100) >= 50)
			player.health--;

		if(player.health <= 0)
			player.die();
	}

	dirx *= 0.8;
	diry *= 0.8;

}


// Pour blood
void CRITTER::blood(int am, float bul_angle) {

	// Pour some blood to background
	for(int f=0; f<am; f++) {
		float a = add_angle(bul_angle, RANDF(-30,30));
		float d = RANDF(0,80);

		float bx,by;
		move_xy(d, a, &bx,&by);

		// Draw
		int i = RAND(0,2);
		SDL_Rect r;
		r.x = (int)(x+bx) - cblood[i]->w/2;
		r.y = (int)(y+by) - cblood[i]->h/2;
		r.w = cblood[i]->w;
		r.h = cblood[i]->h;

		SDL_BlitSurface(cblood[i], NULL, cur_bground, &r);
	}

}


// Die
void CRITTER::die(float bul_angle) {

	remove = true;

	kills++;

	// Play sound
	if(config.sound) {
		int chan = Mix_PlayChannel(-1, snd_cdie[RAND(0,4)], 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		if(pan < 0) pan = 0;
		if(pan > 255) pan = 255;
		Mix_SetPanning(chan, 255-pan, pan);
	}

	// Create blood
	blood(RAND(7,15), bul_angle);

	// Create gibs
	for(int f=0; f<RAND(3,6); f++) {
		add_gib(x, y, add_angle(bul_angle, RANDF(-50,50)), RAND(0,12));
	}
	// Create head gib
	if(RAND(0,100) > 50)
		add_gib(x, y, add_angle(bul_angle, RANDF(-50,50)), RAND(13,14));

}


// Draw the critter
void CRITTER::draw(SDL_Surface *bmp) {

	if(!ok)
		return;

	// Choose the right angle
	int a = (int)((float)(angle / 360.0) * 36);
	if(a >= 35)
		a = 0;

	// Choose the right frame
	int f = 0;
	switch((int)frame) {
		case 0: f = 0; break;
		case 1: f = 1; break;
		case 2: f = 2; break;
		case 3: f = 1; break;
	}

	SDL_Rect rect;
	rect.x = (int)x - 32;
	rect.y = (int)y - 32;
	rect.w = 64;
	rect.h = 64;

	SDL_BlitSurface(critter_gfx[f][a], NULL, bmp, &rect); 
}


// Clear the critter
void CRITTER::clear() {

	x = y = 0;
	dirx = diry = 0;
	sx = sy = 0;

	angle = 0;
	health = 100;
	ok = true;
	remove = false;
	frame = 0;

}


CRITTER::CRITTER() {

	clear();
	next = NULL;
}

CRITTER::~CRITTER() {
	if(next)
		delete next;	// Delete the next critter from the list
}


/////////////////////


// Add a new gib
GIB *add_gib(float x, float y, float angle, int type) {

	GIB *new_gib;
	new_gib = new GIB;
	if(!new_gib)
		error_msg("add_gib():\nUnable to add a new gib!\n");

	new_gib->x = x;
	new_gib->y = y;
	move_xy(RANDF(0.1,4), angle, &(new_gib->dirx), &(new_gib->diry));
	new_gib->sprite = type;

	// Append the critter gib to the list
	if(!gib_first)	// The list is empty
		gib_first = gib_last = new_gib;
	else {
		gib_last->next = new_gib;
		gib_last = new_gib;
	}
	num_gibs++;

	return new_gib;

}


// Draw the gibs
void draw_gibs(SDL_Surface *screen) {

	if(num_gibs == 0)
		return;

	GIB *temp = gib_first;

	while(temp) {
		temp->draw(screen);
		temp = temp->next;
	}
}


// Move the gibs
void move_gibs() {

	if(num_gibs == 0)
		return;

	GIB *temp, *prev;
	temp = gib_first;
	prev = temp;

	while(temp) {
		temp->move();
		// Should we remove this gib?
		if(temp->remove) {
			if(gib_first == gib_last) {
				delete gib_first;
				gib_first = gib_last = temp = NULL;
				num_gibs--;
			}
			else if(temp == gib_first) {
				gib_first = gib_first->next;
				temp->next = NULL;
				delete temp;
				temp = gib_first;
				num_gibs--;
			}
			else if(temp == gib_last) {
				prev->next = NULL;
				temp->next = NULL;
				delete temp;
				gib_last = prev;
				temp = gib_last;
				num_gibs--;
			}
			else {
				prev->next = temp->next;
				temp->next = NULL;
				delete temp;
				temp = prev;
				num_gibs--;
			}
		}
		prev = temp;
		if(temp != NULL)
			temp = temp->next;

	}
}


// Clear the gibs
void clear_gibs() {

	if(gib_first)
		delete gib_first;
	gib_first = NULL;
	gib_last = NULL;
	num_gibs = 0;
}


// Move the gib
void GIB::move() {

	// Model some friction
	dirx *= 0.96;
	diry *= 0.96;

	// Move the gib (and check collisions)
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

	// Emit some blood
	if(RAND(0,100) > 97) {
		int i = RAND(0,2);
		SDL_Rect r;
		r.x = (int)x - cblood[i]->w/2;
		r.y = (int)y - cblood[i]->h/2;
		r.w = cblood[i]->w;
		r.h = cblood[i]->h;

		SDL_BlitSurface(cblood[i], NULL, cur_bground, &r);
	}

	// Remove the gib if it has stopped moving
	//if(distance(dirx, diry, 0,0) <= 0.1) {
	if(fabs(dirx) <= 0.1 && fabs(diry) <= 0.1) {
		// Leave the gib to ground
		SDL_Rect rect;
		rect.x = (int)x - cgibs[sprite]->w/2;
		rect.y = (int)y - cgibs[sprite]->h/2;
		rect.w = cgibs[sprite]->w;
		rect.h = cgibs[sprite]->h;

		SDL_BlitSurface(cgibs[sprite], NULL, cur_bground, &rect); 
		remove = true;
	}
}


// Draw the gib
void GIB::draw(SDL_Surface *bmp) {

	SDL_Rect rect;
	rect.x = (int)x - cgibs[sprite]->w/2;
	rect.y = (int)y - cgibs[sprite]->h/2;
	rect.w = cgibs[sprite]->w;
	rect.h = cgibs[sprite]->h;

	SDL_BlitSurface(cgibs[sprite], NULL, bmp, &rect); 

}


// Clear the gib
void GIB::clear() {

	x = y = 0;
	dirx = diry = 0;
	sprite = 0;
	remove = false;
}


GIB::GIB() {
	
	clear();
	next = NULL;
}


GIB::~GIB() {

	if(next)			// Delete the next gib from the list
		delete next;
}

