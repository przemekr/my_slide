//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef AGG_BUTTON_CTRL_INCLUDED
#define AGG_BUTTON_CTRL_INCLUDED

#include "agg_basics.h"
#include "agg_conv_stroke.h"
#include "agg_gsv_text.h"
#include "agg_trans_affine.h"
#include "agg_color_rgba.h"
#include "agg_rounded_rect.h"
#include "ctrl/agg_ctrl.h"



namespace agg
{

    //----------------------------------------------------------button_ctrl_impl
    class button_ctrl_impl : public ctrl
    {
    public:
        button_ctrl_impl(double x1, double y1, double x2, double y2,
              const char* label, bool flip_y=false);

        void text_thickness(double t)  { m_text_thickness = t; }
        void text_size(double h, double w=0.0);

        const char* label() { return m_label; }
        void label(const char* l);

        bool status() const { return m_status; }
        void status(bool st) { m_status = st; }

        virtual bool in_rect(double x, double y) const;
        virtual bool on_mouse_button_down(double x, double y);
        virtual bool on_mouse_button_up(double x, double y);
        virtual bool on_mouse_move(double x, double y, bool button_flag);
        virtual bool on_arrow_keys(bool left, bool right, bool down, bool up);

        // Vertex source interface
        unsigned num_paths() { return 4; };
        void     rewind(unsigned path_id);
        unsigned vertex(double* x, double* y);

    private:
        double   m_text_thickness;
        double   m_text_height;
        double   m_text_width;
        char     m_label[128];
        bool     m_status;
        bool     m_clicked;
        double   m_vx[32];
        double   m_vy[32];
        rounded_rect shadow;
        rounded_rect outline;
        rounded_rect background;

        gsv_text              m_text;
        conv_stroke<gsv_text> m_text_poly;

        unsigned m_idx;
        unsigned m_vertex;
    };


    //----------------------------------------------------------button_ctrl_impl
    template<class ColorT> class button_ctrl : public button_ctrl_impl
    {
    public:
        button_ctrl(double x1, double y1, double x2, double y2,
              const char* label, bool flip_y=false) :
            button_ctrl_impl(x1, y1, x2, y2, label, flip_y),
            m_text_color(rgba(0.0, 0.0, 0.0)),
            m_shadow_color(rgba(0.0, 0.0, 0.0, 0.15)),
            m_background_color(rgba(0.0, 0.0, 0.4)),
            m_outline_color(rgba(0.0, 0.0, 0.0))
        {
            m_colors[0] = &m_shadow_color;
            m_colors[1] = &m_outline_color;
            m_colors[2] = &m_background_color;
            m_colors[3] = &m_text_color;
        }
          
        void text_color(const ColorT& c) { m_text_color = c; }
        void background_color(const ColorT& c) { m_background_color = c; }
        void outline_color(const ColorT& c) { m_outline_color = c; }

        const ColorT& color(unsigned i) const { return *m_colors[i]; } 

    private:
        button_ctrl(const button_ctrl<ColorT>&);
        const button_ctrl<ColorT>& operator = (const button_ctrl<ColorT>&);

        ColorT m_text_color;
        ColorT m_outline_color;
        ColorT m_background_color;
        ColorT m_shadow_color;
        ColorT* m_colors[4];
    };
}

#endif
