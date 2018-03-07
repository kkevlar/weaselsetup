#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <string.h>

#define MAX_DEVICE_COUNT 24
#define MAX_GC 4

void fill_joystick_list(SDL_Joystick** joys)
{    
    const int n = SDL_NumJoysticks();
    for (int i = 0; i < n; ++i) 
        *(joys+i) = SDL_JoystickOpen(i);
        if (! SDL_JoystickNameForIndex(i))
            *(joys+i) = 0;
}


int main(int argc, char **argv)
{   
    if (argc != 2)
        return 1;

    SDL_Joystick* joystick_list[MAX_DEVICE_COUNT];

    int joystick_id_list[MAX_GC * 2];
    int curr_joystick_id_list_index = 0;    
    int full_gamecube_controller_count = **(argv+ argc-1) - '0';

    if (full_gamecube_controller_count > MAX_GC || 
        full_gamecube_controller_count < 1)
    {
        return;
    }

    for (int i = 0; i < MAX_GC; i++)
        joystick_id_list[i] = -1;

    printf("\nWill set up %d full gamecubes.\n", 
        full_gamecube_controller_count);
    printf("Initializing SDL... ");
    SDL_Init(SDL_INIT_JOYSTICK);
    printf("Initialized\n");
    printf("Initializing joysticks... ");
    fill_joystick_list(joystick_list);
    printf("Initialized\n");

    printf("\nHere is a list of the joysticks connected: \n");

    SDL_JoystickUpdate();

    for(int i = 0; i < MAX_DEVICE_COUNT; i++)
    {
        if (joystick_list[i] && SDL_JoystickNameForIndex(i) )
        {
            printf("  - %d: %s\n",i,SDL_JoystickNameForIndex(i));
        }
    }

    int should_continue_listening_for_digital_input = 1;

    #warning "This loop incomplete"
    while(should_continue_listening_for_digital_input)
    {
        SDL_JoystickUpdate();
        for(int i = 0; 
            should_continue_listening_for_digital_input &&
            i < MAX_DEVICE_COUNT;
            i++)
        {
            if (joystick_list[i] && SDL_JoystickNameForIndex(i) )
            {
                int test = 0;
                for (int g = 0; g < MAX_GC; g++)
                    test += joystick_id_list[g] == i ? 1 : 0;
                for (int id = 0; should_continue_listening_for_digital_input && !test && id < 24; id++)
                {
                    int but =  SDL_JoystickGetButton(joystick_list[i], id);
                    if(but)
                    {
                        joystick_id_list[curr_joystick_id_list_index] = i;
                        should_continue_listening_for_digital_input = 0;
                        break;
                    }
                }
            }
        }
    }
    printf("\n");


    printf("Quitting SDL... ");
    SDL_Quit();
    printf("Quit Successful\nGoodbye!\n\n");

    
    return 0;

}
