#include <cassert>

#include "BgfxWrapper.hpp"
#include "Window.hpp"

bool my::Window::Create(const char* name, int width, int height, unsigned int flags)
{
    mWindow = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    
    if (!mWindow)
        std::cout<<"my::Window::Create: SDL_CreateWindow Failed"<<std::endl;

    BgfxWrapper::Init(this);
    
    return mWindow != nullptr;
}

void my::Window::Close()
{
    if (mWindow != nullptr)
    {
        BgfxWrapper::Release();
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
}