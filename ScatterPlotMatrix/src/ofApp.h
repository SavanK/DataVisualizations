#pragma once

#include "ofMain.h"
#include "TableReader.h"
#include "Scatterplot.h"
#include "ofEvents.h"

class ofApp : public ofBaseApp{

	public:
		int scatter_plot_height;
		int scatter_plot_width;
		int offset_x;
		int offset_y;
		int step_count;

		TableReader *table_reader;
		vector<Scatterplot*> scatter_plots;
		int file_choice;

		ofApp();

		void set_file_choice(int choice);

		void setup();
		void update();
		void draw();

		void on_brush_data(vector<int> &selected_data_index);
		void on_brush_mode(pair<int, bool> &brush_event);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
