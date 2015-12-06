/*
 * MySlide, a simple slide show application.
 * Copyright 2014 Przemyslaw Rzepecki
 * Contact: przemekr@sdfeu.org
 * 
 * This file is part of MySlide.
 * 
 * MySlide is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * MySlide is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * MySlide.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <algorithm> 
#include <vector> 
#include <stack>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <iostream>

#include "app_support.h"
#include "photo_view.h"
#include "blured_text.h"

int maxPhotoIdx = 0;

class the_application: public App
{
public:
   the_application(agg::pix_format_e format, bool flip_y) :
      App(format, flip_y)
   {
      photoView = new PhotoView(*this);
      textView = new TextView(*this);
      changeView("TextView");
   }
   virtual void changeView(const char* name) 
   {
      if (strcmp(name, "PhotoView") == 0)
         view = photoView;
      if (strcmp(name, "TextView") == 0)
         view = textView;
      view->enter();
   };
private:
   PhotoView* photoView;
   TextView* textView;
};

#ifdef MOBILE
#define IDIRPATH "/storage/sdcard0/DCIM/Camera"
#define MDIRPATH "/storage/sdcard0/Music"
#else
#define IDIRPATH "."
#define MDIRPATH "."
#endif

int agg_main(int argc, char* argv[])
{
   DEBUG_PRINT("AGG MAIN\n");

   the_application app(agg::pix_format_bgra32, flip_y);
   std::srand(time(NULL));
   app.caption("MySlide");

   DEBUG_PRINT("opendir");
   DIR* Idir = opendir(IDIRPATH);
   DIR* Mdir = opendir(MDIRPATH);
   struct dirent *dirEntry;
   int img = 0;
   int music = 0;
   const int imgMax = 15;
   const int musicMax = 2;
   std::vector<std::string> images;
   std::vector<std::string> musics;

   while ((dirEntry = readdir(Idir)) != NULL)
   {
      if (img < imgMax && strstr(dirEntry->d_name, ".jpg") != NULL)
      {
         images.push_back(dirEntry->d_name);
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
   DEBUG_PRINT("found %d img, %d music\n", images.size(), musics.size());
   std::random_shuffle (images.begin(), images.end());
   std::random_shuffle (musics.begin(), musics.end());

   //take latest 15 images.
   for (auto it = images.rbegin(); it != images.rend() && img < 15; it++)
   {
      app.load_img(img, (std::string(IDIRPATH) + "/" + *it).c_str());
      double h = app.rbuf_img(img).height();
      double w = app.rbuf_img(img).width();
      app.scale_img(img++, w*1024/h, 1024);
   }

   //take latest 2 musics.
   for (auto it = musics.rbegin(); it != musics.rend() && music < 2; it++)
   {
      app.load_music(music++, (std::string(MDIRPATH) + "/" + *it).c_str());
   }
   maxPhotoIdx = img-1;

   if (app.init(START_W, START_H, WINDOW_FLAGS))
   {
      try {
         if (music)
            app.play_music(0, 50);
         return app.run();
      } catch (...) {
         return 0;
      }
   }
   return 1;
}
