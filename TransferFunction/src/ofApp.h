#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "volumeRenderThread.h"

typedef struct {
    int dataPoint;
    ofFloatColor color;
} ColorTfValue;

typedef struct {
    ColorTfValue minVal;
    ColorTfValue maxVal;
    vector<ColorTfValue> intermediateVals;
} ColorTfDatapoints;

typedef struct {
    int dataPoint;
    float opacity;
} OpacityTfValue;

typedef struct {
    OpacityTfValue minVal;
    OpacityTfValue maxVal;
    vector<OpacityTfValue> intermediateVals;
} OpacityTfDatapoints;

typedef struct {
    ofFloatColor startColor;
    ofFloatColor midColor;
    ofFloatColor endColor;
} ColorSpace;

class ofApp : public ofBaseApp {
private:
    static const int BUTTON_OFFSET = 10;

    static const int CHECKBOX_SIZE = 40;

    static const int HIST_X_TICK_COUNT = 5;
    static const int HIST_Y_TICK_COUNT = 3;

    static const int SLIDER_HIST_X = 220;
    static const int SLIDER_HIST_Y_FROM_BOTTOM = 40;
    static const int SLIDER_TF_X = 220;
    static const int SLIDER_TF_Y_FROM_TOP = 40;
    static const int SLIDER_THUMB_RADIUS = 9;
    static const int SLIDER_LENGTH = 500;
    static const int SLIDER_WIDTH = 3;
    static const int SLIDER_ACTIVE_WIDTH = 5;

    static const int SLIDER_NO_THUMB = -1;
    static const int HIST_SLIDER_MIN_THUMB = 0;
    static const int HIST_SLIDER_MAX_THUMB = 1;
    static const int TF_SLIDER_MIN_THUMB = 2;
    static const int TF_SLIDER_MAX_THUMB = 3;
    static const int TF_COLOR_THUMB = 4;

    static const int TF_OPACITY_THUMB = 5;

    static const int TF_THUMB_RADIUS = 8;
    static const int TF_THUMB_LINE_WIDTH = 2;

    static const int DRAW_STATE_INVALIDATE = 0;
    static const int DRAW_STATE_STABLE = 1;
    static const int MOUSE_MODE_INACTIVE = -1;
    static const int MOUSE_MODE_CLICK = 0;
    static const int MOUSE_MODE_DRAG = 1;

    ImageReader* imageReader;
    bool startedVTK;
    volumeRenderThread vr;

    int* dims;
    double* range;
    int visibleRange[2];
    int visibleTfRange[2];
    float* data;

    // Histogram data-structure
    vector<pair<int, int>> histogramVals;
    int histogramRange[2];

    // GUI
    int drawState;
    int mouseMode;
    string filename;
    bool histLogScale;
    bool useAboveRange, useBelowRange;

    // title
    ofTrueTypeFont verdanaTitleLabel;
    int titleLabelFontSize;
    ofColor titleLabelColor;
    float titleX, titleY;
    string titleLabel;

    // file load buttons
    ofColor buttonLabelColor;
    ofColor buttonBgColor;
    ofTrueTypeFont verdanaButtonLabel;
    int buttonLabelFontSize;

    // Generic
    ofColor genLabelColor;
    ofTrueTypeFont verdanaGenLabel;
    int genLabelFontSize;
    ofImage checkMark, checkBox;
    ofColor sliderColor;
    ofColor sliderInactiveColor;
    int sliderLabelFontSize;
    ofColor sliderLabelColor;
    ofTrueTypeFont verdanaSliderLabel;
    int thumbClicked;
    ofTrueTypeFont verdanaTickLabel;
    int tickLabelFontSize;
    ofColor tickLabelColor;
    ofColor optionsLabelColor;
    ColorSpace divergingCS;
    ColorSpace hsvCS;
    ColorSpace labCS;
    ColorSpace rgbCS;
    ofColor tfThumbColor;

    // histogram
    float histogramBoxWidth, histogramBoxHeight;
    float histogramWidth, histogramHeight;
    float histogramX, histogramY;
    float histogramLeftOffset, histogramTopOffset, histogramBottomOffset;
    ofVec2f histogramOrigin;
    float histBlockLength;
    string histLogLabel;
    float histLogX, histLogY;
    float histSliderMinXPos, histSliderMaxXPos;
    string histSliderLabel;

    // transfer functions
    float tfBoxWidth, tfBoxHeight;
    float tfX, tfY;
    float tfTopOffset, tfBottomOffset, tfLROffset;
    float tfSliderMinXPos, tfSliderMaxXPos;
    string tfSliderLabel;

    // color tf
    ofFloatImage tfColor;
    ColorTfDatapoints tfColorDataPoints;
    ofVec2f tfColorOrigin;
    int tfColorWidth, tfColorHeight;
    int tfColorClickIndex;
    float resetColorX, resetColorY;
    float resetColorWidth, resetColorHeight;
    string resetColorLabel;

    // opacity tf
    ofFloatImage tfOpacity;
    OpacityTfDatapoints tfOpacityDataPoints;
    ofVec2f tfOpacityOrigin;
    int tfOpacityWidth, tfOpacityHeight;
    int tfOpacityClickIndex;
    float resetOpacityX, resetOpacityY;
    float resetOpacityWidth, resetOpacityHeight;
    string resetOpacityLabel;

    string aboveRangeLabel, belowRangeLabel;
    float aboveRangeX, aboveRangeY, belowRangeX, belowRangeY;

    bool isInsideRect(float x, float y,
        float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end);
    void loadFile(string file);
    void updateColor();
    void updateOpacity();
    void toggleShading(bool& shadeOn);
    void resetColorTf();
    void resetOpacityTf();

public:
    void setup();
    void update();
    void draw();
    void exit();

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
