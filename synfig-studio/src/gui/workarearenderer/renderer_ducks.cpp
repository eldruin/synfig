/* === S Y N F I G ========================================================= */
/*!	\file renderer_ducks.cpp
**	\brief Template File
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007, 2008 Chris Moore
**  Copyright (c) 2008 Gerald Young
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "renderer_ducks.h"
#include "workarea.h"
#include "duckmatic.h"
#include <ETL/bezier>
#include <ETL/misc>
#include "widgets/widget_color.h"
#include <synfig/distance.h>
#include "app.h"

#include "general.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;
using namespace studio;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Renderer_Ducks::~Renderer_Ducks()
{
}

/*
bool
Renderer_Ducks::get_enabled_vfunc()const
{
	return get_work_area()->grid_status();
}
*/

struct ScreenDuck
{
	synfig::Point pos;
	Gdk::Color color;
	bool selected;
	bool hover;
	Real width;

	ScreenDuck():width(0) { }
};

void
Renderer_Ducks::render_vfunc(
	const Glib::RefPtr<Gdk::Drawable>& drawable,
	const Gdk::Rectangle& /*expose_area*/
)
{
	assert(get_work_area());
	if(!get_work_area())
		return;

	const synfig::Point window_start(get_work_area()->get_window_tl());
	const float pw(get_pw()),ph(get_ph());

	const bool solid_lines(get_work_area()->solid_lines);

	const std::list<etl::handle<Duckmatic::Bezier> >& bezier_list(get_work_area()->bezier_list());
	const std::list<handle<Duckmatic::Stroke> >& stroke_list(get_work_area()->stroke_list());
	Glib::RefPtr<Pango::Layout> layout(Pango::Layout::create(get_work_area()->get_pango_context()));


	Glib::RefPtr<Gdk::GC> gc(Gdk::GC::create(drawable));
	Cairo::RefPtr<Cairo::Context> cr = drawable->create_cairo_context();

	cr->save();
	cr->set_line_cap(Cairo::LINE_CAP_BUTT);
	cr->set_line_join(Cairo::LINE_JOIN_MITER);

	// Render the strokes
	for(std::list<handle<Duckmatic::Stroke> >::const_iterator iter=stroke_list.begin();iter!=stroke_list.end();++iter)
	{
		cr->save();

		std::list<synfig::Point>::iterator iter2;
		for(iter2=(*iter)->stroke_data->begin();iter2!=(*iter)->stroke_data->end();++iter2)
		{
			cr->line_to(
				((*iter2)[0]-window_start[0])/pw,
				((*iter2)[1]-window_start[1])/ph
				);
		}

		cr->set_line_width(1.0);
		cr->set_source_rgb(
			colorconv_synfig2gdk((*iter)->color).get_red_p(),
			colorconv_synfig2gdk((*iter)->color).get_green_p(),
			colorconv_synfig2gdk((*iter)->color).get_blue_p()
			);
		cr->stroke();

		cr->restore();
	}



	// Render the beziers
	for(std::list<handle<Duckmatic::Bezier> >::const_iterator iter=bezier_list.begin();iter!=bezier_list.end();++iter)
	{
		Point p1((*iter)->p1->get_trans_point()-window_start);
		Point p2((*iter)->p2->get_trans_point()-window_start);
		Point c1((*iter)->c1->get_trans_point()-window_start);
		Point c2((*iter)->c2->get_trans_point()-window_start);
		p1[0]/=pw;p1[1]/=ph;
		p2[0]/=pw;p2[1]/=ph;
		c1[0]/=pw;c1[1]/=ph;
		c2[0]/=pw;c2[1]/=ph;

		cr->save();

		cr->move_to(p1[0], p1[1]);
		cr->curve_to(c1[0], c1[1], c2[0], c2[1], p2[0], p2[1]);

/*
		if (solid_lines)
		{
			cr->set_source_rgb(0,0,0); // DUCK_COLOR_BEZIER_1
			cr->set_line_width(3.0);
			cr->stroke_preserve();

			cr->set_source_rgb(175.0/255.0,175.0/255.0,175.0/255.0); //DUCK_COLOR_BEZIER_2
			cr->set_line_width(1.0);
			cr->stroke();
		}
		else
*/
		{
			//Solid line background
			cr->set_line_width(1.0);
			cr->set_source_rgb(0,0,0); // DUCK_COLOR_BEZIER_1
			cr->stroke_preserve();

			//Dashes
			cr->set_source_rgb(175.0/255.0,175.0/255.0,175.0/255.0); //DUCK_COLOR_BEZIER_2
			std::valarray<double> dashes(2);
			dashes[0]=5.0;
			dashes[1]=5.0;
			cr->set_dash(dashes, 0);
			cr->stroke();
		}
		cr->restore();
	}


	const DuckList duck_list(get_work_area()->get_duck_list());

	std::list<ScreenDuck> screen_duck_list;
	const float radius((abs(pw)+abs(ph))*4);

	etl::handle<Duck> hover_duck(get_work_area()->find_duck(get_work_area()->get_cursor_pos(),radius, get_work_area()->get_type_mask()));

	// Render the ducks
	for(std::list<handle<Duck> >::const_iterator iter=duck_list.begin();iter!=duck_list.end();++iter)
	{

		// If this type of duck has been masked, then skip it
		if((*iter)->get_type() && (!(get_work_area()->get_type_mask() & (*iter)->get_type())))
			continue;

		Point sub_trans_point((*iter)->get_sub_trans_point());
		Point sub_trans_origin((*iter)->get_sub_trans_origin());

		if (App::restrict_radius_ducks &&
			(*iter)->is_radius())
		{
			if (sub_trans_point[0] < sub_trans_origin[0])
				sub_trans_point[0] = sub_trans_origin[0];
			if (sub_trans_point[1] < sub_trans_origin[1])
				sub_trans_point[1] = sub_trans_origin[1];
		}

		Point point((*iter)->get_transform_stack().perform(sub_trans_point));
		Point origin((*iter)->get_transform_stack().perform(sub_trans_origin));

		point[0]=(point[0]-window_start[0])/pw;
		point[1]=(point[1]-window_start[1])/ph;

		bool has_connect(false);
		if((*iter)->get_tangent() || (*iter)->get_type()&Duck::TYPE_ANGLE)
		{
			has_connect=true;
		}
		if((*iter)->get_connect_duck())
		{
			has_connect=true;
			origin=(*iter)->get_connect_duck()->get_trans_point();
		}

		origin[0]=(origin[0]-window_start[0])/pw;
		origin[1]=(origin[1]-window_start[1])/ph;

		bool selected(get_work_area()->duck_is_selected(*iter));
		bool hover(*iter==hover_duck || (*iter)->get_hover());

		if(get_work_area()->get_selected_value_node())
		{
			synfigapp::ValueDesc value_desc((*iter)->get_value_desc());
			if (value_desc.is_valid() &&
				((value_desc.is_value_node()		&& get_work_area()->get_selected_value_node() == value_desc.get_value_node()) ||
				 (value_desc.parent_is_value_node()	&& get_work_area()->get_selected_value_node() == value_desc.get_parent_value_node())))
			{
				cr->save();

				cr->rectangle(
					round_to_int(point[0]-5),
					round_to_int(point[1]-5),
					10,
					10
					);

				cr->set_line_width(2.0);
				cr->set_source_rgb(1, 0, 0); //DUCK_COLOR_SELECTED
				cr->stroke();

				cr->restore();
			}

		}

		if((*iter)->get_box_duck())
		{
			Point boxpoint((*iter)->get_box_duck()->get_trans_point());
			boxpoint[0]=(boxpoint[0]-window_start[0])/pw;
			boxpoint[1]=(boxpoint[1]-window_start[1])/ph;
			Point tl(min(point[0],boxpoint[0]),min(point[1],boxpoint[1]));

			cr->save();

			cr->rectangle(
				round_to_int(tl[0]),
				round_to_int(tl[1]),
				round_to_int(abs(boxpoint[0]-point[0])),
				round_to_int(abs(boxpoint[1]-point[1]))
				);

			// Solid white box
			cr->set_line_width(1.0);
			cr->set_source_rgb(1,1,1); //DUCK_COLOR_BOX_1
			cr->stroke_preserve();

			// Dashes
			cr->set_source_rgb(0,0,0); //DUCK_COLOR_BOX_2
			std::valarray<double> dashes(2);
			dashes[0]=5.0;
			dashes[1]=5.0;
			cr->set_dash(dashes, 0);
			cr->stroke();

			cr->restore();
		}

		ScreenDuck screen_duck;
		screen_duck.pos=point;
		screen_duck.selected=selected;
		screen_duck.hover=hover;

		if(!(*iter)->get_editable())
			screen_duck.color=(DUCK_COLOR_NOT_EDITABLE);
		else if((*iter)->get_tangent())
			if(0){
				// Tangents have different color depending on the split state (disabled for now)
				//
				// Check if we can reach the canvas and set the time to
				// evaluate the split value accordingly
				synfig::Canvas::Handle canvas_h(get_work_area()->get_canvas());
				synfig::Time time(canvas_h?canvas_h->get_time():synfig::Time(0));
				// Retrieve the split value of the bline point.
				synfigapp::ValueDesc& v_d((*iter)->get_value_desc());
				synfig::LinkableValueNode::Handle parent;
				if(v_d.parent_is_linkable_value_node())
				{
					parent=v_d.get_parent_value_node();
					bool split;
					synfig::ValueNode::Handle child(parent->get_link("split"));
					if(synfig::ValueNode_Animated::Handle::cast_dynamic(child))
					{
						synfig::ValueNode_Animated::Handle animated_child(synfig::ValueNode_Animated::Handle::cast_dynamic(child));
						split=animated_child->new_waypoint_at_time(time).get_value(time).get(split);
					}
					else if(synfig::ValueNode_Const::Handle::cast_dynamic(child))
					{
						synfig::ValueNode_Const::Handle const_child(synfig::ValueNode_Const::Handle::cast_dynamic(child));
						split=(const_child->get_value()).get(split);
					}
					screen_duck.color=(split? DUCK_COLOR_TANGENT_2 : DUCK_COLOR_TANGENT_1);
				}
				else
					screen_duck.color=DUCK_COLOR_TANGENT_1;
			} else {
				// All tangents are the same color
				screen_duck.color=((*iter)->get_scalar()<0 ? DUCK_COLOR_TANGENT_1 : DUCK_COLOR_TANGENT_1);
			}
		else if((*iter)->get_type()&Duck::TYPE_VERTEX)
			screen_duck.color=DUCK_COLOR_VERTEX;
		else if((*iter)->get_type()&Duck::TYPE_RADIUS)
			screen_duck.color=DUCK_COLOR_RADIUS;
		else if((*iter)->get_type()&Duck::TYPE_WIDTH)
			screen_duck.color=DUCK_COLOR_WIDTH;
		else if((*iter)->get_type()&Duck::TYPE_ANGLE)
			screen_duck.color=(DUCK_COLOR_ANGLE);
		else
			screen_duck.color=DUCK_COLOR_OTHER;

		screen_duck_list.push_front(screen_duck);

		if(has_connect)
		{
			cr->save();

			cr->move_to(origin[0], origin[1]);
			cr->line_to(point[0], point[1]);

			if(solid_lines)
			{
				// Outside
				cr->set_line_width(3.0);
				cr->set_source_rgb(0,0,0); //DUCK_COLOR_CONNECT_OUTSIDE
				cr->stroke_preserve();

				// Inside
				cr->set_line_width(1.0);
				cr->set_source_rgb(159.0/255,239.0/255,239.0/255); //DUCK_COLOR_CONNECT_INSIDE
				cr->stroke();
			}
			else
			{
				// White background
				cr->set_line_width(1.0);
				cr->set_source_rgb(0,0,0); //DUCK_COLOR_CONNECT_OUTSIDE
				cr->stroke_preserve();

				// Dashes on top of the background
				cr->set_source_rgb(159.0/255,239.0/255,239.0/255); //DUCK_COLOR_CONNECT_INSIDE
				std::valarray<double> dashes(2);
				dashes[0]=5.0;
				dashes[1]=5.0;
				cr->set_dash(dashes, 0);
				cr->stroke();
			}

			cr->restore();
		}

		if((*iter)->is_radius())
		{
			const Real mag((point-origin).mag());

			cr->save();

			cr->arc(
				origin[0],
				origin[1],
				mag,
				0,
				M_PI*2
				);

			if(solid_lines)
			{
				cr->set_line_width(3.0);
				cr->set_source_rgb(0,0,0);
				cr->stroke_preserve();

				cr->set_source_rgb(175.0/255.0,175.0/255.0,175.0/255.0);
			}
			else
			{
				cr->set_source_rgb(1.0,1.0,1.0);
				// OPERATOR_DIFFERENCE does not currently have a C++ wrapper
				//cr->set_operator(Cairo::OPERATOR_DIFFERENCE);
				cairo_set_operator(cr->cobj(), CAIRO_OPERATOR_DIFFERENCE);
			}

			cr->set_line_width(1.0);
			cr->stroke();

			cr->restore();

			if(hover)
			{
				Real mag;
				if (App::restrict_radius_ducks)
				{
					Point sub_trans_point((*iter)->get_sub_trans_point());
					Point sub_trans_origin((*iter)->get_sub_trans_origin());

					if (sub_trans_point[0] < sub_trans_origin[0])
						sub_trans_point[0] = sub_trans_origin[0];
					if (sub_trans_point[1] < sub_trans_origin[1])
						sub_trans_point[1] = sub_trans_origin[1];

					Point point((*iter)->get_transform_stack().perform(sub_trans_point));
					Point origin((*iter)->get_transform_stack().perform(sub_trans_origin));

					mag = (point-origin).mag();
				}
				else
					mag = ((*iter)->get_trans_point()-(*iter)->get_trans_origin()).mag();

				Distance real_mag(mag, Distance::SYSTEM_UNITS);
				real_mag.convert(App::distance_system,get_work_area()->get_rend_desc());
				layout->set_text(real_mag.get_string());

				gc->set_rgb_fg_color(DUCK_COLOR_WIDTH_TEXT_1);
				drawable->draw_layout(
					gc,
					round_to_int(point[0])+1+6,
					round_to_int(point[1])+1-8,
					layout
				);
				gc->set_rgb_fg_color(DUCK_COLOR_WIDTH_TEXT_2);
				drawable->draw_layout(
					gc,
					round_to_int(point[0])+6,
					round_to_int(point[1])-8,
					layout
				);
			}

		}

	}


	for(;screen_duck_list.size();screen_duck_list.pop_front())
	{
		Gdk::Color color(screen_duck_list.front().color);
		double radius = 4;
		double outline = 1;

		// Draw the hovered duck last (on top of everything)
		if(screen_duck_list.front().hover && !screen_duck_list.back().hover && screen_duck_list.size()>1)
		{
			screen_duck_list.push_back(screen_duck_list.front());
			continue;
		}

		cr->save();

		if(!screen_duck_list.front().selected)
		{
			color.set_red(color.get_red()*2/3);
			color.set_green(color.get_green()*2/3);
			color.set_blue(color.get_blue()*2/3);
		}

		if(screen_duck_list.front().hover)
		{
			radius += 1;
			outline += 1;
		}

		cr->arc(
			screen_duck_list.front().pos[0],
			screen_duck_list.front().pos[1],
			radius,
			0,
			M_PI*2
			);

		cr->set_source_rgb(
			color.get_red_p(),
			color.get_green_p(),
			color.get_blue_p()
			);
		cr->fill_preserve();

		cr->set_line_width(outline);
		cr->set_source_rgb(0,0,0); //DUCK_COLOR_OUTLINE
		cr->stroke();

		cr->restore();
	}
}