#include "ParallelCoord.h"

float ParallelCoord::calculate_offset_coord_y(int column_index)
{
	float offset = 0.0f;
	if (columns[column_index]->get_type() == Column::TYPE_STRING) {
		if (columns[column_index]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
			vector<string> group_names = columns[column_index]->get_group_names();
			offset = axis_height / (float)(group_names.size() - 1);
		}
		else {
			offset = axis_height / (float)(column_values[column_index].size() - 1);
		}
	}
	return offset;
}

float ParallelCoord::translate_val_to_coord(int column_index, int data_point_index)
{
	float point_y;
	int val_i;
	int max_i, min_i;
	float val_f;
	float max_f, min_f;
	switch (columns[column_index]->get_type()) {
	case Column::TYPE_INT:
		val_i;
		max_i = columns[column_index]->get_max_i();
		min_i = columns[column_index]->get_min_i();
		column_values[column_index][data_point_index]->get_value(&val_i);
		point_y = ofMap(val_i, min_i, max_i, 0, axis_height);
		if (columns[column_index]->get_draw_order() == Column::MAX_TO_MIN) {
			point_y = axis_height - point_y;
		}
		break;

	case Column::TYPE_FLOAT:
		val_f;
		max_f = columns[column_index]->get_max_f();
		min_f = columns[column_index]->get_min_f();
		column_values[column_index][data_point_index]->get_value(&val_f);
		point_y = ofMap(val_f, min_f, max_f, 0, axis_height);
		if (columns[column_index]->get_draw_order() == Column::MAX_TO_MIN) {
			point_y = axis_height - point_y;
		}
		break;

	case Column::TYPE_STRING:
		point_y = 0;
		if (columns[column_index]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
			vector<string> group_names = columns[column_index]->get_group_names();
			string g_name = string();
			column_values[column_index][data_point_index]->get_value(&g_name);
			float group_offset = axis_height / (float)(group_names.size() - 1);
			for (int k = 0; k < group_names.size(); k++) {
				if (g_name.compare(group_names[k]) == 0) {
					point_y = k * group_offset;
					break;
				}
			}
		}
		else {
			float offset = axis_height / (float) (column_values[column_index].size() - 1);
			point_y = data_point_index * offset;
		}
		break;
	}
	return point_y;
}

bool ParallelCoord::translate_range_coords_to_index(int column_index, float min_val, float max_val, 
	int * min_index, int * max_index)
{
	bool ret_val;
	if (columns[column_index]->get_type() == Column::TYPE_STRING) {
		*min_index = ceil(min_val / axes[column_index]->offset_coord_y);
		*max_index = floor(max_val / axes[column_index]->offset_coord_y);
		ret_val = true;
	}
	else {
		*min_index = -1;
		*max_index = -1;
		ret_val = false;
	}
	return ret_val;
}

