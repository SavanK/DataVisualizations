#ifndef OFAPP_H
#define OFAPP_H
#include"ofMain.h"
#include"VectorField.h"

using namespace std;

class ofApp : public ofBaseApp
{
private:
    static const int DRAW_STATE_INVALIDATE = 0;
    static const int DRAW_STATE_STABLE = 1;

    static const int SUBSECTION_INACTIVE = 0;
    static const int SUBSECTION_ACTIVE = 1;

    static const int SLIDER_LENGTH = 600;
    static const int SLIDER_WIDTH = 3;
    static const int SLIDER_THUMB_RADIUS = 9;

    static const int CHECK_BOX_SIZE = 40;

    static const int LEGEND_WIDTH = 40;
    static const int LEGEND_HEIGHT = 600;

    static const int MAX_ZOOM_FACTOR = 2;

    static const int MOUSE_MODE_INACTIVE = -1;
    static const int MOUSE_MODE_CLICK = 0;
    static const int MOUSE_MODE_DRAG = 1;

    static const int FEATURE_PAN = 0;
    static const int FEATURE_SEED_PICKER = 1;
    static const int FEATURE_ZOOM_SLIDER = 2;
    static const int FEATURE_TD_SLIDER = 3;
    static const int FEATURE_ITER_SLIDER = 4;
    static const int FEATURE_NONE = -1;

    static const int COLOR_SPACE_COLOR = 0;
    static const int COLOR_SPACE_GRAYSCALE = 1;

    static const int ZOOM_TICK_COUNT = 4;
    static const int MAGNITUDE_TICK_COUNT = 6;

    static const int RESET_BUTTON_SIZE = 30;

    int drawState;
    int mouseMode;
    int mouseFeature;
    ofPoint mouseClick;
    float savedSubsecXpos, savedSubsecYpos;
    int colorSpace;
    ColorSpace divergingCS;

    float screenHeight, screenWidth;

    string filename;
    VectorField *vectorField;
    int* dims;
    float* imageData;
    ofFloatImage image;
    float imageXPos, imageYPos;
    float imageDrawXPos, imageDrawYPos;
    float imageWidth, imageHeight;
    float imageMaxWidth, imageMaxHeight;
    float imageDrawWidth, imageDrawHeight;
    float imageSubsectionXPos, imageSubsectionYPos;
    vector<ofPolyline> streamLines;

    float zoomSliderXPos, zoomSliderYPos;
    float zoomFactor;
    string zoomLabel;

    float tdSliderXPos, tdSliderYPos;
    float tdFactor;
    string tdLabel;

    float iterSliderXPos, iterSliderYPos;
    int iterFactor;
    string iterLabel;

    float minTd, maxTd;
    int minIter, maxIter;
    float defaultTd;
    int defaultIter;

    float informationX, informationY;

    float legendX, legendY;
    ofFloatImage legend;
    float* legendData;
    string legendLabel;

    ofTrueTypeFont verdanaTickLabel;
    int tickLabelFontSize;
    ofColor tickLabelColor;

    float csCheckboxX, csCheckboxY;
    ofImage checkbox, checkmark;
    string csCheckboxLabel;
    ofTrueTypeFont verdana;
    int fontSize;

    float resetX, resetY;
    string resetLabel;

    ofColor sliderColor;
    ofColor sliderThumbColor;
    int sliderLabelFontSize;
    ofColor sliderLabelColor;
    ofTrueTypeFont verdanaSliderLabel;
    ofColor labelColor;

    int subsectionStatus;

    bool isInsideRect(float x, float y,
        float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end);
    bool loadFile(string filepath);
    void refreshLegend();

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
