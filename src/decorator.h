class Decorator
{
public:
   Decorator(App& application):
      app(application) {}
   virtual void update(long elapsed_time) {}
   virtual void background(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase,
         bool resized
         ) {}
   virtual void forground(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase
         ) {}
   virtual void alarm(
         agg::rasterizer_scanline_aa<>& ras,
         agg::scanline_u8& sl,
         renderer_base_type& rbase
         ) {}
protected:
   App& app;
};
Decorator* decorator = NULL;
