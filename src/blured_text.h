#include "agg_blur.h"

class TextView : public AppView
{
public:
   TextView(App& app): AppView(app)
   {
      resized = true;
   }

   void on_mouse_button_up(int x, int y, unsigned flags)
   {
      if (m_ctrls.on_mouse_button_up(x, y))
      {
         app.on_ctrl_change();
         app.force_redraw();
      }
   }

   void on_mouse_button_down(int x, int y, unsigned flags)
   {
      if (m_ctrls.on_mouse_button_down(x, y))
      {
         app.on_ctrl_change();
         app.force_redraw();
      }
   }

   void on_mouse_move(int x, int y, unsigned flags)
   {
      if (m_ctrls.on_mouse_move(x, y, (flags & agg::mouse_left) != 0))
      {
         app.on_ctrl_change();
         app.force_redraw();
         return;
      }
   }

   void on_ctrl_change()
   {
   }

   void on_multi_gesture(float x, float y,
         float dTheta, float dDist, int numFingers)
   {
   }

   void on_draw()
   {
      pixfmt_type pf(app.rbuf_window());;
      agg::renderer_base<pixfmt_type> rbase(pf);
      agg::rasterizer_scanline_aa<> ras;
      agg::scanline_u8 sl;
      ras.reset();

      agg::rendering_buffer m_rbuf2;
      agg::pixfmt_bgra32 pixf2(m_rbuf2);
      int w = app.width();
      int h = app.height();

      double y = 210;
      double x = 50;
      
      agg::rgba blue(0.0, 0, 0.9, 0.8-0.8*(time/4000));
      app.draw_text(x+0.3*blur/3.0, y+blur/3.0, w/15, blue, 1.0,
            "Happy Birthday");

      pixf2.attach(pf, 0, y-50, w, y+350);
      agg::stack_blur_rgba32(pixf2,
            blur, 
            blur);
      blur += 0.3;
   }

   void enter()
   {
      wait_mode(false);
      time = 0;
      blur = 0;
   }

   void on_resize(int, int)
   {
      resized = true;
   }
   int max_fps() { return 7; }

private:
   agg::stack_blur  <agg::rgba8, agg::stack_blur_calc_rgb<> >     m_stack_blur;
   void update(long elapsed_time)
   {
      time += elapsed_time;
      if (time > 4000)
      {
         app.changeView("PhotoView");
      }
   }
   bool resized;
   int photoIdx;
   double moveX;
   double moveY;
   double scale;
   double time;
   double blur;
   double x;
   double y;
};
