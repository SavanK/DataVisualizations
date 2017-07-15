#include "Scatterplot.h"

Scatterplot::Scatterplot()
{
}

Scatterplot::~Scatterplot()
{
}

void Scatterplot::setup(int width, int height, int x_pos, int y_pos, int num_steps)
{
	this->width = width;
	this->height = height;
	this->x_pos = x_pos;
	this->y_pos = y_pos;
	this->num_steps = num_steps;

	x_offset = 0.15f * width;
	y_offset = 0.15f * height;
	label_font_size = MAX(0.1f * width, 16);
	tick_font_size = MAX(0.05f * width, 10);
	label_x_offset = 1.5f * x_offset;
	label_y_offset = 1.5f * y_offset;
	tick_hor_y_offset = 0.06f * height;
	tick_ver_x_offset = -0.125f * width;

	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(MAX(0.25f*height,75));
	//Loading verdana font
	verdana_label.load("verdana.ttf", label_font_size, true, true);
	verdana_label.setLineHeight(MAX(0.1f*height, 16));
	verdana_label.setLetterSpacing(MAX(0.004f*height, 1.0));

	verdana_tick.load("verdana.ttf", tick_font_size, true, true);
	verdana_tick.setLineHeight(MAX(0.1f*height, 16));
	verdana_tick.setLetterSpacing(MAX(0.004f*height, 1.0));

	//Setting up colors
	canvas_color.set(255, 255, 255);
	hovered_dot_color.set(224, 31, 212);
	dot_color.set(31, 224, 43);
	label_color.set(60, 60, 60);
	tick_color.set(60, 60, 60);
	axis_color.set(90, 90, 90);
	grid_lines_color.set(90, 90, 90, 100);
	tooltip_bg_color.set(90, 90, 90, 200);
	tooltip_label_color.set(255, 255, 255);
	brush_dot_color.set(60, 60, 60);
	brush_box_bg_color.set(31, 224, 43, 50);
	brush_box_border_color.set(15, 105, 20);
	non_brush_dot_color.set(60, 60, 60, 60);

	//Register for mouse event callbacks
	ofRegisterMouseEvents(this);
}

void Scatterplot::set_data(string x_label, string y_label, vector<float> x_data, vector<float> y_data)
{
	this->x_label = x_label;
	this->y_label = y_label;
	this->x_data = x_data;
	this->y_data = y_data;

	int i;
	float min_x, max_x, min_y, max_y;

	min_x = max_x = this->x_data[0];
	min_y = max_y = this->y_data[0];
	for (i = 0; i < this->x_data.size(); i++) {
		if (this->x_data[i] > max_x) {
			max_x = this->x_data[i];
		} else if (this->x_data[i] < min_x) {
			min_x = this->x_data[i];
		}

		if (this->y_data[i] > max_y) {
			max_y = this->y_data[i];
		}
		else if (this->y_data[i] < min_y) {
			min_y = this->y_data[i];
		}
	}
	x_range = make_pair(min_x, max_x);
	y_range = make_pair(min_y, max_y);
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

void Scatterplot::set_selected_index(vector<int> selected_index)
{
	this->selected_index = selected_index;
}

void Scatterplot::set_brush_active(int index, bool active)
{
	brush_active = active;
	if (brush_active) {
		vector<int> sel_index;
		set_selected_index(sel_index);
	}

	if (index != this->index) {
		// reset brush state and all state variables
		brush_mode = false;
		brush_start_x = 0;
		brush_start_y = 0;
		brush_end_x = 0;
		brush_end_y = 0;
	}
}

void Scatterplot::set_slot_index(int index)
{
	this->index = index;
}

void Scatterplot::brush()
{
	float x_coord_min, x_coord_max, y_coord_min, y_coord_max;
	x_coord_min = brush_start_x < brush_end_x ? brush_start_x : brush_end_x;
	x_coord_max = brush_start_x > brush_end_x ? brush_start_x : brush_end_x;
	y_coord_min = brush_start_y < brush_end_y ? brush_start_y : brush_end_y;
	y_coord_max = brush_start_y > brush_end_y ? brush_start_y : brush_end_y;

	float x_min = ofMap(x_coord_min - x_offset, 0, width, x_range.first, x_range.second);
	float x_max = ofMap(x_coord_max - x_offset, 0, width, x_range.first, x_range.second);
	float y_max = ofMap(height + y_offset - y_coord_min, 0, width, y_range.first, y_range.second);
	float y_min = ofMap(height + y_offset - y_coord_max, 0, width, y_range.first, y_range.second);

	vector<int> selcted_data_indexs;
	for (int i = 0; i < x_data.size(); i++) {
		if (x_data[i] >= x_min && x_data[i] <= x_max && y_data[i] >= y_min && y_data[i] <= y_max) {
			selcted_data_indexs.push_back(1);
		}
		else {
			selcted_data_indexs.push_back(0);
		}
	}
	ofNotifyEvent(brush_on, selcted_data_indexs, this);
}

void Scatterplot::draw()
{
	ofPushMatrix();
	ofTranslate(x_pos, y_pos);

	ofSetColor(canvas_color);
	ofDrawRectangle(0, 0, width + 2*x_offset, height + 2*y_offset);

	ofSetColor(axis_color);
	ofPoint origin, horizontal_axis_end, vertical_axis_end;
	origin.x = x_offset;
	origin.y = height + y_offset;
	horizontal_axis_end.x = x_offset + width;
	horizontal_axis_end.y = origin.y;
	vertical_axis_end.x = origin.x;
	vertical_axis_end.y = y_offset;
	ofDrawArrow(origin, horizontal_axis_end, 6.0f);
	ofDrawArrow(origin, vertical_axis_end, 6.0f);

	float step_size_x, step_size_y;
	step_size_x = width / num_steps;
	step_size_y = height / num_steps;

	int i;
	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_x*i, 0, width, x_range.first, x_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 3);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + i*step_size_x - 20.0f, origin.y + tick_hor_y_offset);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x + i*step_size_x, origin.y, origin.x + i*step_size_x, origin.y - height);
	}

	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_y*i, 0, height, y_range.first, y_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 3);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + tick_ver_x_offset, origin.y - i*step_size_y + 5.0f);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x, origin.y - i*step_size_y, origin.x + width, origin.y - i*step_size_y);
	}

	if (x_label.compare(y_label) == 0) {
		ofSetColor(label_color);
		verdana_label.drawString(x_label, label_x_offset, label_y_offset);
	}

	ofSetColor(brush_mode?brush_dot_color:dot_color);
	if (brush_active) {
		for (i = 0; i < selected_index.size(); i++) {
			draw_dot(i, origin, selected_index[i]==1);
		}
	}
	else {
		for (i = 0; i < this->x_data.size(); i++) {
			draw_dot(i, origin, true);
		}
	}

	if (brush_mode) {
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

	ofPopMatrix();
}

