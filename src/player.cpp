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
#include <math.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "mymath.h"
#include "init.h"
#include "player.h"
#include "game.h"
#include "blend.h"
#include "critter.h"
#include "powerup.h"
#include "particle.h"
#include "bomb.h"


// Player graphics (walking)
SDL_Surface *player_gfx[3][36];
// .. (shooting)
SDL_Surface *player_gfxs[2][36];
// .. (dead)
SDL_Surface *player_dead[36];

// Gun flames
SDL_Surface *gunflame[2];

// Blood
SDL_Surface *pblood[3];

// Energy bar
SDL_Surface *energy_bar[2];
SDL_Surface *weapon_bar[2];


// Player sounds
Mix_Chunk *snd_hit[5];

// Bullet sounds
Mix_Chunk *snd_bullet[4];

// Shooting sound
Mix_Chunk *snd_shoot;
Mix_Chunk *snd_throw;

// Dying sound
Mix_Chunk *snd_die[3];



// Players
PLAYER player;



// Load player graphics
void load_player_gfx() {

	int i,f;

	// Load walking gfx
	for(i=0; i<3; i++) {
		for(f=0; f<36; f++) {
			char file[100] = "";
			sprintf(file, "f%dplayer_%04d.dat", i+1, f);

			// Load the graphic, set the color key and convert it to display format
			SDL_Surface *tmp;
			tmp = IMG_Load(DAT(file));
			if(tmp == NULL)
				error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

			if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
				error_msg("Unable to set color key to player sprite!\n%s", SDL_GetError());

			player_gfx[i][f] = SDL_DisplayFormat(tmp);
			if(player_gfx[i][f] == NULL)
				error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

			SDL_FreeSurface(tmp);
		}
	}

	// Load shooting gfx
	for(i=0; i<2; i++) {
		for(f=0; f<36; f++) {
			char file[100] = "";
			sprintf(file, "fs%dplayer_%04d.dat", i+1, f);

			// Load the graphic, set the color key and convert it to display format
			SDL_Surface *tmp;
			tmp = IMG_Load(DAT(file));
			if(tmp == NULL)
				error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

			if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
				error_msg("Unable to set color key to player sprite!\n%s", SDL_GetError());

			player_gfxs[i][f] = SDL_DisplayFormat(tmp);
			if(player_gfxs[i][f] == NULL)
				error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

			SDL_FreeSurface(tmp);
		}
	}

	// Load dead gfx
	for(f=0; f<36; f++) {
		char file[100] = "";
		sprintf(file, "playerd_%04d.dat", f);

		// Load the graphic, set the color key and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

		if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
			error_msg("Unable to set color key to player sprite!\n%s", SDL_GetError());

		player_dead[f] = SDL_DisplayFormat(tmp);
		if(player_dead[f] == NULL)
			error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

		SDL_FreeSurface(tmp);
	}

	// Load gun flames
	SDL_Surface *tmp;
	tmp = IMG_Load(DAT("gunflame1.dat"));
	if(tmp == NULL)
		error_msg("Unable to load gun flame sprites!\n%s", IMG_GetError());
	gunflame[0] = SDL_DisplayFormat(tmp);
	if(gunflame[0] == NULL)
		error_msg("Unable to load gun flame sprites!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	tmp = IMG_Load(DAT("gunflame2.dat"));
	if(tmp == NULL)
		error_msg("Unable to load gun flame sprites!\n%s", IMG_GetError());
	gunflame[1] = SDL_DisplayFormat(tmp);
	if(gunflame[1] == NULL)
		error_msg("Unable to load gun flame sprites!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	// Energy bar
	tmp = IMG_Load(DAT("energy_bar1.dat"));
	if(tmp == NULL)
		error_msg("Unable to load energy bar!\n%s", IMG_GetError());
	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to energy bar!\n%s", SDL_GetError());
	energy_bar[0] = SDL_DisplayFormat(tmp);
	if(energy_bar[0] == NULL)
		error_msg("Unable to load energy bar!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	tmp = IMG_Load(DAT("energy_bar2.dat"));
	if(tmp == NULL)
		error_msg("Unable to load energy bar!\n%s", IMG_GetError());
	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to energy bar!\n%s", SDL_GetError());
	energy_bar[1] = SDL_DisplayFormat(tmp);
	if(energy_bar[1] == NULL)
		error_msg("Unable to load energy bar!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	// Weapon bar
	tmp = IMG_Load(DAT("wpn_bar1.dat"));
	if(tmp == NULL)
		error_msg("Unable to load weapon bar!\n%s", IMG_GetError());
	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to weapon bar!\n%s", SDL_GetError());
	weapon_bar[0] = SDL_DisplayFormat(tmp);
	if(weapon_bar[0] == NULL)
		error_msg("Unable to load weapon bar!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	tmp = IMG_Load(DAT("wpn_bar2.dat"));
	if(tmp == NULL)
		error_msg("Unable to load weapon bar!\n%s", IMG_GetError());
	if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255,0,255)))
		error_msg("Unable to set color key to weapon bar!\n%s", SDL_GetError());
	weapon_bar[1] = SDL_DisplayFormat(tmp);
	if(weapon_bar[1] == NULL)
		error_msg("Unable to load weapon bar!\n%s", IMG_GetError());
	SDL_FreeSurface(tmp);

	// Blood
	for(i=0; i<3; i++) {
		char file[100] = "";
		sprintf(file, "blood%d.dat", i+1);

		// Load the graphic and convert it to display format
		SDL_Surface *tmp;
		tmp = IMG_Load(DAT(file));
		if(tmp == NULL)
			error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

		pblood[i] = SDL_DisplayFormatAlpha(tmp);
		if(pblood[i] == NULL)
			error_msg("Unable to load player graphics from %s!\n%s", file, IMG_GetError());

		SDL_FreeSurface(tmp);
	}
}


// Unload player graphics
void unload_player_gfx() {

	int i,f;
	for(i=0; i<3; i++)
		for(f=0; f<36; f++)
			if(player_gfx[i][f])
				SDL_FreeSurface(player_gfx[i][f]);

	for(i=0; i<2; i++)
		for(f=0; f<36; f++)
			if(player_gfxs[i][f])
				SDL_FreeSurface(player_gfxs[i][f]);

	for(i=0; i<36; i++)
		if(player_dead[i])
			SDL_FreeSurface(player_dead[i]);

	if(gunflame[0])
		SDL_FreeSurface(gunflame[0]);
	if(gunflame[1])
		SDL_FreeSurface(gunflame[1]);
	if(energy_bar[0])
		SDL_FreeSurface(energy_bar[0]);
	if(energy_bar[1])
		SDL_FreeSurface(energy_bar[1]);
	if(weapon_bar[0])
		SDL_FreeSurface(weapon_bar[0]);
	if(weapon_bar[1])
		SDL_FreeSurface(weapon_bar[1]);


	for(i=0; i<3; i++)
		if(pblood[i])
			SDL_FreeSurface(pblood[i]);
}


// Player dies
void PLAYER::die() {

	ok = false;

	if(shoot_chan != -1 && config.sound) {
		Mix_HaltChannel(shoot_chan);
		shoot_chan = -1;
	}

	if(config.sound) {
		int chan = Mix_PlayChannel(-1, snd_die[RAND(0,2)], 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		Mix_SetPanning(chan, 255-pan, pan);
	}

	// Create some blood
	for(int f=0; f<10; f++)
		blood(RAND(5,10), RANDF(0,359));

}


// Draw the player
void PLAYER::draw(SDL_Surface *bmp) {

	// Choose the right angle
	int a = (int)((float)(angle / 360.0) * 36);
	if(a > 35)
		a = 0;

	// Choose the right frame
	int f = 0;
	SDL_Surface *fram;
	bool draw_gun_flame = false;

	if(ok) {
		if(!key[key_shoot]) {
			// We're not shooting
			switch((int)frame) {
				case 0: f = 0; break;
				case 1: f = 1; break;
				case 2: f = 2; break;
				case 3: f = 1; break;
			}
			fram = player_gfx[f][a];
		}
		else {
			// We're shooting
			if(reload == 5) {
				f = 1;
				draw_gun_flame = true;
			}
			else
				f = 0;

			fram = player_gfxs[f][a];
		}
	}
	else
		fram = player_dead[a];

	SDL_Rect rect;
	rect.x = (int)x - 30;
	rect.y = (int)y - 30;
	rect.w = 60;
	rect.h = 60;

	// Blit
	SDL_BlitSurface(fram, NULL, bmp, &rect); 

	// Draw the gun flame (with additive blending)
	if(draw_gun_flame) {
		if(SDL_MUSTLOCK(bmp))
			SDL_LockSurface(bmp);
		
		// Compute the position
		float ix,iy;
		move_xy(28, add_angle(angle, -40), &ix,&iy);

		if(!screen->format->palette)
			add_blit(gunflame[RAND(0,1)], bmp, x+ix-14, y+iy-14);
		else
			add_blit8(gunflame[RAND(0,1)], bmp, x+ix-14, y+iy-14);

		if(SDL_MUSTLOCK(bmp))
			SDL_UnlockSurface(bmp);
	}

}


// Shout
void PLAYER::shout() {

	if(!config.sound)
		return;

	if(shout_counter > 0)
		shout_counter--;

	if(shout_counter == 0) {
		int chan = Mix_PlayChannel(-1, snd_hit[RAND(0,4)], 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		Mix_SetPanning(chan, 255-pan, pan);
		shout_counter = 20;
	}

}


// Shoot
void PLAYER::shoot() {

	// Trace line from player to his gun direction
	// and look for walls/enemies that are in bullet's way

	float bx, by;

	float ix,iy;
	move_xy(28, add_angle(angle, -40), &ix,&iy);
	bx = x+ix;
	by = y+iy;

	// Bullet dir
	float bdirx, bdiry;
	float aa = add_angle(angle, RANDF(-65,-55));
	move_xy(5, aa, &bdirx, &bdiry);

	bool done = false;
	while(!done) {
		// Break out if bullet is out of screen
		if(bx < -10 || by < -10 || bx > screen->w+10 || by > screen->h+10)
			done = true;

		// Advance the bullet
		bx += bdirx;
		by += bdiry;

		// Check for enemies
		if(num_critters > 0) {
			CRITTER *temp = critter_first;
			while(temp) {
				//if(distance(bx,by, temp->x, temp->y) < 26) {
				if(rects_collide(bx,by,1,1, temp->x-16, temp->y-16, 32,32)) {
					// Bullet hit an enemy!
					temp->health -= 25;

					// Pour blood
					temp->blood(RAND(2,5), aa);

					// Push the critter back
					temp->dirx += bdirx*RANDF(0,0.7);
					temp->diry += bdiry*RANDF(0,0.7);

					// Dies?
					if(temp->health <= 0)
						temp->die(aa);
				
					done = true;
					break;
				}
				temp = temp->next;
			}
		}

		// Check for walls
		if(!done) {
			if(check_bul_collision((int)bx,(int)by)) {
				done = true;
				if(RAND(0,100) > 60 && config.sound) {
					int chan = Mix_PlayChannel(-1, snd_bullet[RAND(0,3)], 0);
					int pan = (int)(((float)bx / (float)SCREEN_W) * 255);
					Mix_SetPanning(chan, 255-pan, pan);
				}
				// Create some particles
				for(int f=0; f<RAND(3,6); f++) {
					float px,py;
					move_xy(RANDF(0,2), add_angle(aa, 180+RANDF(-30,30)), &px,&py);
					add_particle(bx, by, px,py, 255, RAND(100,255), RAND(0,50), RAND(5,15));
				}
			}
		}
	}

	reload = 5;
}


// Collisions with critter
bool PLAYER::col_critter() {

	CRITTER *temp = critter_first;

	// Loop through all critters
	while(temp) {
		// Check for collisions
		if(rects_collide(player.x-15, player.y-15, 30,30, temp->x-16, temp->y-16, 32,32)) {
			
			x = sx + dirx*0.3;
			y = sy + diry*0.3;
			dirx += RANDF(-1,1);
			diry += RANDF(-1,1);

			if(check_collision((int)x,(int)y)) {
				x = sx;
				y = sy;
			}
			return true;
		}
		temp = temp->next;
	}

	return false;
}


// Move the player
void PLAYER::move() {

	if(!ok)
		return;

	// Reload..
	if(reload > 0)
		reload--;
	if(reload2 > 0)
		reload2--;

	// Reduce drugs
	if(drugs > 0)
		drugs--;

	// Check keypresses
	// Turning
	if(key[key_left])
		angle = add_angle(angle, -6);
	else if(key[key_right])
		angle = add_angle(angle, 6);
	
	// Walking forward/backward
	if(key[key_up])
		move_xy(2, angle, &dirx, &diry);
	else if(key[key_down])
		move_xy(-1, angle, &dirx,&diry);

	// Check for shooting
	if(reload == 0 && key[key_shoot]) {
		// Shoot
		shoot();

		// Some random shaking
		dirx += RANDF(-0.2,0.2);
		diry += RANDF(-0.2,0.2);
		angle = add_angle(angle, RANDF(-2,2));

		// Model some recoil
		float dx,dy;
		move_xy(RANDF(-1.2,0), add_angle(angle,-40), &dx,&dy);
		dirx += dx;
		diry += dy;

		// Start shooting sound
		if(shoot_chan == -1 && config.sound)
			shoot_chan = Mix_PlayChannel(-1, snd_shoot, -1);

		// Update panning
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		Mix_SetPanning(shoot_chan, 255-pan, pan);
		
	}
	// Stop the shooting sound
	else if(!key[key_shoot]) {
		if(shoot_chan != -1 && config.sound) {
			Mix_HaltChannel(shoot_chan);
			shoot_chan = -1;
		}
	}

	// Invoke special weapon?
	if(reload2 == 0 && special_weapon != 0 && key[key_special]) {
		if(special_weapon == POWER_UP_BOMB) {
			create_bomb(x,y, add_angle(angle, RANDF(-45,-35)));
			reload2 = 10;
		}
		else if(special_weapon == POWER_UP_MINE) {
			create_mine(x,y, add_angle(angle, RANDF(-45,-35)));
			reload2 = 30;
		}
		// Play sound
		int chan = Mix_PlayChannel(-1, snd_throw, 0);
		int pan = (int)(((float)x / (float)SCREEN_W) * 255);
		Mix_SetPanning(chan, 255-pan, pan);

		swp_amount--;
		if(swp_amount <= 0)
			special_weapon = 0;
	}

	// Advance the animation
	if(key[key_up] || key[key_down] || key[key_left] || key[key_right]) {
		frame += 0.3;
		if((int)frame > 3)
			frame = 0;
	}
	else
		frame = 1;
	
	// Move and check collisions with walls
	sx = x;
	sy = y;

	x += dirx;
	if(check_collision((int)x,(int)y))
		x = sx;

	y += diry;
	if(check_collision((int)x,(int)y))
		y = sy;

	// Check collisions with critters
	col_critter();

	dirx *= 0.7;
	diry *= 0.7;

	// Check screen boundaries
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	if(x > screen->w)
		x = screen->w;
	if(y > screen->h)
		y = screen->h;

	// Check for power-ups
	for(int i=0; i<MAX_POWER_UPS; i++) {
		if(power_up[i].ok)
			if(fabs(x-power_up[i].x) <= 18 && fabs(y-power_up[i].y) <= 18)
				power_up[i].pick_up(this);
	}

}


// Clear the player
void PLAYER::clear() {

	x = y = sx = sy = 0;
	dirx = diry = 0;

	ok = true;
	angle = 0;
	mass = 1.0;

	radius = 10;
	health = 200;
	frame = 0;
	reload = 0;
	reload2 = 0;
	drugs = 0;

	key_up = config.key_up;
	key_down = config.key_down;
	key_left = config.key_left;
	key_right = config.key_right;
	key_shoot = config.key_shoot;
	key_special = config.key_special;

	swp_amount = 0;
	special_weapon = 0;

	shoot_chan = -1;

	shout_counter = 0;
}


// Pour blood
void PLAYER::blood(int am, float _angle) {

	// Pour some blood to background
	for(int f=0; f<am; f++) {
		float a = add_angle(_angle, RANDF(-70,70));
		float d = (RAND(0,100) < 70) ? RANDF(0,30) : RANDF(0,70);

		float bx,by;
		move_xy(d, a, &bx,&by);

		// Draw
		int i = RAND(0,2);
		SDL_Rect r;
		r.x = (int)(x+bx) - pblood[i]->w/2;
		r.y = (int)(y+by) - pblood[i]->h/2;
		r.w = pblood[i]->w;
		r.h = pblood[i]->h;

		SDL_BlitSurface(pblood[i], NULL, cur_bground, &r);
	}

}


