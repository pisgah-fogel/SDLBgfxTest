
// Use compiler defines to choose which driver to compile for
// Use 'gcc -dM -E - <NUL:' or 'gcc -dM -E - </dev/null' to check what is your platform's defines
#if unix || __unix || __unix__ || __linux__ || linux || __linux || __FreeBSD__
	#define SDL_VIDEO_DRIVER_X11
    //#define SDL_VIDEO_DRIVER_WAYLAND
#elif __APPLE__ || __MACH__
    #define SDL_VIDEO_DRIVER_COCOA
    #define SDL_VIDEO_DRIVER_UIKIT
#elif _WIN32 || _WIN64 
	#define SDL_VIDEO_DRIVER_WINDOWS
    #define SDL_VIDEO_DRIVER_WINRT
#elif __ANDROID__
    #define SDL_VIDEO_DRIVER_ANDROID
#endif // TODO: add SDL_VIDEO_DRIVER_VIVANTE

#include <cassert>
#include <iostream>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "SDL.hpp"
#include "Window.hpp"

class Bgfx
{
public:
	static constexpr bgfx::ViewId kClearView = 0;
    static constexpr unsigned int kClearColor = 0x443355FF;

	static bool Init(my::Window& window)
	{
        assert(!GetInstance().mInitialized);

		SDL_SysWMinfo wmi;
		SDL_VERSION(&wmi.version);
		if (!SDL_GetWindowWMInfo(window.getSDLWindow(), &wmi))
		{
			return false;
		}

		bgfx::PlatformData pd;


#if defined(SDL_VIDEO_DRIVER_WINDOWS)
        std::cout<<"Platform: WINDOWS"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.win.window;
#elif defined(SDL_VIDEO_DRIVER_WINRT)
        std::cout<<"Platform: WINDOWS RT"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.winrt.window;
#elif defined(SDL_VIDEO_DRIVER_X11)
        std::cout<<"Platform: X11"<<std::endl;
        pd.ndt = wmi.info.x11.display;
		pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif defined(SDL_VIDEO_DRIVER_DIRECTFB)
        std::cout<<"Platform: DIRECTFB"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.dfb.window;
#elif defined(SDL_VIDEO_DRIVER_COCOA) // OSX
        std::cout<<"Platform: COCOA (OSX)"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.cocoa.window;
#elif defined(SDL_VIDEO_DRIVER_UIKIT) // iOS
        std::cout<<"Platform: UIKIT (iOS)"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.uikit.window;
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
        std::cout<<"Platform: WAYLAND"<<std::endl;
        pd.ndt = NULL; // Not tested
		pd.nwh = (void*)(uintptr_t)wmi.info.wl.display;
#elif defined(SDL_VIDEO_DRIVER_MIR)  /* no longer available, left for API/ABI compatibility. Remove in 2.1! */
        std::cout<<"Platform: MIR"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = NULL;
#elif defined(SDL_VIDEO_DRIVER_ANDROID)
        std::cout<<"Platform: ANDROID"<<std::endl;
        pd.ndt = NULL;
		pd.nwh = wmi.info.android.window;
#elif defined(SDL_VIDEO_DRIVER_VIVANTE) // STEAMLINK
        std::cout<<"Platform: VIVANTE (Steamlink)"<<std::endl;
        pd.ndt = wmi.info.vivante.display;
		pd.nwh = wmi.info.vivante.window;
#else
        //#error No video driver found
        std::cout<<"No video driver found"<<std::endl;
        std::cout<<"Detected system window manager: "<< wmi.subsystem <<std::endl;
        exit(1);
#endif

		pd.context = NULL;
		pd.backBuffer = NULL;
		pd.backBufferDS = NULL;
		bgfx::setPlatformData(pd);

        bgfx::renderFrame();

		bgfx::Init init;
		init.type = bgfx::RendererType::Count; // Automatically choose a renderer
		init.resolution.width = window.GetWidth();
		init.resolution.height = window.GetHeight();
		init.resolution.reset = BGFX_RESET_VSYNC;
		bgfx::init(init);

		bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);

		bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, kClearColor, 1.0f, 0);
		bgfx::setViewRect(kClearView, 0, 0, static_cast<uint16_t>(window.GetWidth()), static_cast<uint16_t>(window.GetHeight()));

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
    Bgfx()
        : mInitialized(false)
    {
    }

    ~Bgfx()
    {
        assert(!mInitialized);
    }

    static Bgfx& GetInstance()
    {
        static Bgfx instance;
        return instance;
    }

    bool mInitialized;
};

int main(int argc, char** argv)
{
    SDL::Init();
    {
        my::Window window;
        if (!window.Create("SDL & bgfx", 800, 600))
        {
            return -1;
        }

        Bgfx::Init(window);

        int counter = 0;
        while (window.IsOpen())
        {
            SDL_Event event;
            while (SDL::PollEvent(event))
            {
                switch (event.type)
                {
				case SDL_QUIT:
				{
					window.Close();
					break;
				}
                case SDL_WINDOWEVENT: 
                {
                    switch (event.window.event) 
                    {
                    case SDL_WINDOWEVENT_CLOSE:
                        window.Close();
                        break;
                    }
                } break;
                }
            }
            if (!window.IsOpen())
            {
                break;
            }

            bgfx::touch(Bgfx::kClearView);
			bgfx::dbgTextClear();
			bgfx::frame();
        }

        Bgfx::Release();
    }
    SDL::Release();
	return 0;
}