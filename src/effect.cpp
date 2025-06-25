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

// Various screen transitions


#include <stdlib.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "mymath.h"
#include "timer.h"
#include "game.h"
#include "init.h"
#include "config.h"

// Glass break sound
Mix_Chunk *snd_glass_break;


// Number of quads
#define QUADS_X		32
#define QUADS_Y		24

// Data structure for the effect
class GLASS_QUAD {
public:
	float x, y;					// Position
	float xs,ys;				// Direction

	int px,py;					// Position in the image

	bool ok;					// Quad exists?
};

GLASS_QUAD quad[QUADS_X][QUADS_Y];


// A "breaking glass" - effect, in the spirit
// of a same kind of effect as in my older game, Steroids.
void break_screen() {

	// Fade out the music
	if(config.sound)
		Mix_FadeOutMusic(2000);

	// Grab the screen
	SDL_Surface *bmp;
	bmp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, 0);
	if(bmp == NULL)
		// Yeah, not that I wanted to display the effect...
		return;

	SDL_BlitSurface(screen, NULL, bmp, NULL);

	// Initialize quads
	int x,y;
	int quad_w = SCREEN_W/QUADS_X;
	int quad_h = SCREEN_H/QUADS_Y;

	int num_quads = 0;

	for(y=0; y<QUADS_Y; y++) {
		for(x=0; x<QUADS_X; x++) {
			// Position
			quad[x][y].x = x * quad_w;
			quad[x][y].y = y * quad_h;
			// Random speed
			quad[x][y].xs = RANDF(-2,2);
			quad[x][y].ys = RANDF(-0.5,0.2);

			if(RAND(0,100) > 90)
				quad[x][y].ys = RANDF(-2,0);

			quad[x][y].px = x * quad_w;
			quad[x][y].py = y * quad_h;
			quad[x][y].ok = true;
			num_quads++;
		}
	}

	// Play the sound
	if(config.sound) {
		Mix_PlayChannel(-1, snd_glass_break, 0);
	}

	// Loop
	timer_count = 0;
	while(num_quads > 0) {
		while(timer_count > 0) {
			// Move glass pieces
			for(y=0; y<QUADS_Y; y++) {
				for(x=0; x<QUADS_X; x++) {
					if(quad[x][y].ok) {
						quad[x][y].x += quad[x][y].xs;
						quad[x][y].y += quad[x][y].ys;

						// Model some gravity
						quad[x][y].ys += 0.1;

						// Check for screen borders
						if(quad[x][y].x < -quad_w || quad[x][y].x > SCREEN_W || quad[x][y].y > SCREEN_H) {
							quad[x][y].ok = false;
							num_quads--;
						}
					}
				}
			}

			timer_count--;
		}
		// Draw

		// Clear the screen
		SDL_FillRect(screen, NULL, 0); //SDL_MapRGB(screen->format, 0,0,0));

		// Draw the glass pieces
		for(y=0; y<QUADS_Y; y++) {
			for(x=0; x<QUADS_X; x++) {
				if(quad[x][y].ok) {
					// Set up rects
					SDL_Rect sr, dr;
					sr.x = quad[x][y].px;
					sr.y = quad[x][y].py;
					sr.w = quad_w;
					sr.h = quad_h;

					dr.x = (int)quad[x][y].x;
					dr.y = (int)quad[x][y].y;
					dr.w = quad_w;
					dr.h = quad_h;

					SDL_BlitSurface(bmp, &sr, screen, &dr);
				}
			}
		}


		// Flip to screen
		SDL_Flip(screen);
	}

	SDL_FreeSurface(bmp);
}


////////////////////////////////////////////

// Wipe lines
class WIPE_LINE {
public:

	float x;			// Position
	float speed;		// Speed (and dir)

	bool ok;			// Does the line exist?
};

// Lines
WIPE_LINE wipe_line[SCREEN_H];



// Wipe (sort of)
void wipe_screen() {

	// Fade out the music
	if(config.sound)
		Mix_FadeOutMusic(1000);

	// Grab the screen
	SDL_Surface *bmp;
	bmp = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_W, SCREEN_H, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, 0);
	if(bmp == NULL)
		// Yeah, not that I wanted to display the effect...
		return;

	SDL_BlitSurface(screen, NULL, bmp, NULL);

	// Initialize lines
	int num_lines = 0;
	int y;

	for(y=0; y<SCREEN_H; y++) {
		wipe_line[y].x = 0;
		//wipe_line[y].speed = (y%2) ? 1 : -1;
		wipe_line[y].speed = (y%2) ? (RANDF(-0.3,-0.1)) : (RANDF(0.1,0.3));
		wipe_line[y].ok = true;
		num_lines++;
	}

	// Loop
	timer_count = 0;
	while(num_lines > 0) {
		while(timer_count > 0) {
			// Move wipe lines
			// I had stupid bug here, I used SCREEN_W
			// (patch by Tim O'Brien)
			for(y=0; y<SCREEN_H; y++) {
				wipe_line[y].x += wipe_line[y].speed;
				wipe_line[y].speed *= 1.1;

				// Check if out of screen
				if(wipe_line[y].x <= -640 || wipe_line[y].x >= 640) {
					wipe_line[y].ok = false;
					num_lines--;
				}
			}

			timer_count--;
		}
		// Draw

		// Clear the screen
		SDL_FillRect(screen, NULL, 0); //SDL_MapRGB(screen->format, 0,0,0));

		// Draw the lines
		for(y=0; y<SCREEN_H; y++) {
			if(wipe_line[y].ok) {
					// Set up rects
					SDL_Rect sr, dr;
					sr.x = 0;
					sr.y = y;
					sr.w = SCREEN_W;
					sr.h = 1;

					dr.x = (int)wipe_line[y].x;
					dr.y = y;
					dr.w = SCREEN_W;
					dr.h = 1;

					SDL_BlitSurface(bmp, &sr, screen, &dr);
			}
			
		}


		// Flip to screen
		SDL_Flip(screen);
	}

	// Clear the screen
	SDL_FillRect(screen, NULL, 0); //SDL_MapRGB(screen->format, 0,0,0));
	SDL_Flip(screen);

	SDL_FreeSurface(bmp);

}


