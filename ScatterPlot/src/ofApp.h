#pragma once

#include "ofMain.h"
#include "TableReader.h"
#include "Scatterplot.h"
#include "ofEvents.h"

class ofApp : public ofBaseApp{

	public:
		TableReader *table_reader;
		Scatterplot *scatterplot;
		int file_choice;

		ofApp();

		void set_file_choice(int choice);

		void setup();
		void update();
		void draw();

		void on_hover_data(int &data_index);

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
