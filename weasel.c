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



    // while(true)
    // {
    //     SDL_JoystickUpdate();
    //     for(int i = 0; i < MAX_DEVICE_COUNT; i++)
    //     {
    //         if (joystick_list[i] && SDL_JoystickNameForIndex(i) )
    //         {
    //             for (int axis = 0; axis < 24; axis++)
    //             {
    //                 int but =  SDL_JoystickGetButton(controllers[i], axis);
    //                 if(but)
    //                     printf("j=%d i=%d axis=%d button=%d\n",
    //                     i,
    //                     axis,
    //                     SDL_JoystickGetAxis(controllers[i], axis),
    //                     SDL_JoystickGetButton(controllers[i], axis));
    //             }
    //         }
    //     }
    // }

    // double radio_input[16] = {0};


    // while(ctrl) 
    // {
    //     SDL_JoystickUpdate();
    //     for (int i = 0; i < 16; i++)
    //     {
    //              radio_input[i] = SDL_JoystickGetAxis(ctrl, i) / 32768.0;
    //              printf("A%d:%.3f ",i,radio_input[i]);
    //     }

    //     for (int i = 0; i < 5; i++)
    //     {
    //         printf("D%i:%d ",i,SDL_JoystickGetButton(ctrl, i));
    //     }
    //     printf("\n");
    //     //cpSleep(1000);        
    // }

    printf("Quitting SDL... ");
    SDL_Quit();
    printf("Quit Successful\nGoodbye!\n\n");

    /*
    int motor_index = 0;
    int mode_index = 1;
    int hat_last = 0;

    cassie_t *c = cassie_init();
    cassie_vis_t *v = cassie_vis_init();
    cassie_state_t *s = cassie_state_alloc();
    cassie_get_state(c, s);

    double radio_input[16] = {0};
    radio_input[6] = 1;
    radio_input[8] = 1;

    double messages[4] = {-1, -1, -1, -1};

    do {
        int steps_per_frame = SDL_JoystickGetButton(xbox, 0) ? 3 : 33;

        for (int i = 0; i < steps_per_frame; ++i)
            cassie_step(c, radio_input);

        cassie_messages(c, messages);
        printf("%.0f, %.0f, %.0f, %.0f\n", messages[0], messages[1], messages[2], messages[3]);

        if (xbox) {
            SDL_JoystickUpdate();
            radio_input[0] = -SDL_JoystickGetAxis(xbox, 1) / 32768.0;
            radio_input[1] =  SDL_JoystickGetAxis(xbox, 0) / 32768.0;
            radio_input[2] = -SDL_JoystickGetAxis(xbox, 4) / 32768.0;
            radio_input[3] =  SDL_JoystickGetAxis(xbox, 3) / 32768.0;
            radio_input[6] = -SDL_JoystickGetAxis(xbox, 2) / 32768.0;
            radio_input[7] = -SDL_JoystickGetAxis(xbox, 5) / 32768.0;

            int hat = SDL_JoystickGetHat(xbox, 0);
            if (hat & 0x1 && !(hat_last & 0x01)) ++mode_index;
            if (hat & 0x2 && !(hat_last & 0x02)) ++motor_index;
            if (hat & 0x4 && !(hat_last & 0x04)) --mode_index;
            if (hat & 0x8 && !(hat_last & 0x08)) --motor_index;
            mode_index = (mode_index + 3) % 3;
            motor_index = (motor_index + 5) % 5;
            hat_last = hat;

            if (SDL_JoystickGetButton(xbox, 7))
                cassie_set_state(c, s);

            radio_input[4] = motor_index / 4.0;
            radio_input[9] = mode_index - 1.0;
        }
    } while (cassie_vis_draw(v, c));

    cassie_vis_free(v);
    cassie_free(c);
    cassie_cleanup();
    
    SDL_Quit();
    */

    return 0;

}
