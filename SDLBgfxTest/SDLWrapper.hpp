#pragma once

#include <cassert>

#include <SDL.h>

// TODO : https://alexandre-laurent.developpez.com/tutoriels/sdl-2/les-evenements/

class SDLWrapper
{
public:
	static bool Init(unsigned int initFlags = SDL_INIT_EVERYTHING)
	{
		SDLWrapper& sdl = GetInstance();

		assert(!sdl.mInitialized);

		sdl.mInitialized = SDL_Init(initFlags) >= 0;
		if (sdl.mInitialized)
		{
			sdl.mInitFlags = initFlags;
			return true;
		}
		else
		{
			std::cout<<"SDL::Init: Error: SDL_Init returned False"<<std::endl;
			sdl.mInitFlags = 0;
			return false;
		}
	}

	static bool IsInitialized()
	{
		return GetInstance().mInitialized;
	}

	static bool Release()
	{
		SDLWrapper& sdl = GetInstance();

		assert(sdl.mInitialized);

		SDL_Quit();
		sdl.mInitialized = false;

		return true;
	}

	static bool PollEvent(SDL_Event& event)
	{
		return GetInstance().mInitialized && SDL_PollEvent(&event) > 0;
	}

	static bool WaitEvent(SDL_Event& event, int timeoutMs = -1)
	{
		return GetInstance().mInitialized && SDL_WaitEventTimeout(&event, timeoutMs) > 0;
	}

	static const char* GetError()
	{
		return SDL_GetError();
	}

	static unsigned int GetInitFlags()
	{
		return GetInstance().mInitFlags;
	}

private:
	SDLWrapper()
		: mInitialized(false)
		, mInitFlags(0)
	{
	}

	~SDLWrapper()
	{
		assert(!mInitialized);
	}

	static SDLWrapper& GetInstance()
	{
		static SDLWrapper instance;
		return instance;
	}

	bool mInitialized;
	unsigned int mInitFlags;
};
