#pragma once

#include "ofMain.h"
#include "TableReader.h"
#include "ParallelCoord.h"

class ofApp : public ofBaseApp{
	private:
		static const int BUTTON_OFFSET = 10;
		static const int BUTTON_Y = 50;

		bool file_loaded;
		TableReader *table_reader;
		ParallelCoord *parallel_coord;

		int screen_height;
		int screen_width;
		float parallel_coord_x, parallel_coord_y;
		ofColor button_label_color;
		ofColor button_bg_color;
		ofTrueTypeFont verdana_button_label;
		int button_label_font_size;
		string button_label;
		ofPoint button_rect_start;
		float button_width;
		float button_height;

		string *err_code = NULL;

		bool is_inside_rect(float x, float y,
			float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end);
		bool load_file(string filepath);
	public:
		ofApp();

		void setup();
		void update();
		void draw();

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
