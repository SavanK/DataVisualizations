#include "Scatterplot.h"

Scatterplot::Scatterplot()
{
	mouse_mode = Scatterplot::MOUSE_MODE_INVALID;
	which_slider = Scatterplot::SLIDER_NONE;
}

Scatterplot::~Scatterplot()
{
}

void Scatterplot::setup(float x_pos, float y_pos, float width, float height, int num_steps)
{
	x_offset = y_offset = 0.1 * height;

	this->width = width;
	this->height = height;
	this->x_pos = x_pos;
	this->y_pos = y_pos;
	this->sp_width = this->sp_height = height - 2*y_offset;
	this->sp_x_pos = x_pos + width - height;
	this->sp_y_pos = y_pos;
	this->num_steps = num_steps;

	col_sel_width = 0.8f * (width - height);
	col_sel_height = 6.0f;
	col_sel_x = 0.1 * (width - height);

	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);
	//Loading verdana font
	verdana_label.load("verdana.ttf", LABEL_FONT_SIZE, true, true);
	verdana_label.setLineHeight(20.0f);
	verdana_label.setLetterSpacing(1.037);

	verdana_sel_column_label.load("verdana.ttf", LABEL_SEL_COLUMN_FONT_SIZE, true, true);
	verdana_sel_column_label.setLineHeight(20.0f);
	verdana_sel_column_label.setLetterSpacing(1.037);

	verdana_tick.load("verdana.ttf", TICK_FONT_SIZE, true, true);
	verdana_tick.setLineHeight(20.0f);
	verdana_tick.setLetterSpacing(1.037);

	//Setting up colors
	canvas_color.set(255, 255, 255);
	hovered_dot_color.set(224, 31, 212);
	dot_color.set(0, 195, 255, 140);
	label_color.set(60, 60, 60);
	tick_color.set(60, 60, 60);
	axis_color.set(90, 90, 90);
	grid_lines_color.set(90, 90, 90, 100);
	tooltip_bg_color.set(90, 90, 90, 200);
	tooltip_label_color.set(255, 255, 255);
	brush_dot_color.set(60, 60, 60, 140);
	brush_box_bg_color.set(0, 195, 255, 50);
	brush_box_border_color.set(0, 103, 135);
	slider_line_color.set(90, 90, 90);
	slider_dot_color.set(90, 90, 90);
	slider_selected_dot_color.set(224, 31, 212);
	slider_drag_sel_dot_color.set(0, 195, 255);
	slider_drag_shadow_dot_color.set(90, 90, 90, 100);

	//Register for mouse event callbacks
	ofRegisterMouseEvents(this);
}

void Scatterplot::set_columns(vector<Column*> c)
{
	columns = c;

	for (int i = 0; i < columns.size(); i++) {
		if (columns[i]->get_type() == Column::TYPE_FLOAT ||
			columns[i]->get_type() == Column::TYPE_INT) {
			available_columns.push_back(i);
		}
	}

	selected_x_column = selected_y_column = available_columns[0];

	calculate_range();
}

void Scatterplot::set_column_values(vector<vector<ColumnValue*>> c_values)
{
	column_values = c_values;
}

bool Scatterplot::is_inside_rect(float x, float y, 
	float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end) {
	float x_start, x_end, y_start, y_end;

	if (rec_x_start < rec_x_end && rec_y_start > rec_y_end) {
		x_start = rec_x_start;
		x_end = rec_x_end;
		y_start = rec_y_start;
		y_end = rec_y_end;
	}
	else if (rec_x_start > rec_x_end && rec_y_start > rec_y_end) {
		x_start = rec_x_end;
		x_end = rec_x_start;
		y_start = rec_y_start;
		y_end = rec_y_end;
	}
	else if (rec_x_start > rec_x_end && rec_y_start < rec_y_end) {
		x_start = rec_x_end;
		x_end = rec_x_start;
		y_start = rec_y_end;
		y_end = rec_y_start;
	}
	else {
		x_start = rec_x_start;
		x_end = rec_x_end;
		y_start = rec_y_end;
		y_end = rec_y_start;
	}

	return (x >= x_start && x <= x_end) && (y <= y_start && y >= y_end);
}