bool ParallelCoord::is_inside_rect(float x, float y, 
	float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end)
{
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

void ParallelCoord::brush_axis()
{
	float max_data, min_data;
	if (columns[click_axis_index]->get_type() == Column::TYPE_INT) {
		max_data = columns[click_axis_index]->get_max_i();
		min_data = columns[click_axis_index]->get_min_i();
	}
	else if (columns[click_axis_index]->get_type() == Column::TYPE_FLOAT) {
		max_data = columns[click_axis_index]->get_max_f();
		min_data = columns[click_axis_index]->get_min_f();
	}

	float max_coord, min_coord;
	if (columns[click_axis_index]->get_draw_order() == Column::MIN_TO_MAX) {
		min_coord = axis_y + axis_height;
		max_coord = axis_y;
	}
	else {
		min_coord = axis_y;
		max_coord = axis_y + axis_height;
	}

	float a = ofMap(axes[click_axis_index]->brush_start_y, min_coord, max_coord, min_data, max_data);
	float b = ofMap(axes[click_axis_index]->brush_end_y, min_coord, max_coord, min_data, max_data);

	float start_data = MIN(a, b);
	float end_data = MAX(a, b);

	for (int k = 0; k < column_values[click_axis_index].size(); k++) {
		ColumnValue *col_val = column_values[click_axis_index][k];
		if (columns[click_axis_index]->get_type() == Column::TYPE_INT) {
			int val_i;
			col_val->get_value(&val_i);
			if (val_i >= start_data && val_i <= end_data) {
				// brushed
				data_set_mask[k] |= 1u << click_axis_index;
			}
			else {
				// unbrushed
				data_set_mask[k] &= ~(1u << click_axis_index);
			}
		}
		else if (columns[click_axis_index]->get_type() == Column::TYPE_FLOAT) {
			float val_f;
			col_val->get_value(&val_f);
			if (val_f >= start_data && val_f < end_data) {
				// brushed
				data_set_mask[k] |= 1u << click_axis_index;
			}
			else {
				// unbrushed
				data_set_mask[k] &= ~(1u << click_axis_index);
			}
		}
	}
}

ParallelCoord::ParallelCoord()
{
	mouse_mode = ParallelCoord::MOUSE_MODE_INVALID;
	draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
	zoom = new PC_Zoom();
}

ParallelCoord::~ParallelCoord()
{
}

void ParallelCoord::set_filename(string name)
{
	filename = name;
}

void ParallelCoord::set_bounds(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	plot_width = width - 2*ParallelCoord::OFFSET_X;
	plot_height = height - 2 * ParallelCoord::OFFSET_Y;
	axis_width = 10;
	axis_height = 0.7f * plot_height; // 70% plot height
	axis_y = 0.25f * plot_height;
	meta_data_height = 0.23f * plot_height;
	meta_data_width = plot_width;
	zoom_height = 0.2f * axis_height; // 20% axis height
	// zoom_width is set with columns

	bg_color.set(255, 255, 255);
	label_color.set(60, 60, 60);
	title_color.set(60, 60, 60);
	label_drag_color.set(60, 60, 60, 140);
	tick_color.set(255, 255, 255);
	tick_bg_color.set(60, 60, 60, 140);
	zoom_label_color.set(255, 255, 255);
	zoom_bg_color.set(60, 60, 60, 60);
	axis_color.set(90, 90, 90, 140);
	axis_brush_color.set(90, 90, 90);
	line_color.setHsb(14, 255, 255);
	line_unbrushed_color.set(120, 120, 120, 50);
	box_plot_color.set(60, 60, 60, 140);
	box_plot_bg_color.set(60, 60, 60, 80);

	checkbox.loadImage("checkbox.png");
	checkmark.loadImage("checkmark.png");

	//Register for mouse event callbacks
	ofRegisterMouseEvents(this);
}

void ParallelCoord::set_columns(vector<Column*> c)
{
	columns = c;

	int col_count = c.size();

	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);
	title_font_size = 42;
	label_font_size = 22;
	zoom_label_font_size = 18;
	tick_font_size = 14;
	config_font_size = 28;

	//Loading verdana font
	verdana_title.load("verdana.ttf", title_font_size, true, true);
	verdana_title.setLineHeight(20.0f);
	verdana_title.setLetterSpacing(1.2f);

	verdana_label.load("verdana.ttf", label_font_size, true, true);
	verdana_label.setLineHeight(20.0f);
	verdana_label.setLetterSpacing(1.037f);

	verdana_config.load("verdana.ttf", config_font_size, true, true);
	verdana_config.setLineHeight(20.0f);
	verdana_config.setLetterSpacing(1.037f);

	verdana_tick.load("verdana.ttf", tick_font_size, true, true);
	verdana_tick.setLineHeight(20.0f);
	verdana_tick.setLetterSpacing(1.037f);

	verdana_zoom_label.load("verdana.ttf", zoom_label_font_size, true, true);
	verdana_zoom_label.setLineHeight(20.0f);
	verdana_zoom_label.setLetterSpacing(1.037f);

	int grouped_column_index = -1;
	for (int i = 0; i < columns.size(); i++) {
		if (columns[i]->get_type() == Column::TYPE_STRING &&
			columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
			grouped_column_index = i;
			break;
		}
	}

	float deno = (grouped_column_index != -1 && grouped_column_index != 0) ? columns.size() + 2 : columns.size() + 1;
	float offset = plot_width / deno;
	zoom_width = 0.9f * offset; // 90% offset between axes
	zoom_offset = 0.1f * offset; // 10% offset between axes

	if (grouped_column_index != -1) {
		int group_count = columns[grouped_column_index]->get_group_names().size();

		float hue_offset = 255 / (float)group_count;
		for (int i = 0; i < group_count; i++) {
			ofColor color;
			color.setHsb((int)(hue_offset*i), 190, 190);
			group_color.push_back(color);
		}
	}

	for (int i = 0; i < columns.size(); i++) {
		LabelRect* rect = new LabelRect();
		label_rects.push_back(rect);
		PC_Axis* axis = new PC_Axis();
		axis->brush_end_y = -1;
		axis->brush_start_y = -1;
		axes.push_back(axis);
	}
}

void ParallelCoord::set_column_values(vector<vector<ColumnValue*>> c_values)
{
	column_values = c_values;

	for (int i = 0; i < (columns.size() - 1) * column_values[0].size(); i++) {
		PC_Line *line = new PC_Line();
		lines.push_back(line);
	}

	for (int i = 0; i < columns.size(); i++) {
		brushed_mask |= 1 << i;
	}

	float red_offset = 255 / (float)column_values[0].size();
	for (int i = 0; i < column_values[0].size(); i++) {
		data_set_mask.push_back(brushed_mask);
		ofColor color;
		color.set((i + 1)*red_offset, 125, 168);
		transition_color.push_back(color);
	}

}

