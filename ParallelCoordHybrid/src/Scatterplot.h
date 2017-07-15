#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include <iostream>
#include <vector>
#include <string>
#include "Column.h"
#include "ColumnValue.h"

using namespace std;

class Scatterplot
{
public:
	static const int MOUSE_MODE_INVALID = 0;
	static const int MOUSE_MODE_SLIDER_CLICK = 1;
	static const int MOUSE_MODE_SLIDER_DRAG = 2;
	static const int MOUSE_MODE_BRUSH_CLICK = 3;
	static const int MOUSE_MODE_BRUSH_DRAG = 4;
	static const int MOUSE_MODE_BRUSH_STABLE = 5;

	static const int SLIDER_NONE = -1;
	static const int SLIDER_X_AXIS = 0;
	static const int SLIDER_Y_AXIS = 1;

	static const int LABEL_FONT_SIZE = 24;
	static const int LABEL_SEL_COLUMN_FONT_SIZE = 24;
	static const int TICK_FONT_SIZE = 18;
	const float DOT_RADIUS = 6.0f;
	const float HOVERED_DOT_RADIUS = 10.0f;
	const float TICK_HOR_Y_OFFSET = 30.0f;
	const float TICK_VER_X_OFFSET = -50.0f;
	const float SLIDER_DOT_OFFSET = 12.0f;

	vector<Column*> columns;
	vector<vector<ColumnValue*>> column_values;
	float x_pos, y_pos;
	float sp_x_pos, sp_y_pos;
	float width, height;
	float sp_width, sp_height;
	float col_sel_width, col_sel_height;
	float col_sel_x;
	float x_offset, y_offset;
	pair<float, float> x_range, y_range;
	vector<int> available_columns;
	int selected_x_column, selected_y_column;
	vector<unsigned*> data_set_mask;

	ofTrueTypeFont verdana_label;
	ofTrueTypeFont verdana_sel_column_label;
	ofTrueTypeFont verdana_tick;
	int num_steps;

	ofColor canvas_color;
	ofColor dot_color;
	ofColor hovered_dot_color;
	ofColor brush_dot_color;
	ofColor label_color;
	ofColor axis_color;
	ofColor grid_lines_color;
	ofColor tooltip_bg_color;
	ofColor tooltip_label_color;
	ofColor tick_color;
	ofColor brush_box_bg_color;
	ofColor brush_box_border_color;
	ofColor slider_line_color;
	ofColor slider_dot_color;
	ofColor slider_selected_dot_color;
	ofColor slider_drag_sel_dot_color;
	ofColor slider_drag_shadow_dot_color;

	int hovered_data_index = -1;
	ofEvent<int> hover_on;

	int mouse_mode;

	float brush_start_x = 0;
	float brush_start_y = 0;
	float brush_end_x = 0;
	float brush_end_y = 0;

	int which_slider;
	int clicked_dot_index;
	int dragged_dot_index;
	float mouse_drag_x;

	Scatterplot();
	~Scatterplot();

	void setup(float x_pos, float y_pos, float width, float height, int num_steps);
	void set_columns(vector<Column*> c);
	void set_column_values(vector<vector<ColumnValue*>> c_values);
	void draw();
	int get_mouse_hovered_data_index(float x, float y);
	bool is_inside_rect(float x, float y, float rec_x_start, float rect_y_start, float rect_x_end, float rect_y_end);
	bool is_inside_circle(float x, float y, float center_x, float center_y, float radius);
	void get_values(int data_index, float *val_x, float *val_y);
	void set_brush_mask(vector<unsigned*> brush_mask);
	void calculate_range();
	void reset();
	void reset_brush_mode();
	void reset_slider_mode();

	void mouseMoved(ofMouseEventArgs & args);
	void mouseDragged(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	void mouseScrolled(ofMouseEventArgs & args);
	void mouseEntered(ofMouseEventArgs & args);
	void mouseExited(ofMouseEventArgs & args);
};

