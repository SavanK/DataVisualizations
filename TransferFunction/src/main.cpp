#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
    settings.width = 0.4f*1920;
    settings.height = 1080;
	settings.setPosition(ofVec2f(0,0));
	ofCreateWindow(settings);

    shared_ptr<ofApp> ofa(new ofApp);

	ofRunApp(ofa);
}
