//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Copyright (C) 2004 Mauricio Piacentini (SDL Support)
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------

#include <string.h>
#include <errno.h>
#include "platform/agg_platform_support.h"
#include "SDL.h"
#include "SDL_surface.h"
#include "SDL_endian.h"
#include "SDL_image.h"
#include "SDL_mixer.h" 

#if __ANDROID__
extern "C"
{
   extern int Android_ScreenWidth;
   extern int Android_ScreenHeight;
}
#endif


static double width_factor = 1;
static double hight_factor = 1;

namespace agg
{
    //------------------------------------------------------------------------
    class platform_specific
    {
    public:
        platform_specific(pix_format_e format, bool flip_y);
        ~platform_specific();

        pix_format_e  m_format;
        pix_format_e  m_sys_format;
        bool          m_flip_y;
        unsigned      m_bpp;
        unsigned      m_pformat;
        unsigned      m_sys_bpp;
        unsigned      m_rmask;
        unsigned      m_gmask;
        unsigned      m_bmask;
        unsigned      m_amask;
        bool          m_update_flag;
        bool          m_resize_flag;
        bool          m_initialized;
        SDL_Window*   m_mwindow;
        SDL_Surface*  m_surface;
        SDL_Texture*  m_texture;
        SDL_Renderer* m_renderer;
        SDL_Surface*  m_surf_img[platform_support::max_images];
        Mix_Music*    m_music[platform_support::max_images];
        Mix_Chunk*    m_sounds[platform_support::max_images];
        int           m_cur_x;
        int           m_cur_y;
        int           m_sw_start;
    };

     


    //------------------------------------------------------------------------
    platform_specific::platform_specific(pix_format_e format, bool flip_y) :
        m_format(format),
        m_sys_format(pix_format_undefined),
        m_flip_y(flip_y),
        m_bpp(0),
        m_sys_bpp(0),
        m_update_flag(true), 
        m_resize_flag(true),
        m_initialized(false),
        m_mwindow(0),
        m_surface(0),
        m_texture(0),
        m_renderer(0),
        m_cur_x(0),
        m_cur_y(0)
    {
        memset(m_surf_img, 0, sizeof(m_surf_img));
        memset(m_music, 0, sizeof(m_music));
        memset(m_sounds, 0, sizeof(m_sounds));

        m_pformat = 0;

        switch(m_format)
        {
        case pix_format_gray8:
            m_bpp = 8;
            break;

        case pix_format_rgb565:
            m_rmask = 0xF800;
            m_gmask = 0x7E0;
            m_bmask = 0x1F;
            m_amask = 0;
            m_bpp = 16;
            break;

        case pix_format_rgb555:
            m_rmask = 0x7C00;
            m_gmask = 0x3E0;
            m_bmask = 0x1F;
            m_amask = 0;
            m_bpp = 16;
            break;
			
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
        case pix_format_rgb24:
            m_rmask = 0xFF;
            m_gmask = 0xFF00;
            m_bmask = 0xFF0000;
            m_amask = 0;
            m_bpp = 24;
            m_pformat = SDL_PIXELFORMAT_RGB24;
            break;

        case pix_format_bgr24:
            m_rmask = 0xFF0000;
            m_gmask = 0xFF00;
            m_bmask = 0xFF;
            m_amask = 0;
            m_bpp = 24;
            m_pformat = SDL_PIXELFORMAT_BGR24;
            break;

        case pix_format_bgra32:
            m_rmask = 0xFF0000;
            m_gmask = 0xFF00;
            m_bmask = 0xFF;
            m_amask = 0xFF000000;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ARGB8888;
            break;

        case pix_format_abgr32:
            m_rmask = 0xFF000000;
            m_gmask = 0xFF0000;
            m_bmask = 0xFF00;
            m_amask = 0xFF;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ABGR8888;
            break;

        case pix_format_argb32:
            m_rmask = 0xFF00;
            m_gmask = 0xFF0000;
            m_bmask = 0xFF000000;
            m_amask = 0xFF;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ARGB8888;
            break;

        case pix_format_rgba32:
            m_rmask = 0xFF;
            m_gmask = 0xFF00;
            m_bmask = 0xFF0000;
            m_amask = 0xFF000000;
            m_pformat = SDL_PIXELFORMAT_RGBA8888;
            m_bpp = 32;
            break;
#else //SDL_BIG_ENDIAN (PPC)
        case pix_format_rgb24:
            m_rmask = 0xFF0000;
            m_gmask = 0xFF00;
            m_bmask = 0xFF;
            m_amask = 0;
            m_bpp = 24;
            m_pformat = SDL_PIXELFORMAT_RGB24;
            break;

        case pix_format_bgr24:
            m_rmask = 0xFF;
            m_gmask = 0xFF00;
            m_bmask = 0xFF0000;
            m_amask = 0;
            m_bpp = 24;
            m_pformat = SDL_PIXELFORMAT_BGR24;
            break;

        case pix_format_bgra32:
            m_rmask = 0xFF00;
            m_gmask = 0xFF0000;
            m_bmask = 0xFF000000;
            m_amask = 0xFF;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ARGB8888;
            break;

        case pix_format_abgr32:
            m_rmask = 0xFF;
            m_gmask = 0xFF00;
            m_bmask = 0xFF0000;
            m_amask = 0xFF000000;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ABGR8888;
            break;

        case pix_format_argb32:
            m_rmask = 0xFF0000;
            m_gmask = 0xFF00;
            m_bmask = 0xFF;
            m_amask = 0xFF000000;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_ARGB8888;
            break;

        case pix_format_rgba32:
            m_rmask = 0xFF000000;
            m_gmask = 0xFF0000;
            m_bmask = 0xFF00;
            m_amask = 0xFF;
            m_bpp = 32;
            m_pformat = SDL_PIXELFORMAT_RGBA8888;
            break;
#endif
        }
    }

