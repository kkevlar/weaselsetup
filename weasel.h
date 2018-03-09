
#ifndef WEASEL_H
#define WEASEL_H

#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <string.h>
#include "config_combiner.h"

#ifndef COMB_TEST

	#include <signal.h>

	static volatile int should_continue_listening_for_digital_input = 1;

	void intHandler(int dummy);
	void fill_joystick_list(SDL_Joystick** joys);
	int listen_for_joystick_buttonpress(SDL_Joystick** joystick_list, 
	    int* joystick_id_list, 
	    int curr_joystick_id_list_index);
	int main(int argc, char **argv);

	#endif
	
#endif