bool Scatterplot::is_inside_circle(float x, float y, float center_x, float center_y, float radius)
{
	float dist = sqrt((x - center_x)*(x - center_x) + (y - center_y)*(y - center_y));
	return dist <= radius;
}

void Scatterplot::get_values(int data_index, float * val_x, float * val_y)
{
	if (columns[selected_x_column]->get_type() == Column::TYPE_FLOAT) {
		column_values[selected_x_column][data_index]->get_value(val_x);
	}
	else {
		int val;
		column_values[selected_x_column][data_index]->get_value(&val);
		*val_x = val;
	}
	if (columns[selected_y_column]->get_type() == Column::TYPE_FLOAT) {
		column_values[selected_y_column][data_index]->get_value(val_y);
	}
	else {
		int val;
		column_values[selected_y_column][data_index]->get_value(&val);
		*val_y = val;
	}
}

void Scatterplot::set_brush_mask(vector<unsigned*> brush_mask)
{
	data_set_mask = brush_mask;
}

void Scatterplot::calculate_range()
{
	float min_x, max_x, min_y, max_y;

	if (columns[selected_x_column]->get_type() == Column::TYPE_FLOAT) {
		min_x = columns[selected_x_column]->get_min_f();
		max_x = columns[selected_x_column]->get_max_f();
	}
	else {
		min_x = columns[selected_x_column]->get_min_i();
		max_x = columns[selected_x_column]->get_max_i();
	}

	if (columns[selected_y_column]->get_type() == Column::TYPE_FLOAT) {
		min_y = columns[selected_y_column]->get_min_f();
		max_y = columns[selected_y_column]->get_max_f();
	}
	else {
		min_y = columns[selected_y_column]->get_min_i();
		max_y = columns[selected_y_column]->get_max_i();
	}

	x_range = make_pair(min_x, max_x);
	y_range = make_pair(min_y, max_y);
}

void Scatterplot::reset()
{
	reset_slider_mode();
	reset_brush_mode();
	available_columns.clear();
	selected_x_column = selected_y_column = -1;
	hovered_data_index = -1;
	x_range.first = x_range.second = 0;
	y_range.first = y_range.second = 0;
}

void Scatterplot::reset_brush_mode()
{
	mouse_mode = Scatterplot::MOUSE_MODE_INVALID;
	brush_start_x = 0;
	brush_start_y = 0;
	brush_end_x = 0;
	brush_end_y = 0;
	for (int i = 0; i < column_values[selected_x_column].size(); i++) {
		// when reset brush fully for both columns
		*data_set_mask[i] |= 1u << selected_x_column;
		*data_set_mask[i] |= 1u << selected_y_column;
	}
}

void Scatterplot::reset_slider_mode()
{
	mouse_mode = Scatterplot::MOUSE_MODE_INVALID;
	clicked_dot_index = -1;
	dragged_dot_index = -1;
	mouse_drag_x = 0;
	which_slider = Scatterplot::SLIDER_NONE;
}

