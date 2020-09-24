#include <cassert>
#include <iostream>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "BgfxWrapper.hpp"
#include "SDLWrapper.hpp"
#include "Window.hpp"

#include "Cube.hpp"


int main(int argc, char** argv)
{
    SDLWrapper::Init();
    {
        my::Window window;
        if (!window.Create("SDL & bgfx", 800, 600))
        {
            return -1;
        }

        Cube test_cube;

        int counter = 0;

        bool looping = true;
        while (looping)
        {
            SDL_Event event;
            while (SDLWrapper::PollEvent(event))
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

            //bgfx::touch(BgfxWrapper::kClearView);


            test_cube.render();

			bgfx::dbgTextClear();
            bgfx::dbgTextPrintf(0, 1, 0x4f, "Counter:%d", counter++);
			bgfx::frame();
        }
        window.Close();
    }
    
    SDLWrapper::Release();

    std::cout<<"No segfault, we are good !"<<std::endl;

	return 0;
}