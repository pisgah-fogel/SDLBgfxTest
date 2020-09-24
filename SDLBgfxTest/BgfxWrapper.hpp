#pragma once

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

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "Window.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

using namespace my;

class BgfxWrapper
{
public:
	static constexpr bgfx::ViewId kClearView = 0;
    static constexpr unsigned int kClearColor = 0x443355FF;

	static bool Init(my::Window* window)
	{
        assert(!GetInstance().mInitialized);

		SDL_SysWMinfo wmi;
		SDL_VERSION(&wmi.version);
		if (!SDL_GetWindowWMInfo(window->getSDLWindow(), &wmi))
		{
			return false;
		}

		bgfx::PlatformData pd;

        // Runtime selection of the driver to use (available driver are set according to the target's platform ie compiler's defines)
        switch (wmi.subsystem) {
            case SDL_SYSWM_UNKNOWN:
                std::cout<<"Platform: Unknown"<<std::endl;
                std::cout<<"Warning: SDL could not detect your platform, the program may crash"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = NULL;
            break;
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
            case SDL_SYSWM_WINDOWS:
                std::cout<<"Platform: WINDOWS"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.win.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_WINRT)
            case SDL_SYSWM_WINRT:
                std::cout<<"Platform: WINDOWS RT"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.winrt.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
            case SDL_SYSWM_X11:
                std::cout<<"Platform: X11"<<std::endl;
                pd.ndt = wmi.info.x11.display;
                pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_DIRECTFB)
            case SDL_SYSWM_DIRECTFB:
                std::cout<<"Platform: DIRECTFB"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.dfb.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_COCOA) // OSX
            case SDL_SYSWM_COCOA:
                std::cout<<"Platform: COCOA (OSX)"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.cocoa.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_UIKIT) // iOS
            case SDL_SYSWM_UIKIT:
                std::cout<<"Platform: UIKIT (iOS)"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.uikit.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
            case SDL_SYSWM_WAYLAND:
                std::cout<<"Platform: WAYLAND"<<std::endl;
                pd.ndt = NULL; // Not tested
                pd.nwh = (void*)(uintptr_t)wmi.info.wl.display;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_MIR)  /* no longer available, left for API/ABI compatibility. Remove in 2.1! */
            case SDL_SYSWM_MIR:
                std::cout<<"Platform: MIR"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = NULL;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_ANDROID)
            case SDL_SYSWM_ANDROID:
                std::cout<<"Platform: ANDROID"<<std::endl;
                pd.ndt = NULL;
                pd.nwh = wmi.info.android.window;
            break;
#endif
#if defined(SDL_VIDEO_DRIVER_VIVANTE) // STEAMLINK
            case SDL_SYSWM_VIVANTE:
                std::cout<<"Platform: VIVANTE (Steamlink)"<<std::endl;
                pd.ndt = wmi.info.vivante.display;
                pd.nwh = (void*)(uintptr_t)wmi.info.vivante.window;
            break;
#endif  
            default:
                std::cout<<"No video driver found (During runtime)"<<std::endl;
                std::cout<<"Detected system window manager: "<< wmi.subsystem <<std::endl;
                exit(1);
        }

		pd.context = NULL;
		pd.backBuffer = NULL;
		pd.backBufferDS = NULL;
		bgfx::setPlatformData(pd);

        bgfx::renderFrame();

		bgfx::Init init;
		init.type = bgfx::RendererType::Count; // Automatically choose a renderer
		init.resolution.width = window->GetWidth();
		init.resolution.height = window->GetHeight();
		init.resolution.reset = BGFX_RESET_VSYNC;
		bgfx::init(init);

		bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);

		bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, kClearColor, 1.0f, 0);
		bgfx::setViewRect(kClearView, 0, 0, static_cast<uint16_t>(window->GetWidth()), static_cast<uint16_t>(window->GetHeight()));

        GetInstance().mInitialized = true;
        return true;
	}

    static bool IsInitialized()
    {
        return GetInstance().mInitialized;
    }

    static bool Release()
    {
		assert(GetInstance().mInitialized);
		GetInstance().mInitialized = false;
        bgfx::shutdown();
		return true;
    }

private:
    BgfxWrapper()
        : mInitialized(false)
    {
    }

    ~BgfxWrapper()
    {
        assert(!mInitialized);
    }

    static BgfxWrapper& GetInstance()
    {
        static BgfxWrapper instance;
        return instance;
    }

    bool mInitialized;
};