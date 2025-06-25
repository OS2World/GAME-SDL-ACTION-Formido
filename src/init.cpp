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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "init.h"
#include "config.h"


// Keep a global pointer to the screen
SDL_Surface *screen;


// Keep a global pointer to the config
CONFIG config;



// Display an error message and quit
void error_msg(char *msg, ...) {

	char *buf = new char[4096];

	// Format the msg to buf
	va_list ap;
	va_start(ap, msg);
	vsprintf(buf, msg, ap);
	va_end(ap);

	// Output to stderr
	fprintf(stderr, "%s\n", buf);

	// Display the message
	// Use MessageBoxes on Windows, printf on others
#ifdef WIN32
	SDL_Quit();
	MessageBox(HWND_DESKTOP, buf, "Error appeared", MB_OK|MB_ICONERROR);
#else
	SDL_Quit();
	printf("Error appeared:\n - %s\n", buf);
#endif

	delete [] buf;
	exit(-1);
}


// Initialize SDL
void init_sdl() {

	// Load the config
	//load_config("formido.cfg", &config);
	load_config(CONFIG("formido.cfg"), &config);

	// Initialize SDL with video and audio support
	if(config.sound) {
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER) < 0)
			error_msg("Unable to init SDL: %s", SDL_GetError());
	}
	else {
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
			error_msg("Unable to init SDL: %s", SDL_GetError());
	}

	atexit(SDL_Quit);

	// Load a icon
	// SDL_WM_SetIcon(IMG_Load(DAT("icon.dat")), NULL);  // Useless, with Mac OS X

	// Hide the mouse cursor
	SDL_ShowCursor(0);

	// Pass all keyboard input directly to window
	// (patch by Tim O'Brien)
#ifndef WIN32
	SDL_WM_GrabInput(SDL_GRAB_ON);
#endif

	// Set window caption
	SDL_WM_SetCaption("Formido", "Formido");

	// Start up the mixer
	if(config.sound) {
		// A guy named Paul suggested using 512 instead of 4096,
		// which was causing sound lag on Linux. It works now, thanks.
#ifndef WIN32
		if(Mix_OpenAudio(config.sound_freq, AUDIO_S16, 2, 512))
			error_msg("Unable to open audio!\n%s", Mix_GetError());
#else
		if(Mix_OpenAudio(config.sound_freq, AUDIO_S16, 2, 4096))
			error_msg("Unable to open audio!\n%s", Mix_GetError());
#endif
		Mix_VolumeMusic(config.music_vol);
	}

	// Set the video mode
	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, config.vid_color_depth, (config.fullscreen) ? (SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN) : (SDL_HWSURFACE|SDL_DOUBLEBUF));
	if(screen == NULL)
		error_msg("Unable to set video mode 640x480 %dbpp!\n%s", config.vid_color_depth, SDL_GetError());

	// Load the palette if we're in 8-bit mode
	if(screen->format->palette) {
		SDL_Surface *pal = IMG_Load(DAT("palette.dat"));
		if(pal == NULL)
			error_msg("Unable to load palette!\n%s", IMG_GetError());

		SDL_SetColors(screen, pal->format->palette->colors, 0, pal->format->palette->ncolors);
		SDL_FreeSurface(pal);
	}

}


// Hack function which adds a prefix
// to a string.
// Used in DAT(x), HISCORE(x) and CONFIG(x) macros and DEFCONFIG(x)
char *prefix_tmp = NULL;
char *add_prefix(char *str, char *prefix) {

	if(!prefix_tmp) {
		prefix_tmp = new char[512];
		if(!prefix_tmp)
			return ("null");	// Not enough memory
	}

	sprintf(prefix_tmp, "%s/%s", prefix, str);

	return prefix_tmp;
}


char *add_home_prefix(char *str, char *prefix) {

	if(!prefix_tmp) {
		prefix_tmp = new char[512];
		if(!prefix_tmp)
			return ("null");	// Not enough memory
	}

	sprintf(prefix_tmp, "%s/%s/%s", getenv("HOME"), prefix, str);

	return prefix_tmp;
}
