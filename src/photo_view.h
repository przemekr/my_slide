class PhotoView : public AppView
{
public:
   PhotoView(App& app): AppView(app)
   {
      resized = true;
      nextIdx = 0;
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

      int w = app.rbuf_window().width();
      int h = app.rbuf_window().height();
      double s = 1+time/100000*scale;

      agg::trans_affine img_mtx;
      img_mtx.reset();
      img_mtx *= agg::trans_affine_scaling(s);
      img_mtx *= agg::trans_affine_translation(moveX*(1-s)*w, moveY*(1-s)*h);
      img_mtx.invert();

      typedef agg::span_interpolator_linear<> interpolator_type;
      typedef agg::image_accessor_clip<pixfmt_type> img_source_type;
      typedef agg::span_image_filter_rgba_2x2<img_source_type,
              interpolator_type> span_gen_type;
      interpolator_type interpolator(img_mtx);
      agg::span_allocator<color_type> sa;

      pixfmt_type img_pixf(app.rbuf_img(currIdx));
      img_source_type img_src(img_pixf, agg::rgba_pre(0.1, 0.9, 0.9, 0.1));
      agg::image_filter<agg::image_filter_kaiser> filter;
      span_gen_type sg(img_src, interpolator, filter);
      ras.move_to_d(0,0);
      ras.line_to_d(w,0);
      ras.line_to_d(w,h);
      ras.line_to_d(0,h);
      agg::render_scanlines_aa(ras, sl, rbase, sa, sg);
      agg::rect_i rec(0, 0, w, h);

      if (time > 2000)
      {
         dim = (time-2000)/3000*128;
         pixfmt_type pf2(app.rbuf_img(nextIdx));
         rbase.blend_from(pf2, 
               &rec,
               0,
               0,
               dim);
      }
   }

   void enter()
   {
      extern int maxPhotoIdx;
      wait_mode(false);
      currIdx = nextIdx;
      if (nextIdx < maxPhotoIdx)
         nextIdx++;
      if (nextIdx == maxPhotoIdx && allPhotosLoaded)
         nextIdx = 0;
      DEBUG_PRINT("photo view enter\n");

      scale =  1+(double)(rand()%100)/10000;
      dim = 0;
      switch (rand()%4)
      {
         case 0:
            moveX =  0;
            moveY =  0;
            break;
         case 1:
            moveX =  0;
            moveY =  1;
            break;
         case 2:
            moveX =  1;
            moveY =  0;
            break;
         case 3:
            moveX =  1;
            moveY =  1;
            break;
      }
      time = 0;
   }

   void on_resize(int, int)
   {
      resized = true;
   }
   int max_fps() { return 15; }

private:
   void update(long elapsed_time)
   {
      time += elapsed_time;
      if (time > 5000)
      {
         app.changeView(std::rand()%3?"PhotoView":"TextView");
      }
   }
   bool resized;
   int currIdx;
   double moveX;
   double moveY;
   double scale;
   double time;
   double dim;
};
