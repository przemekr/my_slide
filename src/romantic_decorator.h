#include <math.h>
#include <stdlib.h>
#include "agg_blur.h"
#include "agg_path_storage.h"
#include "agg_conv_curve.h"
#define AR ((double)app.height()/app.width())


class FlowerLeaf
{
public:
   double rotation;
   double rotationD;
   double x;
   double y;
   double vx;
   double vy;
   double vxd;
   double vyd;

   FlowerLeaf(App& application):
      app(application)
   {
      reset();
      y = 1;
   }
   void reset()
   {
      x = 1;
      y = AR;
      vx = -0.006*rand()/RAND_MAX;
      vy = -0.006*rand()/RAND_MAX;
      rotation  = 0.1*(1-(double)rand()/RAND_MAX);
      rotationD = 0.03*(1-(double)rand()/RAND_MAX);
      vxd = -0.0001*rand()/RAND_MAX;
      vyd = -0.0001*rand()/RAND_MAX;
   }

   void update(long time)
   {
      rotation += rotationD;
      vx += vxd;
      vy += vyd;
      x += vx;
      y += vy;
      double mw = (double)app.rbuf_img(1).width()/app.width();
      double mh = (double)app.rbuf_img(1).height()/app.width();

      if ((x < -mw || y < -mh || x > 1+mw || y > AR+mh)
            && ((double)rand()/RAND_MAX < 0.01))
         reset();
   }

   void draw(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase
         )
   {
      int w = app.width();
      int h = app.height();

      static agg::trans_affine shape_mtx; shape_mtx.reset();
      shape_mtx *= agg::trans_affine_translation(x*w, y*w);
      int mw = app.rbuf_img(1).width();
      int mh = app.rbuf_img(1).height();
      if (x < -mw || y < -mh || x > 1+mw || y > AR+mh)
            return;

      agg::ellipse e;
      e.init(w*x, w*y, mw/2, mh/2, 30);

      agg::trans_affine img_mtx;
      img_mtx *= agg::trans_affine_translation(-mw/2, -mh/2);
      img_mtx *= agg::trans_affine_rotation(rotation);
      img_mtx *= agg::trans_affine_translation(w*x, w*y);
      img_mtx.invert();

      agg::span_allocator<agg::rgba8> sa;

      typedef agg::span_interpolator_linear<> interpolator_type;
      typedef agg::image_accessor_clip<pixfmt_type> img_source_type;
      typedef agg::span_image_filter_rgba_2x2<img_source_type,
              interpolator_type> span_gen_type;
      interpolator_type interpolator(img_mtx);

      pixfmt_type img_pixf(app.rbuf_img(1));
      img_source_type img_src(img_pixf, agg::rgba_pre(0, 0.4, 0, 0.5));
      agg::image_filter<agg::image_filter_kaiser> filter;
      span_gen_type sg(img_src, interpolator, filter);

      ras.add_path(e);
      agg::render_scanlines_aa(ras, sl, rbase, sa, sg);
   }
private:
   App& app;
};



class Heart
{
private:
    agg::stack_blur       <agg::rgba8, agg::stack_blur_calc_rgb<> >     m_stack_blur;
    agg::path_storage     m_path;
    typedef agg::conv_curve<agg::path_storage> shape_type;
    shape_type            m_shape;
    int blur;
    double xshift;
    double yshift;
    double scale;
    double alpha;
    App& app;

public:
    Heart(App& application):
       app(application),
       m_shape(m_path)
   {
      reset();
   }

    void reset()
    {
       alpha = 0.3;
       m_path.remove_all();
       m_path.move_to(111.6,180.1);
       m_path.curve4 (156.0,142.0,  139.5,129.3,  129.3,125.5);
       m_path.curve4 (121.8,122.7,  110.3,139.5,  110.3,139.5);
       m_path.curve4 (110.3,139.5,   96.7,126.8,   87.4,128.1);
       m_path.curve4 ( 80.0,129.1,   72.7,138.3,   73.5,145.8);
       m_path.curve4 ( 75.2,162.3,  111.6,180.1,  111.6,180.1);
       m_path.close_polygon();
       agg::trans_affine shape_mtx;
       xshift = rand()%200;
       yshift = 100-rand()%200;
       scale  = 2.0*rand()/RAND_MAX;
       shape_mtx *= agg::trans_affine_rotation(agg::pi);
       shape_mtx *= agg::trans_affine_translation(200+xshift, 400+yshift);
       shape_mtx *= agg::trans_affine_scaling(1.0* scale);
       m_path.transform(shape_mtx);
       blur = 0+rand()%4;
    }