void ParallelCoord::draw()
{
	ofPushMatrix();
	ofTranslate(x + ParallelCoord::OFFSET_X, y + ParallelCoord::OFFSET_Y);

	// Draw background
	ofSetColor(bg_color);
	ofDrawRectangle(0, 0, plot_width, plot_height);

	// Draw title
	ofSetColor(title_color);
	verdana_title.drawString(filename, title_x, title_y);

	// Draw color & box plot configuration
	verdana_config.drawString(color_config, color_config_x, color_config_y);
	verdana_config.drawString(box_plot_config, box_plot_config_x, box_plot_config_y);

	verdana_label.drawString(color_config_single, color_config_opt_single_x + ParallelCoord::CHECK_MARK_SIZE, 
		color_config_opt_y);
	verdana_label.drawString(color_config_transition, color_config_opt_transition_x + ParallelCoord::CHECK_MARK_SIZE, 
		color_config_opt_y);
	verdana_label.drawString(color_config_group, color_config_opt_group_x + ParallelCoord::CHECK_MARK_SIZE, 
		color_config_opt_y);
	verdana_label.drawString(box_plot_config_status, box_plot_config_opt_status_x + ParallelCoord::CHECK_MARK_SIZE, 
		box_plot_config_opt_y);

	checkbox.draw(color_config_opt_single_x, color_config_opt_y - ParallelCoord::OFFSET_Y,
		ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);
	checkbox.draw(color_config_opt_transition_x, color_config_opt_y - ParallelCoord::OFFSET_Y,
		ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);
	checkbox.draw(color_config_opt_group_x, color_config_opt_y - ParallelCoord::OFFSET_Y,
		ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);
	checkbox.draw(box_plot_config_opt_status_x, color_config_opt_y - ParallelCoord::OFFSET_Y,
		ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);

	int color_config_off = 3;
	switch (color_config_sel) {
	case COLOR_CONFIG_SINGLE:
		color_config_off = 3;
		break;
	case COLOR_CONFIG_TRANSITION:
		color_config_off = 2;
		break;
	case COLOR_CONFIG_GROUP:
		color_config_off = 1;
		break;
	}
	checkmark.draw((plot_width / 2) - ParallelCoord::CONFIG_OFFSET * color_config_off, color_config_opt_y - ParallelCoord::OFFSET_Y,
		ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);
	if (box_plot_sel) {
		checkmark.draw(box_plot_config_opt_status_x, color_config_opt_y  - ParallelCoord::OFFSET_Y,
			ParallelCoord::CHECK_MARK_SIZE, ParallelCoord::CHECK_MARK_SIZE);
	}

	// Draw lines
	int data_set_count = column_values[0].size();
	for (int k = 0; k < lines.size(); k++) {
		if (data_set_mask[k % data_set_count] == brushed_mask) {
			ofSetColor(lines[k]->color);
		}
		else {
			ofSetColor(line_unbrushed_color);
		}
		ofDrawLine(lines[k]->start.x, lines[k]->start.y, lines[k]->end.x, lines[k]->end.y);
	}

	// Draw vertical axis & labels
	int grouped_column_index = -1;
	for (int i = 0; i < columns.size(); i++) {
		if (columns[i]->get_type() == Column::TYPE_STRING && 
			columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
			grouped_column_index = i;
			break;
		}
	}

	float deno = (grouped_column_index != -1 && grouped_column_index != 0) ? columns.size() + 2: columns.size() + 1;
	float offset = plot_width / deno;
	for (int i = 1; i <= columns.size(); i++) {
		int x = (grouped_column_index != -1 && grouped_column_index != 0 && grouped_column_index <= i - 1) ? 
			(i + 1)*offset : i*offset;

		// Draw axes
		ofSetColor(axis_color);
		PC_Axis* axis = axes[i - 1];
		ofDrawRectangle(axis->start.x, axis->start.y, axis_width, axis_height);
		if (axis->brush_end_y != -1 && axis->brush_start_y != -1) {
			ofSetColor(axis_brush_color);
			float startY = MIN(axis->brush_start_y, axis->brush_end_y);
			float diff = ABS(axis->brush_start_y - axis->brush_end_y);
			ofDrawRectangle(axis->start.x, startY, axis_width, diff);
		}

		// Draw box plot
		if (box_plot_sel && (columns[i - 1]->get_type() == Column::TYPE_FLOAT ||
			columns[i - 1]->get_type() == Column::TYPE_INT)) {
			ofSetColor(box_plot_bg_color);
			ofDrawRectangle(x - ParallelCoord::BOX_PLOT_WIDTH / 2, MIN(axis->lower_q_y, axis->upper_q_y), 
				ParallelCoord::BOX_PLOT_WIDTH, abs(axis->upper_q_y - axis->lower_q_y));

			ofPath path = ofPath();
			path.setMode(ofPath::POLYLINES);
			path.setStrokeColor(box_plot_color);
			path.setStrokeWidth(3.0f);
			path.setFilled(false);
			path.moveTo(x - ParallelCoord::BOX_PLOT_WIDTH / 2, axis->lower_q_y);
			path.lineTo(x + ParallelCoord::BOX_PLOT_WIDTH / 2, axis->lower_q_y);
			path.lineTo(x + ParallelCoord::BOX_PLOT_WIDTH / 2, axis->upper_q_y);
			path.lineTo(x - ParallelCoord::BOX_PLOT_WIDTH / 2, axis->upper_q_y);
			path.close();
			path.draw();

			ofPath path2 = ofPath();
			path2.setMode(ofPath::POLYLINES);
			path2.setStrokeColor(box_plot_color);
			path2.setFilled(false);
			path2.setStrokeWidth(6.0f);
			path2.moveTo(x - 15, axis->median_y);
			path2.lineTo(x + 15, axis->median_y);
			path2.draw();

			path2.moveTo(x - 15, axis->lower_w_y);
			path2.lineTo(x + 15, axis->lower_w_y);
			path2.draw();

			path2.moveTo(x - 15, axis->upper_w_y);
			path2.lineTo(x + 15, axis->upper_w_y);
			path2.draw();
		}

		// Draw min-max values
		if (axis->max_str != NULL && axis->min_str != NULL) {
			ofSetColor(label_color);
			verdana_label.drawString(axis->max_str, axis->max_str_pt.x, axis->max_str_pt.y);
			verdana_label.drawString(axis->min_str, axis->min_str_pt.x, axis->min_str_pt.y);
		}

		// Draw ticks
		for (int k = 0; k < axis->ticks.size(); k++) {
			PC_Tick* tick = axis->ticks[k];
			ofSetColor(tick_bg_color);
			ofDrawRectangle(tick->bg_start.x, tick->bg_start.y, tick->bg_width, tick->bg_height);
			ofSetColor(tick_color);
			verdana_tick.drawString(tick->tick_str, tick->start.x, tick->start.y);
		}

		// Draw labels
		float str_width, str_height;
		ofSetColor(label_color);
		LabelRect *rect = label_rects[i - 1];
		verdana_label.drawString(columns[i - 1]->get_name(), rect->start_point.x, rect->start_point.y);
	}

	if (mouse_mode == ParallelCoord::MOUSE_MODE_LABEL_DRAG) {
		// Draw drag shadow
		ofSetColor(label_drag_color);
		float drag_y_fixed = meta_data_height - 30 * 2 - 20;
		float drag_x = mouse_drag_pos.x;
		float str_width = verdana_label.stringWidth(columns[click_label_index]->get_name());
		verdana_label.drawString(columns[click_label_index]->get_name(), drag_x - (str_width / 2.0f), drag_y_fixed);
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_ZOOM) {
		// Draw zoom rectangle
		ofSetColor(zoom_bg_color);
		ofDrawRectangle(axes[zoom->axis_index]->start.x - zoom_width - zoom_offset, zoom->rect_start_y, 
			zoom_width, zoom_height);
		ofDrawRectangle(15, 15, 0.42f*plot_width, zoom_height);
		float zoom_rect_offset_y = zoom_height / (zoom->max_index - zoom->min_index + 2);
		int data_set_count = column_values[0].size();

		for (int k = zoom->min_index; k <= zoom->max_index; k++) {
			ofColor color = line_color;
			string str;
			if (columns[zoom->axis_index]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
				vector<string> group_names = columns[zoom->axis_index]->get_group_names();
				if (k < group_names.size()) {
					str = group_names[k];
					color = group_color[group_names.size() - k];
				}
			}
			else {
				if (k < data_set_count) {
					column_values[zoom->axis_index][k]->get_value(&str);
					color = lines[zoom->axis_index*data_set_count + (data_set_count - 1) - k]->color;
				}
			}
			string truncated_str;
			if (str.length() > 14) {
				truncated_str = str.substr(0, 14);
			}
			else {
				truncated_str = str;
			}
			ofSetColor(color);

			ofDrawLine(axes[zoom->axis_index]->start.x - zoom_offset, zoom->rect_start_y + (zoom_rect_offset_y * (k - zoom->min_index + 1)),
				axes[zoom->axis_index]->start.x, axis_y + (axes[zoom->axis_index]->offset_coord_y * k));
			verdana_zoom_label.drawString(truncated_str,
				axes[zoom->axis_index]->start.x - zoom_width - zoom_offset + 6, 
				zoom->rect_start_y + (zoom_rect_offset_y * (k - zoom->min_index + 1)) + 6);
			verdana_zoom_label.drawString(str,
				15 + 6,
				15 + (zoom_rect_offset_y * (k - zoom->min_index + 1)) + 6);
		}
	}

	ofPopMatrix();
}