void Scatterplot::draw()
{
	ofPushMatrix();
	ofTranslate(x_pos, y_pos);

	ofSetColor(canvas_color);
	ofDrawRectangle(0, 0, width, height);

	float off_y = height / 3.0f;
	ofSetColor(slider_line_color);
	ofDrawRectangle(col_sel_x, off_y, col_sel_width, col_sel_height);
	ofDrawRectangle(col_sel_x, off_y*2, col_sel_width, col_sel_height);

	ofSetColor(label_color);
	verdana_label.drawString("x-axis", col_sel_x, off_y - 10);
	verdana_label.drawString("y-axis", col_sel_x, off_y*2 - 10);

	float off_points_x = col_sel_width / (available_columns.size() - 1);
	for (int i = 0; i < available_columns.size(); i++) {
		float dot_radius;
		if (available_columns[i] == selected_x_column) {
			ofSetColor(slider_selected_dot_color);
			dot_radius = 1.5f*DOT_RADIUS;
			if (mouse_mode != Scatterplot::MOUSE_MODE_SLIDER_DRAG || 
				(mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_DRAG && which_slider != Scatterplot::SLIDER_X_AXIS)) {
				string str = columns[available_columns[i]]->get_name();
				float str_width = verdana_sel_column_label.stringWidth(str);
				float str_x = col_sel_x + (i*off_points_x) - (str_width / 2.0f);
				if (str_x < col_sel_x) {
					str_x = col_sel_x;
				}
				else if (str_x + str_width > col_sel_x + (available_columns.size() - 1)*off_points_x) {
					str_x = col_sel_x + (available_columns.size() - 1)*off_points_x - str_width;
				}
				verdana_sel_column_label.drawString(str, 
					str_x, 
					off_y + col_sel_height / 2.0f + 1.5f * LABEL_SEL_COLUMN_FONT_SIZE);
			}
		}
		else {
			ofSetColor(slider_dot_color);
			dot_radius = DOT_RADIUS;
		}
		ofDrawCircle(col_sel_x + i*off_points_x, off_y + col_sel_height / 2.0f, dot_radius);
	}
	for (int i = 0; i < available_columns.size(); i++) {
		float dot_radius;
		if (available_columns[i] == selected_y_column) {
			ofSetColor(slider_selected_dot_color);
			dot_radius = 1.5f*DOT_RADIUS;
			if (mouse_mode != Scatterplot::MOUSE_MODE_SLIDER_DRAG ||
				(mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_DRAG && which_slider != Scatterplot::SLIDER_Y_AXIS)) {
				string str = columns[available_columns[i]]->get_name();
				float str_width = verdana_sel_column_label.stringWidth(str);
				float str_x = col_sel_x + (i*off_points_x) - (str_width / 2.0f);
				if (str_x < col_sel_x) {
					str_x = col_sel_x;
				}
				else if (str_x + str_width > col_sel_x + (available_columns.size() - 1)*off_points_x) {
					str_x = col_sel_x + (available_columns.size() - 1)*off_points_x - str_width;
				}
				verdana_sel_column_label.drawString(str, 
					str_x, 
					off_y * 2 + col_sel_height / 2.0f + 1.5f * LABEL_SEL_COLUMN_FONT_SIZE);
			}
		}
		else {
			ofSetColor(slider_dot_color);
			dot_radius = DOT_RADIUS;
		}
		ofDrawCircle(col_sel_x + i*off_points_x, off_y * 2 + col_sel_height / 2.0f, dot_radius);
	}

	int dot_index = -1;
	float dot_y = (which_slider == Scatterplot::SLIDER_X_AXIS) ?
		(off_y + col_sel_height / 2.0f) : (off_y * 2 + col_sel_height / 2.0f);
	if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_CLICK) {
		dot_index = clicked_dot_index;
	}
	else if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_DRAG) {
		dot_index = dragged_dot_index;
		float dot_radius = DOT_RADIUS;
		dot_radius += Scatterplot::SLIDER_DOT_OFFSET;
		if (!is_inside_circle(mouse_drag_x, dot_y, col_sel_x + dot_index*off_points_x, dot_y, dot_radius)) {
			// draw shadow dot
			ofSetColor(slider_drag_shadow_dot_color);
			ofDrawCircle(mouse_drag_x, dot_y, 1.5f*DOT_RADIUS);
		}

		// draw column name
		string str = columns[available_columns[dot_index]]->get_name();
		float str_width = verdana_sel_column_label.stringWidth(str);
		float str_x = col_sel_x + (dot_index*off_points_x) - (str_width / 2.0f);
		if (str_x < col_sel_x) {
			str_x = col_sel_x;
		}
		else if (str_x + str_width > col_sel_x + (available_columns.size() - 1)*off_points_x) {
			str_x = col_sel_x + (available_columns.size() - 1)*off_points_x - str_width;
		}
		ofSetColor(slider_drag_sel_dot_color);
		verdana_sel_column_label.drawString(str,
			str_x,
			dot_y + 1.5f * LABEL_SEL_COLUMN_FONT_SIZE);
	}
	if (dot_index != -1) {
		ofSetColor(slider_drag_sel_dot_color);
		ofDrawCircle(col_sel_x + dot_index*off_points_x, dot_y, 1.5f*DOT_RADIUS);
	}

	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(sp_x_pos, sp_y_pos);

	ofSetColor(axis_color);
	ofPoint origin, horizontal_axis_end, vertical_axis_end;
	origin.x = x_offset;
	origin.y = sp_height + y_offset;
	horizontal_axis_end.x = x_offset + sp_width;
	horizontal_axis_end.y = origin.y;
	vertical_axis_end.x = origin.x;
	vertical_axis_end.y = y_offset;
	ofDrawArrow(origin, horizontal_axis_end, 6.0f);
	ofDrawArrow(origin, vertical_axis_end, 6.0f);

	ofSetColor(label_color);
	//verdana_label.drawString(x_label, origin.x + LABEL_HOR_X_OFFSET, origin.y + LABEL_HOR_Y_OFFSET);
	
	ofPushMatrix();
	ofRotate(-90.0f, 0.0f, 0.0f, 1.0f);
	//verdana_label.drawString(y_label, LABEL_VER_Y_OFFSET - origin.y, origin.x - LABEL_VER_X_OFFSET);
	ofPopMatrix();

	float step_size_x, step_size_y;
	step_size_x = sp_width / num_steps;
	step_size_y = sp_height / num_steps;

	int i;
	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_x*i, 0, sp_width, x_range.first, x_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 4);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + i*step_size_x - 20.0f, origin.y + TICK_HOR_Y_OFFSET);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x + i*step_size_x, origin.y, origin.x + i*step_size_x, origin.y - sp_height);
	}

	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_y*i, 0, sp_height, y_range.first, y_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 4);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + TICK_VER_X_OFFSET, origin.y - i*step_size_y + 5.0f);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x, origin.y - i*step_size_y, origin.x + sp_width, origin.y - i*step_size_y);
	}

	ofSetColor(mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_DRAG || mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_STABLE ? 
		brush_dot_color : dot_color);
	for (i = 0; i < column_values[selected_x_column].size(); i++) {
		ofPoint point;
		float val_x, val_y;
		get_values(i, &val_x, &val_y);
		point.x = ofMap(val_x, x_range.first, x_range.second, 0, sp_width) + x_offset;
		point.y = sp_height - ofMap(val_y, y_range.first, y_range.second, 0, sp_height) + y_offset;

		if (is_inside_rect(point.x, point.y, origin.x, origin.y, origin.x + sp_width, origin.y - sp_height)) {
			if (i == hovered_data_index) {
				// draw nothing here
			}
			else {
				if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_DRAG || mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_STABLE) {
					bool is_in_rect = false;
					if (is_inside_rect(point.x, point.y, brush_start_x, brush_start_y, brush_end_x, brush_end_y)) {
						is_in_rect = true;
					}

					if (is_in_rect) {
						ofSetColor(dot_color);
					}
					ofDrawCircle(point, DOT_RADIUS);
					if (is_in_rect) {
						ofSetColor(brush_dot_color);
					}
				}
				else {
					bool x_set = ((*data_set_mask[i] >> selected_x_column) & 1) == 1;
					bool y_set = ((*data_set_mask[i] >> selected_y_column) & 1) == 1;
					if (x_set && y_set) {
						ofSetColor(dot_color);
					}
					else {
						ofSetColor(brush_dot_color);
					}
					ofDrawCircle(point, DOT_RADIUS);
				}
			}
		}
	}

	if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_DRAG || mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_STABLE) {
		ofPath path = ofPath();
		path.setMode(ofPath::POLYLINES);
		path.setStrokeColor(brush_box_border_color);
		path.setStrokeWidth(3);
		path.setFilled(false);
		path.moveTo(brush_start_x, brush_start_y);
		path.lineTo(brush_start_x, brush_end_y);
		path.lineTo(brush_end_x, brush_end_y);
		path.lineTo(brush_end_x, brush_start_y);
		path.close();
		path.draw();

		ofSetColor(brush_box_bg_color);
		ofDrawRectangle(brush_start_x, brush_start_y, (brush_end_x - brush_start_x), (brush_end_y - brush_start_y));
	}
	else {
		if (hovered_data_index != -1) {
			ofSetColor(hovered_dot_color);
			ofPoint point;
			float val_x, val_y;
			get_values(hovered_data_index, &val_x, &val_y);
			point.x = ofMap(val_x, x_range.first, x_range.second, 0, sp_width) + x_offset;
			point.y = sp_height - ofMap(val_y, y_range.first, y_range.second, 0, sp_height) + y_offset;
			ofDrawCircle(point, HOVERED_DOT_RADIUS);

			point.x += (HOVERED_DOT_RADIUS + 2);
			point.y -= (HOVERED_DOT_RADIUS + 2);

			ofSetColor(tooltip_bg_color);
			int width = 140;
			int height = 40;
			ofDrawRectangle(point.x, point.y - height, width, height);

			ofSetColor(tooltip_label_color);
			string x_val_str = to_string(val_x);
			x_val_str = x_val_str.substr(0, 5);
			string y_val_str = to_string(val_y);
			y_val_str = y_val_str.substr(0, 5);
			string val_str;
			val_str.append(x_val_str);
			val_str.append(",");
			val_str.append(y_val_str);
			verdana_tick.drawString(val_str, point.x + 15, point.y - 15);
		}
	}

	ofPopMatrix();
}

