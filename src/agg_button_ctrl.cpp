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

#include <string.h>
#include "agg_button_ctrl.h"
#include "agg_rounded_rect.h"


namespace agg
{

    //------------------------------------------------------------------------
    button_ctrl_impl::button_ctrl_impl(double x1, double y1, double x2, double y2,
                                   const char* l, bool flip_y) :
        ctrl(x1, y1, x2, y2, flip_y),
        m_text_thickness(1.5),
        m_text_height(0.4*(y2-y1)),
        m_text_width(0.8*(x2-x1)/strlen(l)),
        m_status(false),
        m_clicked(false),
        outline(x1, y1, x2, y2, 5.0),
        shadow(x1+2*m_text_thickness, y1-m_text_thickness, x2+2*m_text_thickness, y2-m_text_thickness, 5.0),
        background(x1+m_text_thickness, y1+m_text_thickness, x2-m_text_thickness, y2-m_text_thickness, 5.0),
        m_text_poly(m_text)
    {
        label(l);
    }


    //------------------------------------------------------------------------
    void button_ctrl_impl::text_size(double h, double w)
    {
        m_text_width = w; 
        m_text_height = h; 
    }

    //------------------------------------------------------------------------
    void button_ctrl_impl::label(const char* l)
    {
        unsigned len = strlen(l);
        if(len > 127) len = 127;
        memcpy(m_label, l, len);
        m_label[len] = 0;
    }


    //------------------------------------------------------------------------
    bool button_ctrl_impl::on_mouse_button_down(double x, double y)
    {
        inverse_transform_xy(&x, &y);
        if(x >= m_x1 && y >= m_y1 && x <= m_x2 && y <= m_y2)
        {
            m_clicked = true;
            return true;
        }
        return false;
    }


    //------------------------------------------------------------------------
    bool button_ctrl_impl::on_mouse_move(double, double, bool)
    {
        return false;
    }

    //------------------------------------------------------------------------
    bool button_ctrl_impl::in_rect(double x, double y) const
    {
        inverse_transform_xy(&x, &y);
        return x >= m_x1 && y >= m_y1 && x <= m_x2 && y <= m_y2;
    }

    //------------------------------------------------------------------------
    bool button_ctrl_impl::on_mouse_button_up(double x, double y)
    {
        inverse_transform_xy(&x, &y);
        m_clicked = false;
        if(x >= m_x1 && y >= m_y1 && x <= m_x2 && y <= m_y2)
        {
            m_status = !m_status;
            return true;
        }
        return false;
    }

    //------------------------------------------------------------------------
    bool button_ctrl_impl::on_arrow_keys(bool, bool, bool, bool)
    {
        return false;
    }


    //------------------------------------------------------------------------
    void button_ctrl_impl::rewind(unsigned idx)
    {
        m_idx = idx;

        double d2;
        double t;

        switch(idx)
        {
        default:
        case 0:                 // Background
            m_vertex = 0;
            shadow.rewind(0);
            break;

        case 1:                 // Background
            m_vertex = 0;
            outline.rewind(0);
            break;

        case 2:                 // Border
            m_vertex = 0;
            background.rewind(0);
            break;

        case 3:                 // Text
            m_text.text(m_label);
            m_text.start_point(m_x1 + 2*m_text_thickness, (m_y2+m_y1-m_text_height)/2);
            m_text.size(m_text_height, m_text_width);
            m_text_poly.width(m_text_thickness);
            m_text_poly.line_join(round_join);
            m_text_poly.line_cap(round_cap);
            m_text_poly.rewind(0);
            break;
        }
    }




    //------------------------------------------------------------------------
    unsigned button_ctrl_impl::vertex(double* x, double* y)
    {
        unsigned cmd = path_cmd_line_to;
        switch(m_idx)
        {
        case 0:
            if (m_clicked)
            {
               cmd = path_cmd_stop;
               break;
            }
            cmd = shadow.vertex(x, y);
            break;

        case 1:
            cmd = outline.vertex(x, y);
            break;
            break;

        case 2:
            cmd = background.vertex(x, y);
            break;

        case 3:
            cmd = m_text_poly.vertex(x, y);
            break;

        default:
            cmd = path_cmd_stop;
            break;
        }

        if(!is_stop(cmd))
        {
            transform_xy(x, y);
        }
        return cmd;
    }
}
