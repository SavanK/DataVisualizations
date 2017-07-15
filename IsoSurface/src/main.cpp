#include <iostream>
#include "vtkXMLImageDataReader.h"
#include "ofMain.h"
#include "ofApp.h"

using namespace std;

int main(int argc, char *argv[])
{
    ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);			// <-------- setup the GL context
    ofApp *app = new ofApp();
    ofRunApp(app);

    return 0;
}