int Scatterplot::get_mouse_hovered_data_index(float x, float y)
{
	int result = -1;
	float result_distance = DOT_RADIUS;
	for (int i = 0; i < column_values[selected_x_column].size(); i++) {
		ofPoint point;
		float val_x, val_y;
		get_values(i, &val_x, &val_y);
		point.x = ofMap(val_x, x_range.first, x_range.second, 0, sp_width) + x_offset;
		point.y = sp_height - ofMap(val_y, y_range.first, y_range.second, 0, sp_height) + y_offset;
		float distance = ofVec2f(point).distance(ofVec2f(x-sp_x_pos, y-sp_y_pos));
		if (distance < DOT_RADIUS && distance < result_distance) {
			result = i;
			result_distance = distance;
		}
	}
	return result;
}

void Scatterplot::mouseMoved(ofMouseEventArgs & args)
{
	hovered_data_index = get_mouse_hovered_data_index(args.x, args.y);
	ofNotifyEvent(hover_on, hovered_data_index, this);
}

void Scatterplot::mouseDragged(ofMouseEventArgs & args)
{
	if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_CLICK) {
		mouse_mode = Scatterplot::MOUSE_MODE_BRUSH_DRAG;
	}
	
	if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_DRAG) {
		ofPoint origin;
		origin.x = x_offset;
		origin.y = sp_height + y_offset;

		brush_end_x = args.x - sp_x_pos > origin.x + sp_width ? origin.x + sp_width :
			args.x - sp_x_pos < origin.x ? origin.x : args.x - sp_x_pos;
		brush_end_y = args.y - sp_y_pos < origin.y - sp_height ? origin.y - sp_height :
			args.y - sp_y_pos > origin.y ? origin.y : args.y - sp_y_pos;
		return;
	}

	if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_CLICK) {
		if ((which_slider == Scatterplot::SLIDER_X_AXIS && available_columns[clicked_dot_index] == selected_x_column) ||
			(which_slider == Scatterplot::SLIDER_Y_AXIS) && available_columns[clicked_dot_index] == selected_y_column) {
			mouse_mode = Scatterplot::MOUSE_MODE_SLIDER_DRAG;
			dragged_dot_index = clicked_dot_index;
		}
	} 
	
	if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_DRAG) {
		float off_y = height / 3.0f;
		float off_points_x = col_sel_width / (available_columns.size() - 1);
		float dot_y = (which_slider == Scatterplot::SLIDER_X_AXIS) ? 
			(off_y + col_sel_height / 2.0f) : (off_y*2 + col_sel_height / 2.0f);
		for (int i = 0; i < available_columns.size(); i++) {
			float dot_radius = DOT_RADIUS;
			dot_radius += Scatterplot::SLIDER_DOT_OFFSET;
			if (is_inside_circle(args.x - x_pos, dot_y, col_sel_x + i*off_points_x, dot_y, dot_radius)) {
				dragged_dot_index = i;
				break;
			}
		}
		mouse_drag_x = args.x - x_pos;
		if (mouse_drag_x < col_sel_x) {
			mouse_drag_x = col_sel_x;
		}
		else if (mouse_drag_x > col_sel_x + (available_columns.size() - 1) * off_points_x) {
			mouse_drag_x = col_sel_x + (available_columns.size() - 1) * off_points_x;
		}
		return;
	}
}