    //------------------------------------------------------------------------
    platform_specific::~platform_specific()
    {
        int i;
        for(i = platform_support::max_images - 1; i >= 0; --i)
        {
            if(m_surf_img[i]) SDL_FreeSurface(m_surf_img[i]);
        }
        if(m_texture) SDL_DestroyTexture(m_texture);
        if(m_renderer) SDL_DestroyRenderer(m_renderer);
        if(m_surface) SDL_FreeSurface(m_surface);
        if(m_mwindow) SDL_DestroyWindow(m_mwindow);
    }



    //------------------------------------------------------------------------
    platform_support::platform_support(pix_format_e format, bool flip_y) :
        m_specific(new platform_specific(format, flip_y)),
        m_format(format),
        m_bpp(m_specific->m_bpp),
        m_window_flags(0),
        m_wait_mode(true),
        m_flip_y(flip_y)
    {
       SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
       DEBUG_PRINT("platform support");
       strcpy(m_caption, "Anti-Grain Geometry Application");

       if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG)
       {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                Mix_GetError());
       }
    

       /*Initialize SDL_mixer */
       if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
       {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                Mix_GetError());
       }
    }


    //------------------------------------------------------------------------
    platform_support::~platform_support()
    {
        delete m_specific;
    }


    //------------------------------------------------------------------------
    void platform_support::caption(const char* cap)
    {
        strcpy(m_caption, cap);
        if(m_specific->m_initialized)
        {
            SDL_SetWindowTitle(m_specific->m_mwindow, cap);
        }
    }
    


    //------------------------------------------------------------------------
    bool platform_support::init(unsigned width, unsigned height, unsigned flags)
    {
        ERROR_PRINT("platform_support::init %d,%d\n", width, height);
        m_window_flags = flags;
        int wflags = 0;

        if(m_specific->m_texture) SDL_DestroyTexture(m_specific->m_texture);
        if(m_specific->m_renderer) SDL_DestroyRenderer(m_specific->m_renderer);
        if(m_specific->m_surface) SDL_FreeSurface(m_specific->m_surface);
        //if(m_specific->m_mwindow) SDL_DestroyWindow(m_specific->m_mwindow);
        m_specific->m_texture = 0;
        m_specific->m_renderer = 0;
        m_specific->m_surface = 0;
        //m_specific->m_mwindow = 0;

        if(m_window_flags & window_resize)
        {
            wflags |= SDL_WINDOW_RESIZABLE;
        }
        if(m_window_flags & window_fullscreen)
        {
            wflags |= SDL_WINDOW_FULLSCREEN;
#ifdef __ANDROID__DISABLED
            width = Android_ScreenWidth;
            height = Android_ScreenHeight;
#endif

#ifdef __ANDROID__
            if (m_window_flags & window_keep_aspect_ratio)
            {
               width = (double)Android_ScreenWidth/Android_ScreenHeight*height;
               width_factor = (double)width/Android_ScreenWidth;
               hight_factor = (double)height/Android_ScreenHeight;
            }
#endif
        }
        DEBUG_PRINT("platform_support::init %d,%d,%d", width, height, wflags);

        int numRendDrv = SDL_GetNumRenderDrivers();
        DEBUG_PRINT("num rend drv %d\n", numRendDrv);

        for (int i = 0; i < numRendDrv; i++)
        {
           SDL_RendererInfo info;
           SDL_GetRenderDriverInfo(i, &info);
           DEBUG_PRINT("index %i, %s, flags %x, texture formats %x\n", i, info.name, info.flags,
                 info.texture_formats[0]);
        }

        if (!m_specific->m_mwindow)
        {
           m_specific->m_mwindow = SDL_CreateWindow(m_caption,
                 SDL_WINDOWPOS_UNDEFINED,
                 SDL_WINDOWPOS_UNDEFINED,
                 width, height,
                 wflags);
        }

        if (m_specific->m_mwindow == 0) 
        {
            ERROR_PRINT( 
                    "Unable to create %dx%d %d bpp window: %s\n", 
                    width, 
                    height, 
                    m_bpp, 
                    SDL_GetError());
            return false;
        }

        m_specific->m_surface = SDL_CreateRGBSurface(0, width, height,
              m_bpp,
              m_specific->m_rmask, 
              m_specific->m_gmask, 
              m_specific->m_bmask, 
              m_specific->m_amask);
        DEBUG_PRINT("surface at %p", m_specific->m_surface);

        if(m_specific->m_surface == 0) 
        {
            ERROR_PRINT( 
                    "Unable to create image buffer %dx%d %d bpp: %s\n", 
                    width, 
                    height, 
                    m_bpp, 
                    SDL_GetError());
            return false;
        }

        m_specific->m_renderer = SDL_CreateRenderer(m_specific->m_mwindow,
              -1, 0);
        if(m_specific->m_renderer == 0) 
        {
            ERROR_PRINT( 
                    "Unable to create renderer: %s\n", 
                    SDL_GetError());
            return false;
        }

        {
           SDL_RendererInfo info;
           SDL_GetRendererInfo(m_specific->m_renderer, &info);
           DEBUG_PRINT("Current, %s, flags %x, texture formats %x, %x\n", info.name, info.flags,
                 SDL_PIXELFORMAT_ARGB8888, info.texture_formats[0]);
        }

        m_specific->m_texture = SDL_CreateTexture(
              m_specific->m_renderer,
              m_specific->m_pformat,
              SDL_TEXTUREACCESS_STREAMING, width, height);
        if(m_specific->m_renderer == 0) 
        {
            ERROR_PRINT( 
                    "Unable to create texture: %s\n", 
                    SDL_GetError());
            return false;
        }

        void* pixels;
        int pitch;
        if (SDL_LockTexture(m_specific->m_texture, NULL, &pixels, &pitch) < 0)
        {
           SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't lock texture: %s\n",
                 SDL_GetError());
        }

        m_rbuf_window.attach((unsigned char*)pixels, 
                             width, height, 
                             m_flip_y ? -pitch : pitch);


        if (!m_specific->m_initialized)
        {
            m_initial_width = width;
            m_initial_height = height;
            on_init();
            m_specific->m_initialized = true;
        }
        on_resize(m_rbuf_window.width(), m_rbuf_window.height());
        m_specific->m_update_flag = true;
        return true;
    }



    //------------------------------------------------------------------------
    void platform_support::update_window()
    {
       SDL_UnlockTexture(m_specific->m_texture);
       SDL_RenderClear(m_specific->m_renderer);
       SDL_RenderCopy(m_specific->m_renderer, m_specific->m_texture, NULL, NULL);
       SDL_RenderPresent(m_specific->m_renderer);
       void* pixels;
       int pitch;
       if (SDL_LockTexture(m_specific->m_texture, NULL, &pixels, &pitch) < 0)
       {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Couldn't lock texture: %s\n",
                SDL_GetError());
       }
    }


    //------------------------------------------------------------------------
    int platform_support::run()
    {
        SDL_Event event;
        bool ev_flag = false;

        for(;;)
        {
            if(m_specific->m_update_flag)
            {
                on_draw();
                update_window();
                m_specific->m_update_flag = false;
            }

            ev_flag = false;
            if(m_wait_mode)
            {
                SDL_WaitEvent(&event);
                ev_flag = true;
            }
            else
            {
                if(SDL_PollEvent(&event))
                {
                    ev_flag = true;
                }
                else
                {
                    on_idle();
                }
            }

            if(ev_flag)
            {
                if(event.type == SDL_QUIT)
                {
                    break;
                }

                int y;
                unsigned flags = 0;

                switch (event.type) 
                {
                   int event_x;
                   int event_y;

                   case SDL_WINDOWEVENT:
                      switch (event.window.event)
                      {
                         case SDL_WINDOWEVENT_RESIZED:
                            ERROR_PRINT("SDL_WINDOWEVENT_RESIZED %d,%d\n", event.window.data1, event.window.data2);
#ifndef __ANDROID__
                            if(!init(event.window.data1, event.window.data2, m_window_flags)) return false;
                            on_resize(m_rbuf_window.width(), m_rbuf_window.height());
                            trans_affine_resizing(event.window.data1, event.window.data2);
#endif
                            m_specific->m_update_flag = true;
                            break;
                         default:
                            DEBUG_PRINT("unknown win event type %d\n", event.window.event);
                            break;
                      }
                      break;

                case SDL_APP_DIDENTERFOREGROUND:
                   DEBUG_PRINT("SDL_APP_DIDENTERFOREGROUND");
                   enter_foreground();
                   break;

                case SDL_APP_WILLENTERFOREGROUND:
                   DEBUG_PRINT("SDL_APP_WILLENTERFOREGROUND");
                   break;

                case SDL_APP_DIDENTERBACKGROUND:
                   DEBUG_PRINT("SDL_APP_DIDENTERBACKGROUND");
                   enter_background();
                   break;

                case SDL_APP_WILLENTERBACKGROUND:
                   DEBUG_PRINT("SDL_APP_WILLENTERBACKGROUND");
                   break;

                case SDL_KEYDOWN:
                    {
                        flags = 0;
                        if(event.key.keysym.mod & KMOD_SHIFT) flags |= kbd_shift;
                        if(event.key.keysym.mod & KMOD_CTRL)  flags |= kbd_ctrl;

                        bool left  = false;
                        bool up    = false;
                        bool right = false;
                        bool down  = false;

                        switch(event.key.keysym.sym)
                        {
                        case key_left:
                            left = true;
                            break;

                        case key_up:
                            up = true;
                            break;

                        case key_right:
                            right = true;
                            break;

                        case key_down:
                            down = true;
                            break;
                        }

                        if(m_ctrls.on_arrow_keys(left, right, down, up))
                        {
                            on_ctrl_change();
                            force_redraw();
                        }
                        else
                        {
                            on_key(m_specific->m_cur_x,
                                   m_specific->m_cur_y,
                                   event.key.keysym.sym,
                                   flags);
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    event_x = (double)event.button.x*width_factor;
                    event_y = (double)event.button.y*hight_factor;
                    y = m_flip_y ? 
                        m_rbuf_window.height() - event_y : 
                        event_y;

                    m_specific->m_cur_x = event_x;
                    m_specific->m_cur_y = y;
                    flags = 0;
                    if(event.motion.state & SDL_BUTTON_LMASK) flags |= mouse_left;
                    if(event.motion.state & SDL_BUTTON_RMASK) flags |= mouse_right;

                    if(m_ctrls.on_mouse_move(m_specific->m_cur_x, 
                                             m_specific->m_cur_y,
                                             (flags & mouse_left) != 0))
                    {
                        on_ctrl_change();
                        force_redraw();
                    }
                    else
                    {
                        on_mouse_move(m_specific->m_cur_x, 
                                      m_specific->m_cur_y, 
                                      flags);
                    }
		    SDL_Event eventtrash;
		    while (SDL_PeepEvents(&eventtrash, 1, SDL_GETEVENT, SDL_MOUSEMOTION, SDL_MOUSEMOTION)!=0){;}
                    break;

		case SDL_MOUSEBUTTONDOWN:
                    event_x = (double)event.button.x*width_factor;
                    event_y = (double)event.button.y*hight_factor;
                    DEBUG_PRINT("mouse down: %d,%d %d,%d\n", event.button.x,event.button.y,
                          event_x,event_y);

                    y = m_flip_y
                        ? m_rbuf_window.height() - event_y
                        : event_y;

                    m_specific->m_cur_x = event_x;
                    m_specific->m_cur_y = y;
                    flags = 0;
                    DEBUG_PRINT("mouse down: %d,%d w %d,%d\n", event_x,y,
                          m_rbuf_window.width(), m_rbuf_window.height());
                    switch(event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        {
                            flags = mouse_left;

if(m_ctrls.on_mouse_button_down(m_specific->m_cur_x,
                                m_specific->m_cur_y))
                            {
                                m_ctrls.set_cur(m_specific->m_cur_x, 
                                    m_specific->m_cur_y);
                                on_ctrl_change();
                                force_redraw();
                            }
                            else
                            {
                                if(m_ctrls.in_rect(m_specific->m_cur_x, 
                                    m_specific->m_cur_y))
                                {
                                    if(m_ctrls.set_cur(m_specific->m_cur_x, 
                                        m_specific->m_cur_y))
                                    {
                                        on_ctrl_change();
                                        force_redraw();
                                    }
                                }
                                else
                                {
                                    on_mouse_button_down(m_specific->m_cur_x, 
                                        m_specific->m_cur_y, 
                                        flags);
                                }
                            }
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                        DEBUG_PRINT("SDL_BUTTON_RIGHT");
                        flags = mouse_right;
                        on_mouse_button_down(m_specific->m_cur_x, 
                            m_specific->m_cur_y, 
                            flags);
                        break;
                    } //switch(event.button.button)
                    break;
		    
                case SDL_MOUSEBUTTONUP:
                    event_x = (double)event.button.x*width_factor;
                    event_y = (double)event.button.y*hight_factor;
                    y = m_flip_y
                        ? m_rbuf_window.height() - event_y
                        : event_y;

                    m_specific->m_cur_x = event_x;
                    m_specific->m_cur_y = y;
                    flags = 0;
                    if(m_ctrls.on_mouse_button_up(m_specific->m_cur_x, 
                                                  m_specific->m_cur_y))
                    {
                        on_ctrl_change();
                        force_redraw();
                    }
                    on_mouse_button_up(m_specific->m_cur_x, 
                                       m_specific->m_cur_y, 
                                       flags);
                    break;
                case SDL_MOUSEWHEEL:
                    break;
                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                case SDL_FINGERMOTION:
                    DEBUG_PRINT("Finger: x=%f, y=%f dx=%f, dy=%f, %d\n",
                          event.tfinger.x,
                          event.tfinger.y,
                          event.tfinger.dx,
                          event.tfinger.dy,
                          (int)event.tfinger.fingerId);
                    on_touch_event(
                          event.tfinger.x,
                          m_flip_y ? 1-event.tfinger.y: event.tfinger.y,
                          event.tfinger.dx,
                          event.tfinger.dy,
                          (int)event.tfinger.fingerId,
                          !(event.type == SDL_FINGERUP));
                    break;
                case SDL_MULTIGESTURE:
                    DEBUG_PRINT("Multi Gesture: x = %f, y = %f, dAng = %f, dR = %f numDownTouch = %i\n",
                          event.mgesture.x,
                          event.mgesture.y,
                          event.mgesture.dTheta,
                          event.mgesture.dDist,
                          event.mgesture.numFingers);
                    on_multi_gesture(
                          event.mgesture.x,
                          event.mgesture.y,
                          event.mgesture.dTheta,
                          event.mgesture.dDist,
                          event.mgesture.numFingers);
                    break;
                default:
                    DEBUG_PRINT("unknown event type %d\n", event.type);
                    break;
                }
            }
        }
        return 0;
    }



    //------------------------------------------------------------------------
    const char* platform_support::img_ext() const { return ".bmp"; }

    //------------------------------------------------------------------------
    const char* platform_support::full_file_name(const char* file_name)
    {
        return file_name;
    }

    FILE* platform_support::open_rw_file(const char* org,
          const char* app, const char* fname)
    {
       ERROR_PRINT("open_rw");
#ifdef __ANDROID__
       SDL_RWops* ops = SDL_RWFromFile(fname, "r+");
       if (ops)
       {
          return ops->hidden.stdio.fp;
       }
       ops = SDL_RWFromFile(fname, "w+");
       {
          return ops->hidden.stdio.fp;
       }
       return NULL;
#else
       char* path = SDL_GetPrefPath(org, app);
       if (!path)
       {
          ERROR_PRINT("Couldn't get path: %s\n", SDL_GetError());
          return NULL;
       }
       char filename[1024];
       strcpy(filename, path);
       strcat(filename, fname);
       FILE* f = fopen(filename, "r+");
       if (!f)
       {
          ERROR_PRINT("could not open file: %s\n", strerror(errno));
          f = fopen(filename, "w+");
          if (!f)
             ERROR_PRINT("could not open file: %s\n", strerror(errno));
       }
       SDL_free(path);
       return f;
#endif
    }

    //------------------------------------------------------------------------
    bool platform_support::load_img(unsigned idx, const char* file)
    {
        if(idx < max_images)
        {
            if(m_specific->m_surf_img[idx]) SDL_FreeSurface(m_specific->m_surf_img[idx]);

            char fn[1024];
            strcpy(fn, file);

            SDL_Surface* tmp_surf = IMG_Load(fn);
            if (tmp_surf == 0) 
            {
                ERROR_PRINT( "Couldn't load %s: %s\n", fn, SDL_GetError());
                return false;
            }

            SDL_PixelFormat format;
            format.palette = 0;
            format.BitsPerPixel = m_bpp;
            format.BytesPerPixel = m_bpp >> 8;
            format.Rmask = m_specific->m_rmask;
            format.Gmask = m_specific->m_gmask;
            format.Bmask = m_specific->m_bmask;
            format.Amask = m_specific->m_amask;
            format.Rshift = 0;
            format.Gshift = 0;
            format.Bshift = 0;
            format.Ashift = 0;
            format.Rloss = 0;
            format.Gloss = 0;
            format.Bloss = 0;
            format.Aloss = 0;

            m_specific->m_surf_img[idx] = 
                SDL_ConvertSurface(tmp_surf, 
                                   &format, 
                                   SDL_SWSURFACE);

            SDL_FreeSurface(tmp_surf);
            
            if(m_specific->m_surf_img[idx] == 0) return false;

            m_rbuf_img[idx].attach((unsigned char*)m_specific->m_surf_img[idx]->pixels, 
                                   m_specific->m_surf_img[idx]->w, 
                                   m_specific->m_surf_img[idx]->h, 
                                   m_flip_y ? -m_specific->m_surf_img[idx]->pitch : 
                                               m_specific->m_surf_img[idx]->pitch);
            return true;

        }
        return false;
    }

    //------------------------------------------------------------------------
    bool platform_support::load_music(unsigned idx, const char* file)
    {
       if (idx >= max_images)
       {
          return false;
       }
       if (m_specific->m_music[idx])
       {
          Mix_FreeMusic(m_specific->m_music[idx]);
       }
       m_specific->m_music[idx] = Mix_LoadMUS(file);
       if (m_specific->m_music[idx] == NULL)
       {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
          return false;
       }
       return true;
    }

    //------------------------------------------------------------------------
    bool platform_support::load_sound(unsigned idx, const char* file)
    {
       if (idx >= max_images)
       {
          return false;
       }
       if (m_specific->m_sounds[idx])
       {
          Mix_FreeChunk(m_specific->m_sounds[idx]);
       }
       m_specific->m_sounds[idx] = Mix_LoadWAV(file);
       if (m_specific->m_sounds[idx] == NULL)
       {
          SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
          return false;
       }
       return true;
    }

    void platform_support::play_sound(unsigned idx, unsigned vol)
    {
       if (idx >= max_images || !m_specific->m_sounds[idx])
       {
          return;
       }
       Mix_VolumeChunk(m_specific->m_sounds[idx], vol);
       Mix_PlayChannel( -1, m_specific->m_sounds[idx], 0 );
    }
    void platform_support::play_music(unsigned idx, unsigned vol)
    {
       if (idx >= max_images || !m_specific->m_music[idx])
       {
          return;
       }
       Mix_VolumeMusic(vol);
       if (Mix_PlayingMusic())
       {
          Mix_HaltMusic();
       } 
       if (vol)
       {
          Mix_PlayMusic(m_specific->m_music[idx], -1);
       }
    }

    //------------------------------------------------------------------------
    bool platform_support::save_img(unsigned idx, const char* file)
    {
        if(idx < max_images && m_specific->m_surf_img[idx])
        {
            char fn[1024];
            strcpy(fn, file);
            int len = strlen(fn);
            if(len < 4 || strcmp(fn + len - 4, ".bmp") != 0)
            {
                strcat(fn, ".bmp");
            }
            return SDL_SaveBMP(m_specific->m_surf_img[idx], fn) == 0;
        }
        return false;
    }



    //------------------------------------------------------------------------
    bool platform_support::create_img(unsigned idx, unsigned width, unsigned height)
    {
        if(idx < max_images)
        {

            if(m_specific->m_surf_img[idx]) SDL_FreeSurface(m_specific->m_surf_img[idx]);

             m_specific->m_surf_img[idx] = 
                 SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                      width, 
                                      height,
                                      m_specific->m_surface->format->BitsPerPixel,
                                      m_specific->m_rmask, 
                                      m_specific->m_gmask, 
                                      m_specific->m_bmask, 
                                      m_specific->m_amask);
            if(m_specific->m_surf_img[idx] == 0) 
            {
                ERROR_PRINT( "Couldn't create image: %s\n", SDL_GetError());
                return false;
            }

            m_rbuf_img[idx].attach((unsigned char*)m_specific->m_surf_img[idx]->pixels, 
                                   m_specific->m_surf_img[idx]->w, 
                                   m_specific->m_surf_img[idx]->h, 
                                   m_flip_y ? -m_specific->m_surf_img[idx]->pitch : 
                                               m_specific->m_surf_img[idx]->pitch);

            return true;
        }

        return false;
    }
    
    //------------------------------------------------------------------------
    void platform_support::start_timer()
    {
        m_specific->m_sw_start = SDL_GetTicks();
    }

    //------------------------------------------------------------------------
    double platform_support::elapsed_time() const
    {
        int stop = SDL_GetTicks();
        return double(stop - m_specific->m_sw_start);
    }

    //------------------------------------------------------------------------
    void platform_support::message(const char* msg)
    {
        DEBUG_PRINT( "%s\n", msg);
    }

    //------------------------------------------------------------------------
    void platform_support::force_redraw()
    {
        m_specific->m_update_flag = true;
    }


    //------------------------------------------------------------------------
    void platform_support::on_init() {}
    void platform_support::on_resize(int sx, int sy) {}
    void platform_support::on_idle() {}
    void platform_support::on_mouse_move(int x, int y, unsigned flags) {}
    void platform_support::on_mouse_button_down(int x, int y, unsigned flags) {}
    void platform_support::on_mouse_button_up(int x, int y, unsigned flags) {}
    void platform_support::on_multi_gesture(float dTheta, float dDist, float x,
          float y, int numFingers) {}
    void platform_support::on_touch_event(float x, float y, float dx, float dy,
              int id, bool down) {}
    void platform_support::on_key(int x, int y, unsigned key, unsigned flags) {}
    void platform_support::on_ctrl_change() {}
    void platform_support::on_draw() {}
    void platform_support::on_post_draw(void* raw_handler) {}


}


int agg_main(int argc, char* argv[]);

#ifdef __ANDROID__
#define MAIN SDL_main
#else 
#define MAIN main
#endif

int MAIN(int argc, char* argv[])
{
    return agg_main(argc, argv);
}
