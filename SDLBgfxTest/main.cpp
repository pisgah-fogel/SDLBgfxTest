
// Assigning video driver to this platform (ie compiler's defines)
// Use 'gcc -dM -E - <NUL:' or 'gcc -dM -E - </dev/null' to check what is your current defines
#if unix || __unix || __unix__ || __linux__ || linux || __linux || __FreeBSD__
	#define SDL_VIDEO_DRIVER_X11
    #define SDL_VIDEO_DRIVER_WAYLAND
#elif __APPLE__ || __MACH__
    #define SDL_VIDEO_DRIVER_COCOA
    #define SDL_VIDEO_DRIVER_UIKIT
#elif _WIN32 || _WIN64 
	#define SDL_VIDEO_DRIVER_WINDOWS
    #define SDL_VIDEO_DRIVER_WINRT
#elif __ANDROID__
    #define SDL_VIDEO_DRIVER_ANDROID
#endif

// I don't know which system implements those, Uncomment to use them (You'll get error if you don't have the driver's library)
//#define SDL_VIDEO_DRIVER_DIRECTFB
#define SDL_VIDEO_DRIVER_MIR
#define SDL_VIDEO_DRIVER_VIVANTE

#include <cassert>
#include <iostream>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "SDL.hpp"
#include "Window.hpp"
#include "BgfxWrapper.hpp"

int main(int argc, char** argv)
{
    SDL::Init();
    {
        my::Window window;
        if (!window.Create("SDL & bgfx", 800, 600))
        {
            return -1;
        }

        BgfxWrapper::Init(window);

        int counter = 0;

        bool looping = true;
        while (looping)
        {
            SDL_Event event;
            while (SDL::PollEvent(event))
            {
                switch (event.type)
                {
				case SDL_QUIT:
				{
                    looping = false;
					break;
				}
                case SDL_WINDOWEVENT: 
                {
                    switch (event.window.event) 
                    {
                    case SDL_WINDOWEVENT_CLOSE:
                        looping = false;
                        break;
                    }
                } break;
                }
            }
            if (!window.IsOpen())
            {
                break;
            }

            bgfx::touch(BgfxWrapper::kClearView);
			bgfx::dbgTextClear();
			bgfx::frame();
        }

        BgfxWrapper::Release();
        window.Close();
    }
    
    SDL::Release();

    std::cout<<"Exiting without sigfault"<<std::endl;

	return 0;
}