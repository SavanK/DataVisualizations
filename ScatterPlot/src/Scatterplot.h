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
	const float X_OFFSET = 100.0f;
	const float Y_OFFSET = 100.0f;
	const float DOT_RADIUS = 6.0f;
	const float HOVERED_DOT_RADIUS = 10.0f;
	const int LABEL_FONT_SIZE = 24;
	const int TICK_FONT_SIZE = 18;
	const float LABEL_HOR_X_OFFSET = X_OFFSET;
	const float LABEL_HOR_Y_OFFSET = Y_OFFSET - 30.0f;
	const float LABEL_VER_X_OFFSET = LABEL_HOR_Y_OFFSET - LABEL_FONT_SIZE/2;
	const float LABEL_VER_Y_OFFSET = LABEL_HOR_X_OFFSET;
	const float TICK_HOR_Y_OFFSET = 30.0f;
	const float TICK_VER_X_OFFSET = -50.0f;

	vector<float> x_data;
	vector<float> y_data;
	string x_label;
	string y_label;
	int x_pos, y_pos;
	int width, height;
	pair<float, float> x_range, y_range;
	pair<float, float> x_visible_range, y_visible_range;
	ofTrueTypeFont verdana_label;
	ofTrueTypeFont verdana_tick;
	int num_steps;

	ofColor canvas_color;
	ofColor dot_color;
	ofColor hovered_dot_color;
	ofColor zoom_dot_color;
	ofColor label_color;
	ofColor axis_color;
	ofColor grid_lines_color;
	ofColor tooltip_bg_color;
	ofColor tooltip_label_color;
	ofColor tick_color;
	ofColor zoom_box_bg_color;
	ofColor zoom_box_border_color;

	int hovered_data_index = -1;
	ofEvent<int> hover_on;

	float zoom_start_x = 0;
	float zoom_start_y = 0;
	float zoom_end_x = 0;
	float zoom_end_y = 0;
	bool zoom_mode = false;

	Scatterplot();
	~Scatterplot();

	void setup(int width, int height, int x_pos, int y_pos, int num_steps);
	void set_data(string x_label, string y_label,
		vector<float> x_data, vector<float> y_data);
	void draw();
	int get_mouse_hovered_data_index(float x, float y);
	bool is_inside_rect(float x, float y, float rec_x_start, float rect_y_start, float rect_x_end, float rect_y_end);

	void mouseMoved(ofMouseEventArgs & args);
	void mouseDragged(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	void mouseScrolled(ofMouseEventArgs & args);
	void mouseEntered(ofMouseEventArgs & args);
	void mouseExited(ofMouseEventArgs & args);
};

