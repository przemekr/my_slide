/*
 * AGG-Timer, a simple timer clock application.
 * Copyright 2014 Przemyslaw Rzepecki
 * Contact: przemekr@sdfeu.org
 * 
 * This file is part of AGG-Timer.
 * 
 * AGG-Timer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * AGG-Timer is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * AGG-Timer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <algorithm> 
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


int agg_main(int argc, char* argv[])
{
    the_application app(agg::pix_format_bgra32, flip_y);
    app.caption("AGG Timer");

    DIR* dir = opendir(".");
    struct dirent *dirEntry;
    int img = 0;
    int music = 0;
    while ((dirEntry = readdir(dir)) != NULL)
    {
       if (strstr(dirEntry->d_name, ".jpg") != NULL)
       {
          app.load_img(img++, dirEntry->d_name);
       }
       if (strstr(dirEntry->d_name, ".ogg") != NULL)
       {
          app.load_music(music++, dirEntry->d_name);
       }
    }
    closedir(dir);
    std::cout << "Loaded: " << img << " images" << std::endl;
    std::cout << "Loaded: " << music << " music" << std::endl;
    maxPhotoIdx = img-1;

    if (app.init(START_W, START_H, WINDOW_FLAGS))
    {
       try {
          app.play_music(1, 50);
          return app.run();
       } catch (...) {
          return 0;
       }
    }
    return 1;
}
