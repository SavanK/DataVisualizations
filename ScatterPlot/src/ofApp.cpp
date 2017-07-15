#include "ofApp.h"

ofApp::ofApp()
{
	table_reader = new TableReader();
	scatterplot = new Scatterplot();
}

void ofApp::set_file_choice(int choice)
{
	file_choice = choice;
}

//--------------------------------------------------------------
void ofApp::setup(){
	switch (file_choice)
	{
	case 1:
		table_reader->read_data("bin/data/iris.tsv");
		scatterplot->set_data(table_reader->column_names[3], table_reader->column_names[2],
			table_reader->column_values[3], table_reader->column_values[2]);
		break;

	case 2:
		table_reader->read_data("bin/data/auto-mpg.tsv");
		scatterplot->set_data(table_reader->column_names[1], table_reader->column_names[4],
			table_reader->column_values[1], table_reader->column_values[4]);
		break;

	case 3:
		table_reader->read_data("bin/data/abalone.tsv");
		scatterplot->set_data(table_reader->column_names[0], table_reader->column_names[2],
			table_reader->column_values[0], table_reader->column_values[2]);
		break;

	case 4:
		table_reader->read_data("bin/data/winequality-white.tsv");
		scatterplot->set_data(table_reader->column_names[10], table_reader->column_names[8],
			table_reader->column_values[10], table_reader->column_values[8]);
		break;

	default:
		table_reader->read_data("bin/data/iris.tsv");
		scatterplot->set_data(table_reader->column_names[3], table_reader->column_names[2],
			table_reader->column_values[3], table_reader->column_values[2]);
		break;
	}
	scatterplot->setup(500, 500, 50, 50, 5);
	
	ofAddListener(scatterplot->hover_on, this, &ofApp::on_hover_data);

	ofSetWindowShape(800, 800);
	ofBackground(196, 196, 196);
	ofEnableSmoothing();
}

void ofApp::on_hover_data(int &data_index)
{

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	scatterplot->draw();
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