void Scatterplot::mousePressed(ofMouseEventArgs & args)
{
	if (args.button == 0 || args.button == 1) {
		ofPoint origin;
		origin.x = x_offset;
		origin.y = sp_height + y_offset;
		if (args.x - sp_x_pos >= origin.x && args.x - sp_x_pos <= origin.x + sp_width &&
			args.y - sp_y_pos <= origin.y && args.y - sp_y_pos >= origin.y - sp_height) {
			brush_start_x = args.x - sp_x_pos;
			brush_start_y = args.y - sp_y_pos;
			brush_end_x = brush_start_x;
			brush_end_y = brush_start_y;
			for (int i = 0; i < column_values[selected_x_column].size(); i++) {
				// To begin with brush fully considering if it was half brushed in PC
				*data_set_mask[i] |= 1u << selected_x_column;
				*data_set_mask[i] |= 1u << selected_y_column;
			}
			mouse_mode = Scatterplot::MOUSE_MODE_BRUSH_CLICK;
		}
		else {
			float off_y = height / 3.0f;
			float off_points_x = col_sel_width / (available_columns.size() - 1);
			for (int i = 0; i < available_columns.size(); i++) {
				float dot_radius;
				if (available_columns[i] == selected_x_column) {
					ofSetColor(hovered_dot_color);
					dot_radius = 1.5f*DOT_RADIUS;
				}
				else {
					ofSetColor(axis_color);
					dot_radius = DOT_RADIUS;
				}
				dot_radius += Scatterplot::SLIDER_DOT_OFFSET;
				if (is_inside_circle(args.x - x_pos, args.y - y_pos, col_sel_x + i*off_points_x, off_y + col_sel_height / 2.0f, dot_radius)) {
					if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_STABLE) {
						// reset brush mode
						reset_brush_mode();
					}
					mouse_mode = Scatterplot::MOUSE_MODE_SLIDER_CLICK;
					which_slider = Scatterplot::SLIDER_X_AXIS;
					clicked_dot_index = i;
					break;
				}
			}

			if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_CLICK) {
				return;
			}

			for (int i = 0; i < available_columns.size(); i++) {
				float dot_radius;
				if (available_columns[i] == selected_y_column) {
					ofSetColor(hovered_dot_color);
					dot_radius = 1.5f*DOT_RADIUS;
				}
				else {
					ofSetColor(axis_color);
					dot_radius = DOT_RADIUS;
				}
				dot_radius += Scatterplot::SLIDER_DOT_OFFSET;
				if (is_inside_circle(args.x - x_pos, args.y - y_pos, col_sel_x + i*off_points_x, off_y*2 + col_sel_height / 2.0f, dot_radius)) {
					if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_STABLE) {
						// reset brush mode
						reset_brush_mode();
					}
					mouse_mode = Scatterplot::MOUSE_MODE_SLIDER_CLICK;
					which_slider = Scatterplot::SLIDER_Y_AXIS;
					clicked_dot_index = i;
					break;
				}
			}
		}
	}
	else if (args.button == 2) {
		// reset brush state and all state variables
		reset_brush_mode();
	}
}

