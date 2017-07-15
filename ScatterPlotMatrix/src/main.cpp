#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	while (1) {
		ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context
		ofApp *app = new ofApp();

		cout << "Choose the data file\n";
		cout << "\t1.iris\n";
		cout << "\t2.auto-mpg\n";
		cout << "\t3.abalone\n";
		cout << "\t4.winequality-white\n";
		cout << "\t5.QUIT\n";

		int file_choice;
		cin >> file_choice;

		if (file_choice == 5) {
			exit(0);
		}

		app->set_file_choice(file_choice);

		// this kicks off the running of my app
		// can be OF_WINDOW or OF_FULLSCREEN
		// pass in width and height too:
		ofRunApp(app);
	}
}
