/*
 * agg_timer, a simple arithmetic game.
 * Copyright 2014 Przemyslaw Rzepecki
 * Contact: przemekr@sdfeu.org
 * 
 * This file is part of agg_timer.
 * 
 * agg_timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * agg_timer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * agg_timer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <algorithm> 
#include <stack>
#include <unistd.h>
#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_ellipse.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_amask_adaptor.h"
#include "agg_span_allocator.h"
#include "agg_span_gradient.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_pattern_filters_rgba.h"
#include "agg_pixfmt_rgba.h"
#include "agg_image_accessors.h"
#include "agg_renderer_outline_image.h"
#include "agg_renderer_outline_aa.h"
#include "agg_rasterizer_outline_aa.h"
#include "agg_conv_transform.h"
#include "agg_color_rgba.h"
#include "agg_gsv_text.h"
#include "agg_button_ctrl.h"
#include "platform/agg_platform_support.h"
#include <unistd.h>
#include <stdint.h>
#include "stdio.h"
#include "time.h"
#include "stdarg.h"

#include <time.h>

#ifdef MOBILE
#define START_H  1024
#define START_W  760
#define WINDOW_FLAGS agg::window_fullscreen | agg::window_keep_aspect_ratio
#else
#define START_H  800
#define START_W  550
#define WINDOW_FLAGS agg::window_resize | agg::window_hw_buffer
#endif

#define CTRL_TEXT_THICKNESS 3

enum flip_y_e { flip_y = true };
const agg::rgba transp(0, 0, 0, 0);
const agg::rgba dblue(0,0,200,128);
const agg::rgba lblue(0,0,0.9,1);
const agg::rgba lgreen(0,0.9,0,1);
const agg::rgba lgray(60,60,60);
const agg::rgba green(0,120,0);
const agg::rgba yellow(63,63,0);
const agg::rgba red(0.9,0,0,0.7);
const agg::rgba black(0,0,0);
typedef agg::rgba8             color_type;
typedef agg::pixfmt_bgra32     pixfmt_type;
typedef agg::pixfmt_bgra32_pre pixfmt_pre;
typedef agg::renderer_base<pixfmt_type> renderer_base_type;
typedef agg::renderer_base<pixfmt_pre> renderer_base_pre;
typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
typedef agg::line_image_pattern<agg::pattern_filter_bilinear_rgba8> pattern_type;
typedef agg::renderer_outline_image<renderer_base_type, pattern_type> renderer_type;
typedef agg::rasterizer_outline_aa<renderer_type> rasterizer_type;


class View
{
public:
   virtual void enter() {}
   virtual void on_draw() {}
   virtual void on_resize(int sx, int sy) {}
   virtual void on_idle() {}
   virtual void on_mouse_move(int x, int y, unsigned flags) {}
   virtual void on_multi_gesture(float dTheta, float dDist, float x,
         float y, int numFinders) {}
   virtual void on_touch_event(float x, float y, float dx, float dy, int id, bool down) {}
   virtual void on_mouse_button_down(int x, int y, unsigned flags) {}
   virtual void on_mouse_button_up(int x, int y, unsigned flags) {}
   virtual void on_key(int x, int y, unsigned key, unsigned flags) {}
   virtual void on_ctrl_change() {}
   agg::ctrl_container m_ctrls;
   void add_ctrl(agg::ctrl& c) { m_ctrls.add(c);}
};

class App: public agg::platform_support
{
public:
   App(agg::pix_format_e format, bool flip_y) :
      agg::platform_support(format, flip_y)
   {
      sound = false;
      music = false;
      in_background = false;
      start_timer();
      current_sound = 2;
   }

   bool sound_on() { return sound; }
   bool music_on() { return music; }
   void sound_on(bool on) { sound = on; }
   void music_on(bool on) { music = on; }

   virtual void changeView(const char* name) {};

   void draw_text(double x, double y, double size, const char* str)
   {
      agg::rgba c(0.8, 0, 0, 0.6);
      draw_text(x, y, size, c, 1.0, str);
   }


   void draw_text(double x, double y, double size, agg::rgba& c, double tickness,
         const char* fmt, ...)
   {
      va_list ap;
      va_start(ap, fmt);
      char str[1024];
      vsnprintf(str, sizeof(str), fmt, ap);
      va_end(ap);

      agg::rasterizer_scanline_aa<> m_ras;
      agg::scanline_p8              m_sl;
      pixfmt_type pf(rbuf_window());
      agg::renderer_base<pixfmt_type> rb(pf);
      agg::renderer_scanline_aa_solid<agg::renderer_base<pixfmt_type> > ren(rb);

      agg::gsv_text txt;
      agg::conv_stroke<agg::gsv_text> txt_stroke(txt);
      txt_stroke.width(size/6*tickness);
      txt_stroke.line_cap(agg::round_cap);
      txt.size(size);
      txt.start_point(x, y);
      txt.text(str);
      m_ras.add_path(txt_stroke);
      ren.color(c);
      agg::render_scanlines(m_ras, m_sl, ren);
   }

   virtual void on_ctrl_change()
   {
      view->on_ctrl_change();
   }

   virtual void on_resize(int nx, int ny)
   {
      view->on_resize(nx, ny);
   }

   virtual void on_idle()
   {
      view->on_idle();
   }

   virtual void on_mouse_button_up(int x, int y, unsigned flags)
   {
      view->on_mouse_button_up(x, y, flags);
   }

   virtual void on_mouse_button_down(int x, int y, unsigned flags)
   {
      view->on_mouse_button_down(x, y, flags);
   }

   virtual void on_mouse_move(int x, int y, unsigned flags)
   {
      view->on_mouse_move(x, y, flags);
   }

   virtual void on_multi_gesture(float x, float y,
         float dTheta, float dDist, int numFingers)
   {
      view->on_multi_gesture(x, y, dTheta, dDist, numFingers);
   }
   virtual void on_touch_event(float x, float y, float dx, float dy,
         int id, bool down)
   {
      view->on_touch_event(x, y, dx, dy, id, down);
   }

   virtual void on_draw()
   {
      if (!in_background)
         view->on_draw();
   }

   void enter_background()
   {
      in_background = true;
   }

   void enter_foreground()
   {
      in_background = false;
   }
   int  current_sound;

protected:
   View* view;
   bool sound;
   bool music;
   bool in_background;
};




class AppView: public View
{
public:
   void on_idle()
   {
      int loop_time = app.elapsed_time();
      if (loop_time < 1000/max_fps())
      {
         usleep(1000*(1000/max_fps()-loop_time));
      }
      update(app.elapsed_time());
      app.start_timer();
      app.force_redraw();
   }
protected:
   AppView(App& application): app(application) {}
   virtual int max_fps() { return 20; }
   virtual void update(long elapsed_time) {}

   int width()          { return app.width(); }
   int height()         { return app.height(); }
   void start_timer()   { app.start_timer(); }
   long elapsed_time()  { return app.elapsed_time(); }
   void update_window() { app.update_window(); }
   void force_redraw()  { app.force_redraw(); }
   bool wait_mode()     { return app.wait_mode(); }
   void wait_mode(bool v) {  app.wait_mode(v); }
   agg::rendering_buffer& rbuf_window() { return app.rbuf_window(); }
   void message(const char* buf)   { app.message(buf); }

   App& app;
};


