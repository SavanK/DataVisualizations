#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "Column.h"
#include "ColumnValue.h"
#include "ofMain.h"
#include "ofEvents.h"

using namespace std;

typedef struct {
	ofPoint start_point;
	float width;
	float height;
} LabelRect;

typedef struct {
	ofPoint start;
	ofPoint end;
	ofColor color;
} PC_Line;

typedef struct {
	ofPoint start;
	ofPoint bg_start;
	float bg_width;
	float bg_height;
	string tick_str;
} PC_Tick;

typedef struct {
	ofPoint start;
	char* min_str = NULL;
	char* max_str = NULL;
	ofPoint min_str_pt;
	ofPoint max_str_pt;
	vector<PC_Tick*> ticks;
	float brush_start_y;
	float brush_end_y;
	float offset_coord_y;
	float median_y;
	float lower_q_y;
	float upper_q_y;
	float lower_w_y;
	float upper_w_y;
} PC_Axis;

typedef struct {
	int axis_index;
	int min_index;
	int max_index;
	float rect_start_y;
} PC_Zoom;

class ParallelCoord
{
private:
	static const int OFFSET_X = 0;
	static const int OFFSET_Y = 25;

	static const int CONFIG_OFFSET = 175;
	static const int CHECK_MARK_SIZE = 40;
	
	static const int TICK_COUNT = 4;
	static const int MAX_GROUPS = 60;

	static const int BRUSH_MARGIN = 20;
	static const int ZOOM_ENTRY_COUNT = 4;

	static const int BOX_PLOT_WIDTH = 50;

	static const int MOUSE_MODE_INVALID = 0;
	static const int MOUSE_MODE_LABEL_CLICK = 1;
	static const int MOUSE_MODE_LABEL_DRAG = 2;
	static const int MOUSE_MODE_AXIS_CLICK = 3;
	static const int MOUSE_MODE_AXIS_BRUSH = 4;
	static const int MOUSE_MODE_ZOOM = 5;

	static const int DRAW_STATE_INVALIDATE = 0;
	static const int DRAW_STATE_STABLE = 1;

	static const int COLOR_CONFIG_SINGLE = 0;
	static const int COLOR_CONFIG_TRANSITION = 1;
	static const int COLOR_CONFIG_GROUP = 2;

	string filename;
	vector<Column*> columns;
	vector<vector<ColumnValue*>> column_values;
	// will support upto 32 columns on a m/c where unsigned int takes 4bytes
	// should extend mask to long if necessary
	// however, you can hardly drawn >32 axes on a regular screen
	// so effective optimization
	vector<unsigned> data_set_mask;
	unsigned brushed_mask = 0;
	vector<PC_Line*> lines;
	vector<PC_Axis*> axes;
	PC_Zoom *zoom;
	int draw_state;
	int x, y;
	int width, height;
	int plot_width, plot_height;
	int meta_data_height, meta_data_width;
	int axis_width, axis_height;
	int zoom_height, zoom_width, zoom_offset;
	int axis_y;
	int title_font_size;
	int label_font_size;
	int config_font_size;
	int zoom_label_font_size;
	int tick_font_size;
	int mouse_mode;
	int click_label_index;
	int drop_label_index;
	int click_axis_index;

	float title_x, title_y;
	float color_config_x, color_config_y;
	float box_plot_config_x, box_plot_config_y;
	float color_config_opt_y, box_plot_config_opt_y;
	float color_config_opt_single_x;
	float color_config_opt_transition_x;
	float color_config_opt_group_x;
	float box_plot_config_opt_status_x;

	string color_config = "Line color";
	string box_plot_config = "Box plot";
	string color_config_single = "Single color";
	string color_config_transition = "Transition";
	string color_config_group = "Group color";
	string box_plot_config_status = "Show / Hide";

	ofImage checkmark, checkbox;

	int color_config_sel = COLOR_CONFIG_SINGLE;
	bool box_plot_sel = false;

	ofTrueTypeFont verdana_title;
	ofTrueTypeFont verdana_label;
	ofTrueTypeFont verdana_config;
	ofTrueTypeFont verdana_tick;
	ofTrueTypeFont verdana_zoom_label;

	ofColor bg_color;
	ofColor axis_color;
	ofColor axis_brush_color;
	ofColor title_color;
	ofColor label_color;
	ofColor label_drag_color;
	ofColor tick_color;
	ofColor tick_bg_color;
	ofColor line_color;
	ofColor line_unbrushed_color;
	ofColor zoom_bg_color;
	ofColor zoom_label_color;
	ofColor box_plot_color;
	ofColor box_plot_bg_color;
	vector<ofColor> group_color;
	vector<ofColor> transition_color;

	ofPoint mouse_drag_pos;
	vector<LabelRect*> label_rects;

	float calculate_offset_coord_y(int column_index);
	float translate_val_to_coord(int column_index, int data_point_index);
	bool translate_range_coords_to_index(int column_index, float min_val, float max_val, 
		int *min_index, int *max_index);
	bool is_inside_rect(float x, float y,
		float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end);
	void brush_axis();

public:
	ParallelCoord();
	~ParallelCoord();

	void set_filename(string name);
	void set_bounds(int x, int y, int width, int height);
	void set_columns(vector<Column*> c);
	void set_column_values(vector<vector<ColumnValue*>> c_values);
	void draw();
	void update();
	void reset();

	void mouseMoved(ofMouseEventArgs & args);
	void mouseDragged(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	void mouseScrolled(ofMouseEventArgs & args);
	void mouseEntered(ofMouseEventArgs & args);
	void mouseExited(ofMouseEventArgs & args);
};