void ParallelCoord::update()
{
	if (draw_state == ParallelCoord::DRAW_STATE_INVALIDATE) {
		ofPushMatrix();
		ofTranslate(x + ParallelCoord::OFFSET_X, y + ParallelCoord::OFFSET_Y);

		// Measure title
		float title_str_width = verdana_title.stringWidth(filename);
		float title_str_height = verdana_title.stringHeight(filename);
		title_x = (plot_width / 2.0f) - (title_str_width / 2.0f);
		title_y = title_str_height + ParallelCoord::OFFSET_Y;

		// Measure color config and box plot config
		float color_config_str_h = verdana_config.stringHeight(color_config);
		float color_config_str_w = verdana_config.stringWidth(color_config);
		color_config_x = (plot_width / 2.0f) - color_config_str_w - 50;
		color_config_y = title_y + color_config_str_h + ParallelCoord::OFFSET_Y;
		box_plot_config_x = (plot_width / 2.0f) + 50;
		box_plot_config_y = color_config_y;

		color_config_opt_y = color_config_y + ParallelCoord::OFFSET_Y;
		box_plot_config_opt_y = color_config_opt_y;
		color_config_opt_single_x = (plot_width/2) - ParallelCoord::CONFIG_OFFSET * 3;
		color_config_opt_transition_x = (plot_width / 2) - ParallelCoord::CONFIG_OFFSET * 2;
		color_config_opt_group_x = (plot_width / 2) - ParallelCoord::CONFIG_OFFSET * 1;
		box_plot_config_opt_status_x = box_plot_config_x;

		int grouped_column_index = -1;
		for (int i = 0; i < columns.size(); i++) {
			if (columns[i]->get_type() == Column::TYPE_STRING &&
				columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
				grouped_column_index = i;
				break;
			}
		}

		float deno = (grouped_column_index != -1 && grouped_column_index != 0) ? columns.size() + 2 : columns.size() + 1;
		float offset = plot_width / deno;
		for (int i = 1; i <= columns.size(); i++) {
			int x = (grouped_column_index != -1 && grouped_column_index != 0 && grouped_column_index <= i - 1) ? 
				(i + 1)*offset : i*offset;
			// measure & form axes
			PC_Axis* axis = axes[i - 1];
			axis->start.x = x - axis_width / 2.0f;
			axis->start.y = axis_y;
			axis->offset_coord_y = calculate_offset_coord_y(i - 1);

			// measure & form labels
			float str_width, str_height;
			str_width = verdana_label.stringWidth(columns[i - 1]->get_name());
			str_height = verdana_label.stringHeight(columns[i - 1]->get_name());
			LabelRect *rect = label_rects[i - 1];
			rect->start_point.x = x - (str_width / 2.0f);
			rect->start_point.y = meta_data_height - 30 - ((i % 2) * 30);
			rect->width = str_width;
			rect->height = str_height;

			// measure & form min-max and ticks 
			if (columns[i - 1]->get_type() == Column::TYPE_STRING) {
				// measure & form ticks
				if (columns[i - 1]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP) {
					// iff grouped data
					vector<string> group_names = columns[i - 1]->get_group_names();
					if (axis->ticks.size() == 0) {
						// initialize
						for (int p = 0; p < group_names.size(); p++) {
							PC_Tick* tick = new PC_Tick();
							axis->ticks.push_back(tick);
						}
					}
					if (group_names.size() <= ParallelCoord::MAX_GROUPS) {
						float group_offset = axis_height / (float)(group_names.size() - 1);
						for (int k = 0; k < group_names.size(); k++) {
							str_width = verdana_tick.stringWidth(group_names[k]);
							PC_Tick* tick = axis->ticks[k];
							tick->bg_start.x = x - str_width - 15;
							tick->bg_start.y = axis_y + k * group_offset - 15;
							tick->bg_width = str_width + 10;
							tick->bg_height = 20;

							tick->start.x = x - str_width - 10;
							tick->start.y = axis_y + k * group_offset;
							tick->tick_str = group_names[k].c_str();
						}
					}
				}
			} else {
				// measure & form min-max
				float max, min;
				char *max_str = NULL, *min_str = NULL;
				if (columns[i - 1]->get_type() == Column::TYPE_INT) {
					max_str = new char[20];
					min_str = new char[20];
					max = columns[i - 1]->get_max_i();
					min = columns[i - 1]->get_min_i();
					sprintf(max_str, "%d", columns[i - 1]->get_max_i());
					sprintf(min_str, "%d", columns[i - 1]->get_min_i());
				}
				else if (columns[i - 1]->get_type() == Column::TYPE_FLOAT) {
					max_str = new char[20];
					min_str = new char[20];
					max = columns[i - 1]->get_max_f();
					min = columns[i - 1]->get_min_f();
					sprintf(max_str, "%.2f", columns[i - 1]->get_max_f());
					sprintf(min_str, "%.2f", columns[i - 1]->get_min_f());
				}

				str_width = verdana_label.stringWidth(max_str);
				float max_y = columns[i - 1]->get_draw_order() == Column::MIN_TO_MAX ? axis_y - 10 : axis_y + axis_height + 25;
				axis->max_str = max_str;
				axis->max_str_pt.x = x - (str_width / 2.0f);
				axis->max_str_pt.y = max_y;

				float min_y = columns[i - 1]->get_draw_order() == Column::MAX_TO_MIN ? axis_y - 10 : axis_y + axis_height + 25;
				str_width = verdana_label.stringWidth(min_str);
				axis->min_str = min_str;
				axis->min_str_pt.x = x - (str_width / 2.0f);
				axis->min_str_pt.y = min_y;

				// measure & form box plots
				if (columns[i - 1]->get_type() == Column::TYPE_INT || columns[i - 1]->get_type() == Column::TYPE_FLOAT) {
					ofSetColor(box_plot_color);
					float min_y, max_y;
					min_y = columns[i - 1]->get_draw_order() == Column::MIN_TO_MAX ? axis_y + axis_height : axis_y;
					max_y = columns[i - 1]->get_draw_order() == Column::MIN_TO_MAX ? axis_y : axis_y + axis_height;
					axis->lower_q_y = ofMap(columns[i - 1]->get_lower_quartile(), min, max, min_y, max_y);
					axis->upper_q_y = ofMap(columns[i - 1]->get_upper_quartile(), min, max, min_y, max_y);
					axis->median_y = ofMap(columns[i - 1]->get_median(), min, max, min_y, max_y);
					axis->lower_w_y = ofMap(columns[i - 1]->get_lower_whisker(), min, max, min_y, max_y);
					axis->upper_w_y = ofMap(columns[i - 1]->get_upper_whisker(), min, max, min_y, max_y);
				}

				// measure & form ticks
				float interval = (max - min) / (float)TICK_COUNT;
				float interval_pixels = axis_height / (float)TICK_COUNT;

				if (axis->ticks.size() == 0) {
					// initialize
					for (int p = 0; p < TICK_COUNT - 1; p++) {
						PC_Tick* tick = new PC_Tick();
						axis->ticks.push_back(tick);
					}
				}

				for (int k = 1; k < ParallelCoord::TICK_COUNT; k++) {
					char val_str[20];
					float val = (min + interval * k);
					if (val == floor(val)) {
						snprintf(val_str, 20, "%d", (int)val);
					}
					else {
						snprintf(val_str, 20, "%.2f", val);
					}
					str_width = verdana_tick.stringWidth(val_str);
					int m = columns[i - 1]->get_draw_order() == Column::MIN_TO_MAX ? k : ParallelCoord::TICK_COUNT - k;

					PC_Tick* tick = axis->ticks[k - 1];
					tick->bg_start.x = x - str_width - 15;
					tick->bg_start.y = axis_y + axis_height - m * interval_pixels - 15;
					tick->bg_width = str_width + 10;
					tick->bg_height = 20;

					tick->start.x = x - str_width - 10;
					tick->start.y = axis_y + axis_height - m * interval_pixels;
					tick->tick_str = val_str;
				}
			}

			// measure & form lines
			for (int j = 0; j < column_values[i - 1].size(); j++) {
				if (i - 1 < columns.size() - 1) {
					float a_y;
					float b_y;
					float a_x = x;
					float b_x = (grouped_column_index != -1 && grouped_column_index != 0 && grouped_column_index <= i) ? (i + 2)*offset : (i + 1)*offset;
					PC_Line *line = lines[(i - 1)*column_values[0].size() + j];

					if (grouped_column_index != -1 && color_config_sel == ParallelCoord::COLOR_CONFIG_GROUP) {
						vector<string> group_names = columns[grouped_column_index]->get_group_names();
						string g_name = string();
						column_values[grouped_column_index][j]->get_value(&g_name);
						int k = 0;
						for (k = 0; k < group_names.size(); k++) {
							if (g_name.compare(group_names[k]) == 0) {
								break;
							}
						}
						if (k > group_names.size() - 1) {
							k = group_names.size() - 1;
						}
						line->color = group_color[k];
					}
					else if(color_config_sel == ParallelCoord::COLOR_CONFIG_TRANSITION){
						line->color = transition_color[j];
					}
					else {
						line->color = line_color;
					}

					a_y = translate_val_to_coord(i - 1, j);
					b_y = translate_val_to_coord(i, j);

					line->start.x = a_x;
					line->start.y = axis_y + axis_height - a_y;
					line->end.x = b_x;
					line->end.y = axis_y + axis_height - b_y;
				}
			}
		}

		draw_state = ParallelCoord::DRAW_STATE_STABLE;

		ofPopMatrix();
	}
}

