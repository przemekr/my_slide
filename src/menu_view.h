#include "ctrl/agg_cbox_ctrl.h"
#include "ctrl/agg_rbox_ctrl.h"
#include "ctrl/agg_slider_ctrl.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <pthread.h>
#include "SDL_thread.h"


enum Unit {
   HOURS = 0,
   DAYS,
   WEEKS,
   MONTHS
};

struct Settings
{
   int latest;
   Unit unit;
   int sound;
   Settings(): latest(0), sound(0) {}
   time_t unit_in_sec()
   {
      switch (unit)
      {
         case HOURS:
            return 60*60;
         case DAYS:
            return 24*60*60;
         case WEEKS:
            return 7*24*60*60;
         case MONTHS:
            return 30*24*60*60;
      }
   }
};

class MenuView : public AppView
{
public:
   MenuView(App& app): AppView(app),
   exitApp(30,  20,  130, 40,  "Quit App",  !flip_y),
   start  (30,  50,  130, 70,  "Start  ",   !flip_y),
   sound  (30,  80,            "Sound  ",   !flip_y),
   rando  (30,  110,           "Random ",   !flip_y),
   latest (200, 60,  370, 75,   !flip_y),
   unit   (380, 20,  500, 120,  !flip_y)
   {
      start.background_color(red);
      start.status(false);
      exitApp.background_color(red);
      sound.text_size(15);
      sound.text_color(red);
      sound.text_thickness(CTRL_TEXT_THICKNESS);
      sound.active_color(red);
      sound.inactive_color(red);
      rando.text_size(15);
      rando.text_color(red);
      rando.text_thickness(CTRL_TEXT_THICKNESS);
      rando.active_color(red);
      rando.inactive_color(red);

      latest.range(1, 8);
      latest.num_steps(8);
      latest.value(1);
      latest.label("\nLatest %1.0f");
      latest.background_color(transp);
      latest.text_color(agg::rgba(0.9,0.0,0,9));
      latest.text_thickness(CTRL_TEXT_THICKNESS);
      latest.border_width(0, 0);
      unit.background_color(transp);
      add_ctrl(start);
      add_ctrl(latest);
      add_ctrl(exitApp);
      add_ctrl(sound);
      add_ctrl(rando);
      add_ctrl(unit);

      unit.add_item("Hours");
      unit.add_item("Days");
      unit.add_item("Weeks");
      unit.add_item("Months");
      unit.cur_item(0);
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

      agg::rect_i rec(0, 0, w, h);
      pixfmt_type pf2(app.rbuf_img(0));
      rbase.blend_from(pf2, 
            &rec,
            0,
            0,
            128);

      agg::render_ctrl(ras, sl, rbase, start);
      agg::render_ctrl(ras, sl, rbase, exitApp);
      agg::render_ctrl(ras, sl, rbase, sound);
      agg::render_ctrl(ras, sl, rbase, rando);
      agg::render_ctrl(ras, sl, rbase, unit);
      agg::render_ctrl(ras, sl, rbase, latest);
   }

   void on_ctrl_change()
   {

#ifdef MOBILE
#define IDIRPATH "/storage/sdcard0/DCIM/Camera"
#define MDIRPATH "/storage/sdcard0/Music"
#else
#define IDIRPATH "."
#define MDIRPATH "."
#endif

      app.sound_on(sound.status());

      if (start.status())
      {
         DIR* Idir = opendir(IDIRPATH);
         DIR* Mdir = opendir(MDIRPATH);
         struct dirent *dirEntry;
         int img = 1;
         int music = 0;
         const int imgMax = 15;
         const int musicMax = 2;
         struct stat sb;
         time_t latest = time(NULL) - settings.latest*settings.unit_in_sec();
         time_t mtime;

         while ((dirEntry = readdir(Idir)) != NULL)
         {
            if (img < imgMax && strstr(dirEntry->d_name, ".jpg") != NULL)
            {
               memset(&sb, '\0', sizeof(struct stat));
               stat((std::string(IDIRPATH) + "/" + dirEntry->d_name).c_str(), &sb);
               mtime = sb.st_mtime;
               if (mtime > latest)
               {
                  images.push_back(std::pair<std::string, std::string>(dirEntry->d_name, ctime(&mtime)));
               }
            }
         }
         while ((dirEntry = readdir(Mdir)) != NULL)
         {
            if (music < musicMax && (strstr(dirEntry->d_name, ".ogg") != NULL))
            {
               musics.push_back(dirEntry->d_name);
            }
         }
         closedir(Idir);
         closedir(Mdir);

         if (rando.status())
         {
            std::random_shuffle(images.begin(), images.end());
            std::random_shuffle(musics.begin(), musics.end());
         }

         //take latest 2 musics.
         if (sound.status())
            for (auto it = musics.rbegin(); it != musics.rend() && music < 2; it++)
            {
               app.load_music(music++, (std::string(MDIRPATH) + "/" + *it).c_str());
            }
         if (music)
            app.play_music(0, 50);

         //lunch the image loader thread. Wait until it loads 3 photos
         SDL_Thread *thread;
         thread = SDL_CreateThread(image_loader, "ImageLoader", this);

         start.status(false);
         app.changeView("TextView");
      }

      if (exitApp.status())
      {
         throw 0;
      }

      settings.unit   = (Unit)unit.cur_item();
      settings.latest = latest.value();
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

   static int image_loader(void* menu)
   {
      SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);

      class MenuView* m = (MenuView*)menu;
      int img = 1;
      for (auto it = images.begin(); it != images.end() && maxPhotoIdx < m->app.max_images; it++)
      {
         m->app.load_img(img, (std::string(IDIRPATH) + "/" + it->first).c_str());
         double h = m->app.rbuf_img(img).height();
         double w = m->app.rbuf_img(img).width();
         DEBUG_PRINT("photo loaded! %d, %s\n", maxPhotoIdx, it->first.c_str());
         if (w>h)
         {
            m->app.rotate_img(img, agg::pi/2);
            DEBUG_PRINT("photo rotated! %d\n", maxPhotoIdx);
         }
         m->app.scale_img(img, m->app.rbuf_window().width(), m->app.rbuf_window().height());
         img++;
         maxPhotoIdx = img;
         DEBUG_PRINT("photo scaled! %d\n", maxPhotoIdx);
      }
      DEBUG_PRINT("all loaded\n");
      allPhotosLoaded = true;
      return 0;
   }

   const Settings& getSettings() { return settings; }

private:
   agg::button_ctrl<agg::rgba8> start;
   agg::button_ctrl<agg::rgba8> exitApp;
   agg::cbox_ctrl<agg::rgba8>   sound;
   agg::cbox_ctrl<agg::rgba8>   rando;
   agg::slider_ctrl<agg::rgba8> latest;

   agg::rbox_ctrl<agg::rgba8> unit;

   std::vector<std::string> musics;
   Settings settings;
};
