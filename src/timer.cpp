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
#include "timer.h"
#include "init.h"


// Timer
int timer_count;
SDL_TimerID timer;


// Is the game paused?
bool game_paused;


// Timer callback
Uint32 timer_callback(Uint32 interval, void *param) {
	if(!game_paused)
		timer_count++;
	return interval;
}



// Start a timer
void start_timer(int fps) {

	// Add a timer to run at 'fps' frames per second
	timer_count = 0;
	game_paused = false;
	timer = SDL_AddTimer((1000/fps), timer_callback, 0);
	if(timer == NULL)
		error_msg("Error setting timer!\n%s", SDL_GetError());

}


// Destroy a timer
void kill_timer() {

	SDL_RemoveTimer(timer);
}