void ParallelCoord::reset()
{
	data_set_mask.clear();
	brushed_mask = 0;
	lines.clear();
	axes.clear();
	mouse_mode = ParallelCoord::MOUSE_MODE_INVALID;
	draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
	x = y = 0;
	width = height = plot_width = plot_height = 0;
	meta_data_height = meta_data_width = axis_width = axis_height = 0;
	axis_y = 0;
	click_label_index = -1;
	click_axis_index = -1;
	drop_label_index = -1;
	label_rects.clear();
	group_color.clear();
	transition_color.clear();
	color_config_sel = ParallelCoord::COLOR_CONFIG_SINGLE;
	box_plot_sel = false;
}

void ParallelCoord::mouseMoved(ofMouseEventArgs & args)
{
	int zoom_index = -1;
	for (int i = 0; i < axes.size(); i++) {
		if (columns[i]->get_type() == Column::TYPE_STRING) {
			if (columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_UNIQUE || 
				(columns[i]->get_string_subtype() == Column::SUBTYPE_STRING_GROUP && 
					columns[i]->get_group_names().size() > ParallelCoord::MAX_GROUPS)) {
				if (is_inside_rect(args.x - x - OFFSET_X, args.y - y - OFFSET_Y,
					axes[i]->start.x - 20,
					axes[i]->start.y,
					axes[i]->start.x + axis_width + 20 * 2,
					axes[i]->start.y + axis_height)) {
					zoom_index = i;
					break;
				}
			}
		}
	}

	if (zoom_index != -1) {
		// set zoom parameters
		mouse_mode = ParallelCoord::MOUSE_MODE_ZOOM;
		zoom->axis_index = zoom_index;
		float start_y = (args.y - y - OFFSET_Y - axis_y) - (axes[zoom_index]->offset_coord_y * 
			ParallelCoord::ZOOM_ENTRY_COUNT / 2.0f);
		float end_y = (args.y - y - OFFSET_Y - axis_y) + (axes[zoom_index]->offset_coord_y *
			ParallelCoord::ZOOM_ENTRY_COUNT / 2.0f);
		float offset = 0.0f;
		if (start_y < 0) {
			offset = start_y;
			start_y = axis_y;
		}
		if (offset > 0) {
			end_y += offset;
		}
		else {
			if (end_y > axis_height) {
				offset = end_y - axis_height;
				end_y = axis_height;
			}

			if (offset > 0) {
				start_y -= offset;
			}
		}
		float rect_start_y = axis_y + start_y - (zoom_height / 2.0f);
		if (rect_start_y < axis_y) {
			rect_start_y = axis_y;
		} else if ((rect_start_y + zoom_height) > (axis_y + axis_height)) {
			rect_start_y = (axis_y + axis_height) - zoom_height;
		}
		int min_index, max_index;
		translate_range_coords_to_index(zoom_index, start_y, end_y, &min_index, &max_index);

		zoom->min_index = min_index;
		zoom->max_index = max_index;
		zoom->rect_start_y = rect_start_y;
	}
	else {
		// reset zoom parameters if active
		if (mouse_mode == ParallelCoord::MOUSE_MODE_ZOOM) {
			mouse_mode = ParallelCoord::MOUSE_MODE_INVALID;
			zoom->axis_index = -1;
			zoom->min_index = -1;
			zoom->max_index = -1;
		}
	}
}

