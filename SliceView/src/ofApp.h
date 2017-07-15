#ifndef OFAPP_H
#define OFAPP_H
#include"ofMain.h"
#include"ImageReader.h"

using namespace std;

class ofApp : public ofBaseApp
{
private:
    static const int AXIS_LENGTH = 300;

    static const int BUTTON_OFFSET = 10;
    static const int BUTTON_X = 20;
    static const int BUTTON_Y = 20;

    static const int SLIDER_X = 20;
    static const int SLIDER_Y_FROM_BOTTOM = 160;
    static const int SLIDER_LENGTH = 600;
    static const int SLIDER_WIDTH = 3;
    static const int SLIDER_THUMB_RADIUS = 9;
    static const int SLICE_BUTTON_WIDTH = 80;
    static const int SLICE_BUTTON_HEIGHT = 40;
    static const int SLICE_BUTTON_X = 20 + (SLIDER_LENGTH - SLICE_BUTTON_WIDTH)/2;
    static const int SLICE_BUTTON_Y_FROM_BOTTOM = 120;
    static const int COLOR_CHECKBOX_X = 20 + (SLIDER_LENGTH - SLICE_BUTTON_WIDTH)/2;
    static const int COLOR_CHECKBOX_Y_FROM_BOTTOM = 220;
    static const int COLOR_CHECKBOX_SIZE = 40;
    static const int COLOR_CHECKBOX_MARGIN = 15;

    static const int HINT_X = 20;
    static const int HINT_Y_FROM_BOTTOM = 310;
    static const int ACTIVE_AXIS_X = 20;
    static const int ACTIVE_AXIS_Y_FROM_BOTTOM = 260;

    static const int PLANE_NONE = -1;
    static const int PLANE_X = 0;
    static const int PLANE_Y = 1;
    static const int PLANE_Z = 2;

    static const int DRAW_STATE_INVALIDATE = 0;
    static const int DRAW_STATE_STABLE = 1;

    static const int MOUSE_MODE_INACTIVE = -1;
    static const int MOUSE_MODE_CLICK = 0;
    static const int MOUSE_MODE_DRAG = 1;

    static const int MODEL_3D_VIEW = 0;
    static const int SLICE_VIEW = 1;

    static const int IMAGE_GRAYSCALE = 0;
    static const int IMAGE_COLOR = 1;

    static const int MAX_RED = 240;
    static const int MAX_GREEN = 59;
    static const int MAX_BLUE = 32;

    static const int MIN_RED = 255;
    static const int MIN_GREEN = 237;
    static const int MIN_BLUE = 160;

    static const int TICK_COUNT = 3;

    int drawState;
    int mouseMode;
    int viewRendered;
    int slicedImageFormat;

    string filename;

    bool file_loaded;
    ofColor buttonLabelColor;
    ofColor buttonBgColor;
    ofTrueTypeFont verdanaButtonLabel;
    int buttonLabelFontSize;

    string fuelLabel;
    ofPoint fuelRectStart;
    float fuelWidth;
    float fuelHeight;

    string engineLabel;
    ofPoint engineRectStart;
    float engineWidth;
    float engineHeight;

    string stagLabel;
    ofPoint stagRectStart;
    float stagWidth;
    float stagHeight;

    string hintLabel;
    string activeAxisLabel;

    int screenHeight;
    int screenWidth;
    float xPos, yPos, zPos;

    ImageReader *imageReader;

    int* dims;
    double* range;
    float* data;

    ofEasyCam cam;
    ofVboMesh vboMesh;
    ofLight light;

    ofFloatColor lineColor = ofFloatColor(0.086f, 0.274f, 0.576f);

    ofPlanePrimitive zPlane;
    ofPlanePrimitive xPlane;
    ofPlanePrimitive yPlane;

    int activePlane;

    ofColor planeColor;
    ofColor activePlaneColor;

    string sliceLabel;
    ofPoint sliceRectStart;
    ofColor sliderColor;
    ofColor sliderInactiveColor;
    float sliderXPos;
    int sliderLabelFontSize;
    ofColor sliderLabelColor;
    ofTrueTypeFont verdanaSliderLabel;

    string redoLabel;

    ofImage checkMark, checkBox;
    string colorChooserLabel;
    ofPoint colorChooserRectStart;

    ofFloatImage slicedImage;
    float slicedImgMaxHeight;
    float slicedImgMaxWidth;

    ofFloatImage legend;
    int legendWidth;
    int legendHeight;
    float legendXFromRight, legendY;
    ofTrueTypeFont verdanaTickLabel;
    int tickLabelFontSize;
    ofColor tickLabelColor;

    bool isInsideRect(float x, float y,
        float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end);
    bool loadFile(string filepath);
    void addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofFloatColor color);
    void setupMesh();
    void clearMesh();
    void prepareSlicedView();
    void scaleWithAspectRatio(float width, float height, float* newWidth, float* newHeight);

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

#endif // OFAPP_H
