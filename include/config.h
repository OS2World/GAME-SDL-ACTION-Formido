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

#ifndef CONFIG_H
#define CONFIG_H

// Config structure
struct CONFIG {
	int vid_color_depth;	// Video mode color depth
	int fullscreen;			// Fullscreen (1) or window (0)
	int sound;				// Sound enabled (1) or disabled(0)
	int sound_freq;			// Sound frequency
	int music_vol;			// Music volume (0-128)
	int show_fps;			// Show fps amount?
	// Player keys
	int key_up;
	int key_down;
	int key_left;
	int key_right;
	int key_shoot;
	int key_special;
};

// Load config from file
void load_config(char *file, CONFIG *conf);

// Save config to file
void save_config(char *file, CONFIG *conf);

#endif