void ParallelCoord::mouseDragged(ofMouseEventArgs & args)
{
	if (mouse_mode == ParallelCoord::MOUSE_MODE_LABEL_CLICK) {
		mouse_mode = ParallelCoord::MOUSE_MODE_LABEL_DRAG;
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_AXIS_CLICK) {
		mouse_mode = ParallelCoord::MOUSE_MODE_AXIS_BRUSH;
	}

	if (mouse_mode == ParallelCoord::MOUSE_MODE_LABEL_DRAG) {
		mouse_drag_pos.x = args.x;
		mouse_drag_pos.y = args.y;
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_AXIS_BRUSH) {
		float brush_y = args.y - y - OFFSET_Y;
		if (brush_y < axis_y) {
			brush_y = axis_y;
		}
		else if (brush_y > axis_y + axis_height) {
			brush_y = axis_y + axis_height;
		}
		axes[click_axis_index]->brush_end_y = brush_y;
		brush_axis();
	}
}

void ParallelCoord::mousePressed(ofMouseEventArgs & args)
{
	// label click?
	int label_index = -1;
	for (int i = 0; i < label_rects.size(); i++) {
		if (is_inside_rect(args.x - x - OFFSET_X, args.y - y - OFFSET_Y,
			label_rects[i]->start_point.x,
			label_rects[i]->start_point.y - label_rects[i]->height,
			label_rects[i]->start_point.x + label_rects[i]->width,
			label_rects[i]->start_point.y)) {
			label_index = i;
			break;
		}
	}

	if (label_index != -1) {
		mouse_mode = ParallelCoord::MOUSE_MODE_LABEL_CLICK;
		click_label_index = label_index;
		return;
	}

	// axis click?
	int axis_index = -1;
	for (int i = 0; i < axes.size(); i++) {
		if (is_inside_rect(args.x - x - OFFSET_X, args.y - y - OFFSET_Y,
			axes[i]->start.x - 20,
			axes[i]->start.y,
			axes[i]->start.x + axis_width + 20*2,
			axes[i]->start.y + axis_height)) {
			if (columns[i]->get_type() == Column::TYPE_FLOAT || 
				columns[i]->get_type() == Column::TYPE_INT) {
				axis_index = i;
				break;
			}
		}
	}

	if (axis_index != -1) {
		mouse_mode = ParallelCoord::MOUSE_MODE_AXIS_CLICK;
		click_axis_index = axis_index;
		axes[axis_index]->brush_start_y = args.y - y - OFFSET_Y;
		return;
	}

	float x_pos = args.x -x - ParallelCoord::OFFSET_X;
	float y_pos = args.y - y - ParallelCoord::OFFSET_Y;

	if (x_pos >= color_config_opt_single_x && x_pos <= color_config_opt_single_x + ParallelCoord::CHECK_MARK_SIZE && 
		y_pos >= color_config_opt_y - ParallelCoord::OFFSET_Y && y_pos <= color_config_opt_y - ParallelCoord::OFFSET_Y + ParallelCoord::CHECK_MARK_SIZE) {
		color_config_sel = ParallelCoord::COLOR_CONFIG_SINGLE;
		draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
		return;
	}

	if (x_pos >= color_config_opt_transition_x && x_pos <= color_config_opt_transition_x + ParallelCoord::CHECK_MARK_SIZE &&
		y_pos >= color_config_opt_y - ParallelCoord::OFFSET_Y && y_pos <= color_config_opt_y - ParallelCoord::OFFSET_Y + ParallelCoord::CHECK_MARK_SIZE) {
		color_config_sel = ParallelCoord::COLOR_CONFIG_TRANSITION;
		draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
		return;
	}

	if (x_pos >= color_config_opt_group_x && x_pos <= color_config_opt_group_x + ParallelCoord::CHECK_MARK_SIZE &&
		y_pos >= color_config_opt_y - ParallelCoord::OFFSET_Y && y_pos <= color_config_opt_y - ParallelCoord::OFFSET_Y + ParallelCoord::CHECK_MARK_SIZE) {
		color_config_sel = ParallelCoord::COLOR_CONFIG_GROUP;
		draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
		return;
	}

	if (x_pos >= box_plot_config_opt_status_x && x_pos <= box_plot_config_opt_status_x + ParallelCoord::CHECK_MARK_SIZE &&
		y_pos >= color_config_opt_y - ParallelCoord::OFFSET_Y && y_pos <= color_config_opt_y - ParallelCoord::OFFSET_Y + ParallelCoord::CHECK_MARK_SIZE) {
		box_plot_sel = !box_plot_sel;
		return;
	}
}

