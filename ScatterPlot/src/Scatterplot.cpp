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

	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);
	//Loading verdana font
	verdana_label.load("verdana.ttf", LABEL_FONT_SIZE, true, true);
	verdana_label.setLineHeight(20.0f);
	verdana_label.setLetterSpacing(1.037);

	verdana_tick.load("verdana.ttf", TICK_FONT_SIZE, true, true);
	verdana_tick.setLineHeight(20.0f);
	verdana_tick.setLetterSpacing(1.037);

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
	zoom_dot_color.set(60, 60, 60);
	zoom_box_bg_color.set(31, 224, 43, 50);
	zoom_box_border_color.set(15, 105, 20);

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
	x_visible_range = make_pair(min_x, max_x);
	y_visible_range = make_pair(min_y, max_y);
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

void Scatterplot::draw()
{
	ofPushMatrix();
	ofTranslate(x_pos, y_pos);

	ofSetColor(canvas_color);
	ofDrawRectangle(0, 0, width + 2*X_OFFSET, height + 2*Y_OFFSET);

	ofSetColor(axis_color);
	ofPoint origin, horizontal_axis_end, vertical_axis_end;
	origin.x = X_OFFSET;
	origin.y = height + Y_OFFSET;
	horizontal_axis_end.x = X_OFFSET + width;
	horizontal_axis_end.y = origin.y;
	vertical_axis_end.x = origin.x;
	vertical_axis_end.y = Y_OFFSET;
	ofDrawArrow(origin, horizontal_axis_end, 6.0f);
	ofDrawArrow(origin, vertical_axis_end, 6.0f);

	ofSetColor(label_color);
	verdana_label.drawString(x_label, origin.x + LABEL_HOR_X_OFFSET, origin.y + LABEL_HOR_Y_OFFSET);
	
	ofPushMatrix();
	ofRotate(-90.0f, 0.0f, 0.0f, 1.0f);
	verdana_label.drawString(y_label, LABEL_VER_Y_OFFSET - origin.y, origin.x - LABEL_VER_X_OFFSET);
	ofPopMatrix();

	float step_size_x, step_size_y;
	step_size_x = width / num_steps;
	step_size_y = height / num_steps;

	int i;
	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_x*i, 0, width, x_visible_range.first, x_visible_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 4);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + i*step_size_x - 20.0f, origin.y + TICK_HOR_Y_OFFSET);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x + i*step_size_x, origin.y, origin.x + i*step_size_x, origin.y - height);
	}

	for (i = 1; i < num_steps; i++) {
		float val = ofMap(step_size_y*i, 0, height, y_visible_range.first, y_visible_range.second);
		string val_str = to_string(val);
		val_str = val_str.substr(0, 4);
		ofSetColor(tick_color);
		verdana_tick.drawString(val_str, origin.x + TICK_VER_X_OFFSET, origin.y - i*step_size_y + 5.0f);

		ofSetColor(grid_lines_color);
		ofDrawLine(origin.x, origin.y - i*step_size_y, origin.x + width, origin.y - i*step_size_y);
	}

	ofSetColor(zoom_mode?zoom_dot_color:dot_color);
	for (i = 0; i < this->x_data.size(); i++) {
		ofPoint point;
		point.x = ofMap(x_data[i], x_visible_range.first, x_visible_range.second, 0, width) + X_OFFSET;
		point.y = height - ofMap(y_data[i], y_visible_range.first, y_visible_range.second, 0, height) + Y_OFFSET;

		if (is_inside_rect(point.x, point.y, origin.x, origin.y, origin.x + width, origin.y - height)) {
			if (i == hovered_data_index) {
				// draw nothing here
			}
			else {
				if (zoom_mode) {
					bool is_in_rect = false;
					if (is_inside_rect(point.x, point.y, zoom_start_x, zoom_start_y, zoom_end_x, zoom_end_y)) {
						is_in_rect = true;
					}

					if (is_in_rect) {
						ofSetColor(dot_color);
					}
					ofDrawCircle(point, DOT_RADIUS);
					if (is_in_rect) {
						ofSetColor(zoom_dot_color);
					}
				}
				else {
					ofDrawCircle(point, DOT_RADIUS);
				}
			}
		}
	}

	if (zoom_mode) {
		ofPath path = ofPath();
		path.setMode(ofPath::POLYLINES);
		path.setStrokeColor(zoom_box_border_color);
		path.setStrokeWidth(3);
		path.setFilled(false);
		path.moveTo(zoom_start_x, zoom_start_y);
		path.lineTo(zoom_start_x, zoom_end_y);
		path.lineTo(zoom_end_x, zoom_end_y);
		path.lineTo(zoom_end_x, zoom_start_y);
		path.close();
		path.draw();

		ofSetColor(zoom_box_bg_color);
		ofDrawRectangle(zoom_start_x, zoom_start_y, (zoom_end_x - zoom_start_x), (zoom_end_y - zoom_start_y));
	}
	else {
		if (hovered_data_index != -1) {
			ofSetColor(hovered_dot_color);
			ofPoint point;
			point.x = ofMap(x_data[hovered_data_index], x_visible_range.first, x_visible_range.second, 0, width) + X_OFFSET;
			point.y = height - ofMap(y_data[hovered_data_index], y_visible_range.first, y_visible_range.second, 0, height) + Y_OFFSET;
			ofDrawCircle(point, HOVERED_DOT_RADIUS);

			point.x += (HOVERED_DOT_RADIUS + 2);
			point.y -= (HOVERED_DOT_RADIUS + 2);

			ofSetColor(tooltip_bg_color);
			int width = 140;
			int height = 40;
			ofDrawRectangle(point.x, point.y - height, width, height);

			ofSetColor(tooltip_label_color);
			string x_val_str = to_string(x_data[hovered_data_index]);
			x_val_str = x_val_str.substr(0, 5);
			string y_val_str = to_string(y_data[hovered_data_index]);
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
	for (int i = 0; i < x_data.size(); i++) {
		ofPoint point;
		point.x = ofMap(x_data[i], x_visible_range.first, x_visible_range.second, 0, width) + X_OFFSET;
		point.y = height - ofMap(y_data[i], y_visible_range.first, y_visible_range.second, 0, height) + Y_OFFSET;
		float distance = ofVec2f(point).distance(ofVec2f(x-x_pos, y-y_pos));
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
	if (zoom_mode) {
		ofPoint origin;
		origin.x = X_OFFSET;
		origin.y = height + Y_OFFSET;

		zoom_end_x = args.x - x_pos > origin.x + width ? origin.x + width : 
			args.x - x_pos < origin.x ? origin.x : args.x - x_pos;
		zoom_end_y = args.y - y_pos < origin.y - height ? origin.y - height : 
			args.y - y_pos > origin.y ? origin.y : args.y - y_pos;
	}
}

void Scatterplot::mousePressed(ofMouseEventArgs & args)
{
	if (args.button == 0 || args.button == 1) {
		ofPoint origin;
		origin.x = X_OFFSET;
		origin.y = height + Y_OFFSET;
		if (args.x - x_pos > origin.x && args.x - x_pos < origin.x + width &&
			args.y - y_pos < origin.y && args.y - y_pos > origin.y - height) {
			zoom_start_x = args.x - x_pos;
			zoom_start_y = args.y - y_pos;
			zoom_end_x = zoom_start_x;
			zoom_end_y = zoom_start_y;
			zoom_mode = true;
		}
	}
	else if (args.button == 2) {
		x_visible_range.first = x_range.first;
		x_visible_range.second = x_range.second;
		y_visible_range.first = y_range.first;
		y_visible_range.second = y_range.second;
	}
}

void Scatterplot::mouseReleased(ofMouseEventArgs & args)
{
	if (zoom_mode) {
		ofPoint origin;
		origin.x = X_OFFSET;
		origin.y = height + Y_OFFSET;

		zoom_end_x = args.x - x_pos > origin.x + width ? origin.x + width :
			args.x - x_pos < origin.x ? origin.x : args.x - x_pos;
		zoom_end_y = args.y - y_pos < origin.y - height ? origin.y - height :
			args.y - y_pos > origin.y ? origin.y : args.y - y_pos;

		float x_coord_min, x_coord_max, y_coord_min, y_coord_max;
		x_coord_min = zoom_start_x < zoom_end_x ? zoom_start_x : zoom_end_x;
		x_coord_max = zoom_start_x > zoom_end_x ? zoom_start_x : zoom_end_x;
		y_coord_min = zoom_start_y < zoom_end_y ? zoom_start_y : zoom_end_y;
		y_coord_max = zoom_start_y > zoom_end_y ? zoom_start_y : zoom_end_y;

		float x_min = ofMap(x_coord_min - X_OFFSET, 0, width, x_visible_range.first, x_visible_range.second);
		float x_max = ofMap(x_coord_max - X_OFFSET, 0, width, x_visible_range.first, x_visible_range.second);
		float y_max = ofMap(height + Y_OFFSET - y_coord_min, 0, width, y_visible_range.first, y_visible_range.second);
		float y_min = ofMap(height + Y_OFFSET - y_coord_max, 0, width, y_visible_range.first, y_visible_range.second);

		x_visible_range.first = x_min;
		x_visible_range.second = x_max;
		y_visible_range.first = y_min;
		y_visible_range.second = y_max;

		// reset zoom state and all state variables
		zoom_mode = false;
		zoom_start_x = 0;
		zoom_start_y = 0;
		zoom_end_x = 0;
		zoom_end_y = 0;
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