void Scatterplot::mouseReleased(ofMouseEventArgs & args)
{
	if (mouse_mode == Scatterplot::MOUSE_MODE_BRUSH_DRAG) {
		ofPoint origin;
		origin.x = x_offset;
		origin.y = sp_height + y_offset;

		brush_end_x = args.x - sp_x_pos > origin.x + sp_width ? origin.x + sp_width :
			args.x - sp_x_pos < origin.x ? origin.x : args.x - sp_x_pos;
		brush_end_y = args.y - sp_y_pos < origin.y - sp_height ? origin.y - sp_height :
			args.y - sp_y_pos > origin.y ? origin.y : args.y - sp_y_pos;

		float x_coord_min, x_coord_max, y_coord_min, y_coord_max;
		x_coord_min = brush_start_x < brush_end_x ? brush_start_x : brush_end_x;
		x_coord_max = brush_start_x > brush_end_x ? brush_start_x : brush_end_x;
		y_coord_min = brush_start_y < brush_end_y ? brush_start_y : brush_end_y;
		y_coord_max = brush_start_y > brush_end_y ? brush_start_y : brush_end_y;

		float x_min = ofMap(x_coord_min - x_offset, 0, sp_width, x_range.first, x_range.second);
		float x_max = ofMap(x_coord_max - x_offset, 0, sp_width, x_range.first, x_range.second);
		float y_max = ofMap(sp_height + y_offset - y_coord_min, 0, sp_height, y_range.first, y_range.second);
		float y_min = ofMap(sp_height + y_offset - y_coord_max, 0, sp_height, y_range.first, y_range.second);

		for (int i = 0; i < column_values[selected_x_column].size(); i++) {
			float val_x, val_y;
			get_values(i, &val_x, &val_y);
			if (val_x >= x_min && val_x <= x_max && val_y >= y_min && val_y <= y_max) {
				// brushed
				*data_set_mask[i] |= 1u << selected_x_column;
				*data_set_mask[i] |= 1u << selected_y_column;
			}
			else {
				// unbrushed
				*data_set_mask[i] &= ~(1u << selected_x_column);
				*data_set_mask[i] &= ~(1u << selected_y_column);
			}
		}

		mouse_mode = Scatterplot::MOUSE_MODE_BRUSH_STABLE;
	}
	else if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_CLICK) {
		if (which_slider == Scatterplot::SLIDER_X_AXIS && 
			available_columns[clicked_dot_index] != selected_x_column) {
			selected_x_column = available_columns[clicked_dot_index];
		}
		else if (which_slider == Scatterplot::SLIDER_Y_AXIS &&
			available_columns[clicked_dot_index] != selected_y_column) {
			selected_y_column = available_columns[clicked_dot_index];
		}

		calculate_range();

		reset_slider_mode();
	}
	else if (mouse_mode == Scatterplot::MOUSE_MODE_SLIDER_DRAG) {
		if (which_slider == Scatterplot::SLIDER_X_AXIS &&
			available_columns[dragged_dot_index] != selected_x_column) {
			selected_x_column = available_columns[dragged_dot_index];
		}
		else if (which_slider == Scatterplot::SLIDER_Y_AXIS &&
			available_columns[dragged_dot_index] != selected_y_column) {
			selected_y_column = available_columns[dragged_dot_index];
		}

		calculate_range();

		reset_slider_mode();
	}
}

void Scatterplot::mouseScrolled(ofMouseEventArgs & args)
{

}

void Scatterplot::mouseEntered(ofMouseEventArgs & args)
{

}

void Scatterplot::mouseExited(ofMouseEventArgs & args)
{

}