void ParallelCoord::mouseReleased(ofMouseEventArgs & args)
{
	if (mouse_mode == ParallelCoord::MOUSE_MODE_LABEL_DRAG) {
		int label_index = -1;
		for (int i = 0; i < label_rects.size(); i++) {
			if (is_inside_rect(args.x - x - OFFSET_X, args.y - y - OFFSET_Y,
				label_rects[i]->start_point.x,
				label_rects[i]->start_point.y - label_rects[i]->height,
				label_rects[i]->start_point.x + label_rects[i]->width,
				label_rects[i]->start_point.y)) {
				label_index = i;
				break;
			}
		}

		if (label_index != -1) {
			// do reorder axes
			drop_label_index = label_index;
			if (drop_label_index < click_label_index) {
				Column* temp_col = columns[click_label_index];
				vector<ColumnValue*> temp_col_vals = column_values[click_label_index];
				int k = click_label_index;
				while (k > drop_label_index) {
					columns[k] = columns[k - 1];
					column_values[k] = column_values[k - 1];
					k--;
				}
				columns[drop_label_index] = temp_col;
				column_values[drop_label_index] = temp_col_vals;
				draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
			}
			else if (drop_label_index > click_label_index) {
				Column* temp = columns[click_label_index];
				vector<ColumnValue*> temp_col_vals = column_values[click_label_index];
				int k = click_label_index;
				while (k < drop_label_index) {
					columns[k] = columns[k + 1];
					column_values[k] = column_values[k + 1];
					k++;
				}
				columns[drop_label_index] = temp;
				column_values[drop_label_index] = temp_col_vals;
				draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
			}
		}
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_LABEL_CLICK) {
		// invert axis
		columns[click_label_index]->set_draw_order(
			columns[click_label_index]->get_draw_order() == Column::MIN_TO_MAX ?
			Column::MAX_TO_MIN : Column::MIN_TO_MAX);

		draw_state = ParallelCoord::DRAW_STATE_INVALIDATE;
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_AXIS_BRUSH) {
		// brush data set
		float brush_y = args.y - y - OFFSET_Y;
		if (brush_y < axis_y) {
			brush_y = axis_y;
		}
		else if (brush_y > axis_y + axis_height) {
			brush_y = axis_y + axis_height;
		}
		axes[click_axis_index]->brush_end_y = brush_y;

		brush_axis();
	}
	else if (mouse_mode == ParallelCoord::MOUSE_MODE_AXIS_CLICK) {
		for (int k = 0; k < column_values[click_axis_index].size(); k++) {
			data_set_mask[k] |= 1u << click_axis_index;
		}

		axes[click_axis_index]->brush_start_y = -1;
		axes[click_axis_index]->brush_end_y = -1;
	}

	// reset mouse mode
	mouse_mode = ParallelCoord::MOUSE_MODE_INVALID;
	click_label_index = -1;
	drop_label_index = -1;
	click_axis_index = -1;
}

void ParallelCoord::mouseScrolled(ofMouseEventArgs & args)
{
}

void ParallelCoord::mouseEntered(ofMouseEventArgs & args)
{
}

void ParallelCoord::mouseExited(ofMouseEventArgs & args)
{
}
