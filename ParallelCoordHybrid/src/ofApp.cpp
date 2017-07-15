#include "ofApp.h"

bool ofApp::is_inside_rect(float x, float y, float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end)
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

bool ofApp::load_file(string filepath)
{
	if (!filepath.empty()) {
		size_t index_last_path_delimiter = filepath.find_last_of('/');
		if (index_last_path_delimiter == string::npos) {
			index_last_path_delimiter = filepath.find_last_of('\\');
		}

		if (index_last_path_delimiter == string::npos) {
			err_code = new string("Bad file path");
			return false;
		}

		filename = filepath.substr(index_last_path_delimiter + 1);
		if (filename.length() > 4) {
			string extension = filename.substr(filename.length() - 4);

			if (extension.compare(".tsv") == 0) {
				// valid file and valid extension
				table_reader->read_data(filepath);

				parallel_coord->set_filename(filename);
				parallel_coord->set_bounds(parallel_coord_x, parallel_coord_y,
					screen_width - 2 * parallel_coord_x, screen_height - parallel_coord_y);
				parallel_coord->set_columns(table_reader->get_columns());
				parallel_coord->set_column_values(table_reader->get_column_values());

				scatterplot->setup(scatter_plot_x, scatter_plot_y, scatter_plot_width, scatter_plot_height, 3);
				scatterplot->set_columns(table_reader->get_columns());
				scatterplot->set_column_values(table_reader->get_column_values());
				scatterplot->set_brush_mask(parallel_coord->get_bursh_mask());
				return true;
			}
			else {
				err_code = new string("Incorrect file extension");
				return false;
			}
		}
		else {
			err_code = new string("Incorrect file extension");
			return false;
		}
	}

	err_code = new string("Empty filepath");
	return false;
}

ofApp::ofApp()
{
	table_reader = new TableReader();
	parallel_coord = new ParallelCoord();
	scatterplot = new Scatterplot();
	file_loaded = false;
}

//--------------------------------------------------------------
void ofApp::setup(){
	//old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);
	button_label_font_size = 42;

	//Loading verdana font
	verdana_button_label.load("verdana.ttf", button_label_font_size, true, true);
	verdana_button_label.setLineHeight(20.0f);
	verdana_button_label.setLetterSpacing(1.2f);

	button_label = string("Load file ...");

	button_bg_color.set(90, 90, 90);
	button_label_color.set(255, 255, 255);

	ofSetFullscreen(true);
	screen_height = ofGetScreenHeight();
	screen_width = ofGetScreenWidth();
	parallel_coord_x = 0;
	parallel_coord_y = 0.35f * screen_height; // give 70% of height for parallel coord to draw
	scatter_plot_height = 0.35f * screen_height;
	scatter_plot_width = 2.5f * scatter_plot_height;
	scatter_plot_x = screen_width - scatter_plot_width;
	scatter_plot_y = 0;
	ofBackground(196, 196, 196);
	ofEnableSmoothing();
}

//--------------------------------------------------------------
void ofApp::update(){
	float str_width = verdana_button_label.stringWidth(button_label);
	float str_height = verdana_button_label.stringHeight(button_label);

	button_rect_start.x = ofApp::BUTTON_X;
	button_rect_start.y = ofApp::BUTTON_Y;
	button_width = str_width + 2 * ofApp::BUTTON_OFFSET;
	button_height = str_height + 2 * ofApp::BUTTON_OFFSET;

	if (file_loaded) {
		parallel_coord->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Draw button background
	ofSetColor(button_bg_color);
	ofDrawRectangle(button_rect_start.x, button_rect_start.y, button_width, button_height);

	// Draw button label
	ofSetColor(button_label_color);
	verdana_button_label.drawString(button_label, 
		button_rect_start.x + ofApp::BUTTON_OFFSET, ofApp::BUTTON_Y + button_height - ofApp::BUTTON_OFFSET);

	if (file_loaded) {
		// Draw title
		ofSetColor(button_bg_color);
		verdana_button_label.drawString(filename,
			button_rect_start.x, button_rect_start.y + button_height * 2 + ofApp::BUTTON_OFFSET);

		parallel_coord->draw();
		scatterplot->draw();
	}
	else if (err_code != NULL && !err_code->empty()) {
		ofSetColor(button_bg_color);
		verdana_button_label.drawString(err_code->c_str(), 
			button_rect_start.x, button_rect_start.y + button_height * 2 + ofApp::BUTTON_OFFSET);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if (is_inside_rect(x, y, button_rect_start.x, button_rect_start.y, button_rect_start.x + button_width, button_rect_start.y + button_height)) {
		button_bg_color.set(60, 60, 60);
	}
	else {
		button_bg_color.set(90, 90, 90);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (is_inside_rect(x, y, button_rect_start.x, button_rect_start.y, button_rect_start.x + button_width, button_rect_start.y + button_height)) {
		ofFileDialogResult result = ofSystemLoadDialog(button_label);
		if (result.bSuccess) {
			string path = result.getPath();
			if (file_loaded) {
				scatterplot->reset();
				parallel_coord->reset();
				table_reader->reset();
			}
			// clear previous err code
			if (err_code != NULL) {
				err_code->clear();
			}
			file_loaded = load_file(path);
		}
		button_bg_color.set(90, 90, 90);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}