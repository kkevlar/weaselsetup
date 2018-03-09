#include "weasel.h"

#define MAX_DEVICE_COUNT 24
#define NUM_DIGITAL_BUTTONS_TESTED 12
#define MAX_GC 4

#ifndef COMB_TEST

#include <signal.h>

static volatile int should_continue_listening_for_digital_input = 1;

void intHandler(int dummy) {
    should_continue_listening_for_digital_input = 0;
}

void fill_joystick_list(SDL_Joystick** joys)
{    
    const int n = SDL_NumJoysticks();
    for (int i = 0; i < n; ++i) 
    {
        *(joys+i) = SDL_JoystickOpen(i);
        if (! SDL_JoystickNameForIndex(i))
            *(joys+i) = 0;
    }
}

int listen_for_joystick_buttonpress(SDL_Joystick** joystick_list, 
    int* joystick_id_list, 
    int curr_joystick_id_list_index)
{
    should_continue_listening_for_digital_input = 1;
    
    while(should_continue_listening_for_digital_input)
    {
        SDL_JoystickUpdate();
        for(int i = 0; 
            should_continue_listening_for_digital_input &&
            i < MAX_DEVICE_COUNT;
            i++)
        {
            if (joystick_list[i] && 
                SDL_JoystickNameForIndex(i))
            {
                int searchfor = 0;
                for (int g = 0; g < MAX_GC; g++)
                    searchfor += joystick_id_list[g] == i ? 1 : 0;

                for (int curr_digital_button_id = 0;
                    should_continue_listening_for_digital_input &&
                    !searchfor &&
                    curr_digital_button_id < NUM_DIGITAL_BUTTONS_TESTED;
                    curr_digital_button_id++)
                {
                    if(SDL_JoystickGetButton(joystick_list[i], curr_digital_button_id))
                    {
                        joystick_id_list[curr_joystick_id_list_index] = i;
                        should_continue_listening_for_digital_input = 0;
                        break;
                    }
                }
            }
        }
    }
    return 0;
}


int main(int argc, char **argv)
{   
    if (argc != 2)
    {
        printf("Terminating due to unexpected number of command line args.");
        return 1;
    }

    SDL_Joystick* joystick_list[MAX_DEVICE_COUNT];

    int joystick_id_list[MAX_GC * 2];
    int curr_joystick_id_list_index = 0;    
    int full_gamecube_controller_count = **(argv+ argc-1) - '0';

    if (full_gamecube_controller_count > MAX_GC || 
        full_gamecube_controller_count < 1)
    {
        printf("Terminating due to unrealistic number of expected GC Pads.");
        return 1;
    }

    for (int i = 0; i < MAX_GC*2; i++)
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

   signal(SIGINT, intHandler);    

    for (curr_joystick_id_list_index = 0;
        curr_joystick_id_list_index < full_gamecube_controller_count*2;
        curr_joystick_id_list_index++)
    {
        char* left_or_right_string = 
            (curr_joystick_id_list_index/2)*2 
            == curr_joystick_id_list_index ?
            "LEFT" : "RIGHT";
        printf("Press button on the %s half of P%d...\n",
            left_or_right_string,
            (curr_joystick_id_list_index/2)+1);
        #ifndef SKIP_REAL_JOYSTICKS
            listen_for_joystick_buttonpress(joystick_list, 
                joystick_id_list, 
                curr_joystick_id_list_index);
        #else
            joystick_id_list[curr_joystick_id_list_index] = curr_joystick_id_list_index*curr_joystick_id_list_index+1;
        #endif
    }

    for (int i = 0; i < MAX_GC * 2; i++)
    {
        printf("%d ",joystick_id_list[i]);
        // if (joystick_id_list[i] %2== 0)
        //     joystick_id_list[i]++;
        // else if (joystick_id_list[i] %2== 1)
        //     joystick_id_list[i]--;
        printf("%d\n",joystick_id_list[i]);


    }

    for(int g = 0; g < full_gamecube_controller_count; g++)
    {
        char namebuffer[128];
        sprintf(namebuffer, "../../Dolphin Emulator/Config/Profiles/GCPad/%d-weaselout.ini", g+1);
        combine_configurations(
            "../../Dolphin Emulator/Config/Profiles/GCPad/smashL.ini",
            "../../Dolphin Emulator/Config/Profiles/GCPad/smashR.ini",
            joystick_id_list[g*2 + 0],
            joystick_id_list[g*2 + 1],
            namebuffer);
    }


    printf("Quitting SDL... ");
    SDL_Quit();
    printf("Quit Successful\nGoodbye!\n\n");

    
    return 0;

}
#endif
