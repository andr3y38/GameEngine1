// SDL_MAIN_HANDLED indicates the program has its own main function
// Do not use the SDL_main method - allowing us to define custom behaviour
#define SDL_MAIN_HANDLED
#include "GameEngine.h"
#include <sstream>

using namespace GE;

int main()
{
    // Create the game engine object
    GameEngine ge;

    // Initilise the game engine object
    if (!ge.init()) {
        display_info_message("Couldn't start SDL. Check console output for error logs");
        return -1;
    }

    // Store time at two points in the program
    Uint32 last_time = SDL_GetTicks(), current_time = 0;
    int frame_count = 0;

    //MAIN LOOP
    while (ge.keep_running()) {
        // Update game state
        ge.update();

        ge.processInput();

        // Render the frame to the window and calc frame statistics
        ge.draw();
        frame_count++;
        current_time = SDL_GetTicks();

        if (current_time - last_time > 1000) {
            // string to hold frame timing and construct this message
            std::ostringstream msg;
            msg << "FPS = " << frame_count;
            ge.setwindowtitle(msg.str().c_str());
            // reset the frame counter
            frame_count = 0;
            // update time between frames
            last_time = current_time;
        }
    }

    // Destroy game engine
    ge.shutdown();

    return 0;
}