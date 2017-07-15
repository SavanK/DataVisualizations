#include "ofApp.h"

ofApp::ofApp()
{
	table_reader = new TableReader();
}

void ofApp::set_file_choice(int choice)
{
	file_choice = choice;
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFullscreen(true);
	ofBackground(196, 196, 196);
	ofEnableSmoothing();

	int num_cols;
	switch (file_choice)
	{
	case 1:
		table_reader->read_data("bin/data/iris.tsv");
		num_cols = table_reader->get_column_count();
		break;

	case 2:
		table_reader->read_data("bin/data/auto-mpg.tsv");
		num_cols = table_reader->get_column_count();
		break;

	case 3:
		table_reader->read_data("bin/data/abalone.tsv");
		num_cols = table_reader->get_column_count();
		break;

	case 4:
		table_reader->read_data("bin/data/winequality-white.tsv");
		num_cols = table_reader->get_column_count();
		break;

	default:
		table_reader->read_data("bin/data/iris.tsv");
		num_cols = table_reader->get_column_count();
		break;
	}
	
	// 0.1 offset between the scatterplots and 0.15 offset within the scatterplot
	scatter_plot_height = ofGetWindowHeight() / (float)(1.25f*num_cols + 0.1f);
	scatter_plot_width = scatter_plot_height;
	offset_x = 0.1 * scatter_plot_width;
	offset_y = 0.1 * scatter_plot_height;
	step_count = scatter_plot_height > 200.0f ? 3 : 2;

	int k = 0;
	for (int i = 0; i < num_cols; i++) {
		for (int j = num_cols - 1; j >= 0; j--) {
			k++;
			Scatterplot *scatterplot = new Scatterplot();
			scatterplot->set_slot_index(k);
			scatterplot->set_data(table_reader->get_column_name(i), table_reader->get_column_name(j),
				table_reader->get_column(i), table_reader->get_column(j));
			scatterplot->setup(scatter_plot_width, scatter_plot_height,
				i*(1.15f * scatter_plot_width) + (i)*offset_x,
				(num_cols - 1 - j)*(1.15f * scatter_plot_height) + (num_cols - j - 1)*offset_y,
				step_count);
			scatter_plots.push_back(scatterplot);
		}
	}

	for (int i = 0; i < scatter_plots.size(); i++) {
		ofAddListener(scatter_plots[i]->brush_on, this, &ofApp::on_brush_data);
		ofAddListener(scatter_plots[i]->brush_mode_changed, this, &ofApp::on_brush_mode);
	}
}

void ofApp::on_brush_data(vector<int> &selected_data_index)
{
	for (int i = 0; i < scatter_plots.size(); i++) {
		scatter_plots[i]->set_selected_index(selected_data_index);
	}
}

void ofApp::on_brush_mode(pair<int, bool> &brush_event)
{
	for (int i = 0; i < scatter_plots.size(); i++) {
		scatter_plots[i]->set_brush_active(brush_event.first, brush_event.second);
	}
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	/* ---- Would be have been good to have, but caused trouble at the last moment
	int num_cols = sqrt(scatter_plots.size());
	float draw_width = num_cols*(1.15f * scatter_plot_width) + (num_cols - 1)*offset_x;
	float x = (ofGetScreenWidth() - draw_width)/2.0f;
	float y = 0;

	ofPushMatrix();
	ofTranslate(x, y);
	*/
	for (int i = 0; i < scatter_plots.size(); i++) {
		scatter_plots[i]->draw();
	}

	/*
	ofPopMatrix();
	*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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