    void move()
    {
       m_path.remove_all();
       m_path.move_to(111.6,180.1);
       m_path.curve4 (156.0,142.0,  139.5,129.3,  129.3,125.5);
       m_path.curve4 (121.8,122.7,  110.3,139.5,  110.3,139.5);
       m_path.curve4 (110.3,139.5,   96.7,126.8,   87.4,128.1);
       m_path.curve4 ( 80.0,129.1,   72.7,138.3,   73.5,145.8);
       m_path.curve4 ( 75.2,162.3,  111.6,180.1,  111.6,180.1);
       m_path.close_polygon();
       agg::trans_affine shape_mtx;
       xshift += 0.3;
       yshift += 0.3;
       blur += 1;
       shape_mtx *= agg::trans_affine_rotation(agg::pi);
       shape_mtx *= agg::trans_affine_translation(200+xshift, 400+yshift);
       shape_mtx *= agg::trans_affine_scaling(1.0* scale);
       m_path.transform(shape_mtx);
    }

    void draw(
          agg::rasterizer_scanline_aa<>& ras,
          agg::scanline_u8& sl,
          renderer_base_type& rbase
          )
    {
       agg::rgba red(0.9, 0.1, 0.1, alpha);
       agg::scanline_u8 msl;
       agg::rasterizer_scanline_aa<> mras;

       renderer_scanline_type ren_sl(rbase);
       ren_sl.color(red);
       mras.reset();
       mras.add_path(m_shape);
       agg::render_scanlines_aa_solid(mras, msl, rbase, red);
       pixfmt_type pf(app.rbuf_window());;

       agg::rendering_buffer m_rbuf2;
       agg::pixfmt_bgra32 pixf2(m_rbuf2);
       int w = app.width();
       int h = app.height();

       double x = 50;
       double y = 210;
       double bw = 90;
       double bh = 80;
       x += xshift;
       y += yshift;
       x *= scale;
       y *= scale;
       bw *= scale;
       bh *= scale;

       agg::render_scanlines_aa_solid(mras, msl, rbase, red);
       pixf2.attach(pf, x, y, x+bw, y+bh);

       agg::stack_blur_rgba32(pixf2,
             blur, 
             blur);

       if (blur > 30)
          reset();
       if (alpha < 0.9)
          alpha += 0.1;
       move();
    }
};

class RomanticDecorator: public Decorator
{
public:
   RomanticDecorator(App& app):
      Decorator(app), leaf1(app), leaf2(app), heart1(app), heart2(app)
    {}

   void update(long elapsed_time)
   {
      leaf1.update(elapsed_time);
      leaf2.update(elapsed_time);
   }

   void background(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase,
         bool resized
         )
   {
      static agg::rendering_buffer rbuf;
      int w = app.rbuf_window().width();
      int h = app.rbuf_window().height();

      if (resized)
      {
         static agg::trans_affine shape_mtx; shape_mtx.reset();
         shape_mtx *= agg::trans_affine_scaling(
               app.rbuf_img(0).width()/w,
               app.rbuf_img(0).height()/h
               );
         static unsigned char* pixels = NULL;
         if (pixels)
            free(pixels);
         pixels = (unsigned char*)malloc(w*h*4);
         rbuf.attach(pixels, w, h, -w*4);
         pixfmt_type pfb(rbuf);
         agg::renderer_base<pixfmt_type> backgrd(pfb);

         typedef agg::span_interpolator_linear<agg::trans_affine> interpolator_type;
         interpolator_type interpolator(shape_mtx);
         typedef agg::image_accessor_clone<pixfmt_type> img_accessor_type;
         pixfmt_type pixf_img(app.rbuf_img(0));
         img_accessor_type ia(pixf_img);
         typedef agg::span_image_filter_rgba_nn<img_accessor_type, interpolator_type> span_gen_type;
         span_gen_type sg(ia, interpolator);
         agg::span_allocator<color_type> sa;
         ras.move_to_d(0,0);
         ras.line_to_d(w,0);
         ras.line_to_d(w,h);
         ras.line_to_d(0,h);

         agg::render_scanlines_aa(ras, sl, backgrd, sa, sg);
         resized = false;
      }
      rbase.copy_from(rbuf);
   }

   void forground(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase
         )
   {
      leaf1.draw(ras, sl, rbase);
      leaf2.draw(ras, sl, rbase);
   }
   void alarm(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase
         )
   {
      heart1.draw(ras, sl, rbase);
      heart2.draw(ras, sl, rbase);
   }

private:
   FlowerLeaf leaf1;
   FlowerLeaf leaf2;
   Heart heart1;
   Heart heart2;
};
