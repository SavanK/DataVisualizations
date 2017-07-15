#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Scatterplot
{
public:
	float x_offset;
	float y_offset;
	const float DOT_RADIUS = 4.0f;
	const float HOVERED_DOT_RADIUS = 6.0f;
	int label_font_size;
	int tick_font_size;
	float label_x_offset;
	float label_y_offset;
	float tick_hor_y_offset;
	float tick_ver_x_offset;

	vector<float> x_data;
	vector<float> y_data;
	string x_label;
	string y_label;
	int x_pos, y_pos;
	int width, height;
	pair<float, float> x_range, y_range;
	ofTrueTypeFont verdana_label;
	ofTrueTypeFont verdana_tick;
	int num_steps;
	vector<int> selected_index;

	ofColor canvas_color;
	ofColor dot_color;
	ofColor hovered_dot_color;
	ofColor brush_dot_color;
	ofColor non_brush_dot_color;
	ofColor label_color;
	ofColor axis_color;
	ofColor grid_lines_color;
	ofColor tooltip_bg_color;
	ofColor tooltip_label_color;
	ofColor tick_color;
	ofColor brush_box_bg_color;
	ofColor brush_box_border_color;

	ofEvent<vector<int>> brush_on;
	ofEvent<pair<int, bool>> brush_mode_changed;

	float brush_start_x = 0;
	float brush_start_y = 0;
	float brush_end_x = 0;
	float brush_end_y = 0;
	bool brush_mode = false;
	bool brush_active = false;
	int index;

	Scatterplot();
	~Scatterplot();

	void setup(int width, int height, int x_pos, int y_pos, int num_steps);
	void set_data(string x_label, string y_label,
		vector<float> x_data, vector<float> y_data);
	void draw();
	void draw_dot(int i, ofPoint origin, bool brushed);
	bool is_inside_rect(float x, float y, float rec_x_start, float rect_y_start, 
		float rect_x_end, float rect_y_end);
	void set_selected_index(vector<int> selected_index);
	void set_brush_active(int index, bool active);
	void set_slot_index(int index);
	void brush();

	void mouseMoved(ofMouseEventArgs & args);
	void mouseDragged(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	void mouseScrolled(ofMouseEventArgs & args);
	void mouseEntered(ofMouseEventArgs & args);
	void mouseExited(ofMouseEventArgs & args);
};

