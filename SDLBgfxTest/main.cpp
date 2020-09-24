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

        // TODO: initialize the cube here
        bgfx::VertexBufferHandle vbh;
        bgfx::IndexBufferHandle ibh;
        cube_vertexDeclaration(&vbh, &ibh);

        bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
        bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
        bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

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


            const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
            const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
            float view[16];
            bx::mtxLookAt(view, eye, at);
            float proj[16];
            bx::mtxProj(proj, 60.0f, 800.f/600.f, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            float mtx[16];
            bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
            bgfx::setTransform(mtx);        

            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);

        //bgfx::touch(0); // draw an empty primitive

            bgfx::submit(0, program);

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