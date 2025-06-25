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
#include <stdio.h>
#include "config.h"
#include "init.h"

// Load config from file
void load_config(char *file, CONFIG *conf) {

	FILE *f = fopen(file, "rt");
	if(!f)
	{
		printf("Opening default config...\n");
		f = fopen(DEFCONFIG("formido.cfg"), "rt");
		if(!f)
		{
			error_msg("Unable to load config file: %s!", file);
		}
	}
		
		
	fscanf(f, "video_mode_color_depth = %d\n", &(conf->vid_color_depth));
	fscanf(f, "video_mode_fullscreen = %d\n", &(conf->fullscreen));
	fscanf(f, "sound_enabled = %d\n", &(conf->sound));
	fscanf(f, "sound_freq = %d\n", &(conf->sound_freq));
	fscanf(f, "music_volume = %d\n", &(conf->music_vol));
	fscanf(f, "show_fps = %d\n", &(conf->show_fps));
	fscanf(f, "key_up = %d\n", &(conf->key_up));
	fscanf(f, "key_down = %d\n", &(conf->key_down));
	fscanf(f, "key_left = %d\n", &(conf->key_left));
	fscanf(f, "key_right = %d\n", &(conf->key_right));
	fscanf(f, "key_shoot = %d\n", &(conf->key_shoot));
	fscanf(f, "key_special = %d\n", &(conf->key_special));
	fclose(f);
}


// Save config to file
void save_config(char *file, CONFIG *conf) {

	FILE *f = fopen(file, "wt");
	if(!f)
	{
		system("mkdir -p ~/.formido");	// Pouah, hard encoded, baaaad...
		f = fopen(file, "wt");
		if(!f)
		{
			error_msg("Unable to save config file: %s!", file);
		}
	}

	fprintf(f, "video_mode_color_depth = %d\n", (conf->vid_color_depth));
	fprintf(f, "video_mode_fullscreen = %d\n", (conf->fullscreen));
	fprintf(f, "sound_enabled = %d\n", (conf->sound));
	fprintf(f, "sound_freq = %d\n", (conf->sound_freq));
	fprintf(f, "music_volume = %d\n", (conf->music_vol));
	fprintf(f, "show_fps = %d\n", (conf->show_fps));
	fprintf(f, "key_up = %d\n", (conf->key_up));
	fprintf(f, "key_down = %d\n", (conf->key_down));
	fprintf(f, "key_left = %d\n", (conf->key_left));
	fprintf(f, "key_right = %d\n", (conf->key_right));
	fprintf(f, "key_shoot = %d\n", (conf->key_shoot));
	fprintf(f, "key_special = %d\n", (conf->key_special));

	fclose(f);
}