void Scatterplot::draw_dot(int i, ofPoint origin, bool brushed) {
	ofPoint point;
	point.x = ofMap(x_data[i], x_range.first, x_range.second, 0, width) + x_offset;
	point.y = height - ofMap(y_data[i], y_range.first, y_range.second, 0, height) + y_offset;

	if (is_inside_rect(point.x, point.y, origin.x, origin.y, origin.x + width, origin.y - height)) {
		if (brush_mode) {
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
			ofSetColor(brushed ? dot_color : non_brush_dot_color);
			ofDrawCircle(point, DOT_RADIUS);
		}
	}
}

void Scatterplot::mouseMoved(ofMouseEventArgs & args)
{
}

void Scatterplot::mouseDragged(ofMouseEventArgs & args)
{
	if (brush_mode) {
		ofPoint origin;
		origin.x = x_offset;
		origin.y = height + y_offset;

		brush_end_x = args.x - x_pos > origin.x + width ? origin.x + width :
			args.x - x_pos < origin.x ? origin.x : args.x - x_pos;
		brush_end_y = args.y - y_pos < origin.y - height ? origin.y - height :
			args.y - y_pos > origin.y ? origin.y : args.y - y_pos;

		brush();
	}
}

void Scatterplot::mousePressed(ofMouseEventArgs & args)
{
	if (args.button == 0 || args.button == 1) {
		ofPoint origin;
		origin.x = x_offset;
		origin.y = height + y_offset;
		if (args.x - x_pos > origin.x && args.x - x_pos < origin.x + width &&
			args.y - y_pos < origin.y && args.y - y_pos > origin.y - height) {
			brush_start_x = args.x - x_pos;
			brush_start_y = args.y - y_pos;
			brush_end_x = brush_start_x;
			brush_end_y = brush_start_y;
			brush_mode = true;

			pair<int, bool> brush_event;
			brush_event.first = index;
			brush_event.second = brush_mode;
			ofNotifyEvent(brush_mode_changed, brush_event, this);
		}

		if (brush_mode) {
			brush_end_x = args.x - x_pos > origin.x + width ? origin.x + width :
				args.x - x_pos < origin.x ? origin.x : args.x - x_pos;
			brush_end_y = args.y - y_pos < origin.y - height ? origin.y - height :
				args.y - y_pos > origin.y ? origin.y : args.y - y_pos;

			brush();
		}
	}
	else if (args.button == 2) {
		// reset brush state and all state variables
		brush_mode = false;
		brush_start_x = 0;
		brush_start_y = 0;
		brush_end_x = 0;
		brush_end_y = 0;

		pair<int, bool> brush_event;
		brush_event.first = index;
		brush_event.second = brush_mode;
		ofNotifyEvent(brush_mode_changed, brush_event, this);
	}
}

void Scatterplot::mouseReleased(ofMouseEventArgs & args)
{

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