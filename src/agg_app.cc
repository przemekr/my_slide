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

int maxPhotoIdx = 0;

#include "app_support.h"
#include "photo_view.h"
#include "blured_text.h"
#include "menu_view.h"


class the_application: public App
{
public:
   the_application(agg::pix_format_e format, bool flip_y) :
      App(format, flip_y),
      photoView(*this),
      textView(*this),
      menuView(*this)
   {
      load_img(0, "menu_background.png");
      changeView("MenuView");
   }
   virtual void changeView(const char* name) 
   {
      if (strcmp(name, "PhotoView") == 0)
         view = &photoView;
      if (strcmp(name, "TextView") == 0)
         view = &textView;
      if (strcmp(name, "MenuView") == 0)
         view = &menuView;
      view->enter();
   };
private:
   PhotoView photoView;
   TextView textView;
   MenuView menuView;
};


int agg_main(int argc, char* argv[])
{
   DEBUG_PRINT("AGG MAIN\n");

   the_application app(agg::pix_format_bgra32, flip_y);
   std::srand(time(NULL));
   app.caption("MySlide");


   if (app.init(START_W, START_H, WINDOW_FLAGS))
   {
      try {
         return app.run();
      } catch (...) {
         return 0;
      }
   }
   return 1;
}
