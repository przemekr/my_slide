#include "ctrl/agg_cbox_ctrl.h"
#include "ctrl/agg_rbox_ctrl.h"

struct Settings
{
   int theme;
   int sound;
   Settings(): theme(0), sound(0) {}
};

class MenuView : public View
{
public:
   MenuView(App& application): app(application),
   exitApp (30, 20, 130, 40,    "Quit App",  !flip_y),
   exitMenu(30, 50, 130, 70,    "Return  ",  !flip_y),
   sound   (150, 20,            "Sound  ",   !flip_y),

   alarmSound (150, 400, 250,  460,  !flip_y),
   theme      (150, 480, 250,  540,  !flip_y)
   {
      exitMenu.background_color(red);
      exitApp.background_color(red);
      sound.text_size(15);
      sound.text_color(red);
      sound.text_thickness(CTRL_TEXT_THICKNESS);
      sound.active_color(red);
      sound.inactive_color(red);
      alarmSound.background_color(transp);
      alarmSound.text_size(10);
      alarmSound.border_width(0);
      theme.background_color(transp);
      theme.text_size(10);
      theme.border_width(0);
      add_ctrl(exitMenu);
      add_ctrl(exitApp);
      add_ctrl(sound);
      add_ctrl(alarmSound);
      add_ctrl(theme);

      settingsf = app.open_rw_file("traffar.se", "agg_timer", "settings");
      fread(&settings, sizeof(Settings), 1, settingsf);

      alarmSound.add_item("....");
      alarmSound.add_item("Gong");
      alarmSound.add_item("Meloldy");
      alarmSound.cur_item(settings.sound);

      theme.add_item("plain");
      theme.add_item("flower");
      theme.cur_item(settings.theme);

      empty_decor    = new Decorator(app);
      romantic_decor = new RomanticDecorator(app);
      on_ctrl_change();
   }

   void on_draw()
   {
      double w = app.rbuf_window().width();
      double h = app.rbuf_window().height();

      pixfmt_type   pf(app.rbuf_window());
      pixfmt_pre    pixf_pre(app.rbuf_window());
      agg::renderer_base<pixfmt_type> rbase(pf);
      renderer_base_pre rb_pre(pixf_pre);
      agg::rasterizer_scanline_aa<> ras;
      agg::scanline_u8 sl;
      agg::span_allocator<color_type> sa;
      ras.reset();
      rbase.clear(lgray);

      agg::render_ctrl(ras, sl, rbase, exitMenu);
      agg::render_ctrl(ras, sl, rbase, exitApp);
      agg::render_ctrl(ras, sl, rbase, sound);
      agg::render_ctrl(ras, sl, rbase, alarmSound);
      agg::render_ctrl(ras, sl, rbase, theme);

      agg::rgba blue(0.0, 0, 0.9, 0.6);
      app.draw_text(30, 420, 20, blue, 1.0, "Sound");
      app.draw_text(30, 500, 20, blue, 1.0, "Theme");
   }

   void on_ctrl_change()
   {
      app.sound_on(sound.status());

      app.current_sound = alarmSound.cur_item();
      decorator = theme.cur_item() == 1? romantic_decor:
         theme.cur_item() == 0? empty_decor:
         empty_decor;

      if (exitMenu.status())
      {
         exitMenu.status(false);
         app.changeView("timer");
      }
      if (exitApp.status())
      {
         throw 0;
      }

      settings.sound = alarmSound.cur_item();
      settings.theme = theme.cur_item();
      rewind(settingsf);
      fwrite(&settings, sizeof(Settings), 1, settingsf);
      fflush(settingsf);
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
         return;
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

private:
    App& app;
    agg::button_ctrl<agg::rgba8> exitMenu;
    agg::button_ctrl<agg::rgba8> exitApp;
    agg::cbox_ctrl<agg::rgba8>   sound;

    agg::rbox_ctrl<agg::rgba8> theme;
    agg::rbox_ctrl<agg::rgba8> alarmSound;
    Decorator* empty_decor;
    Decorator* romantic_decor;
    FILE* settingsf;
    Settings settings;
};
