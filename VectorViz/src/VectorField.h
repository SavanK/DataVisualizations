#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include <string>
#include "vtkXMLImageDataReader.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "ofMain.h"

using namespace std;

typedef struct {
    pair<float, float> xRange;
    pair<float, float> yRange;
} Coords;

typedef struct {
    ofFloatColor startColor;
    ofFloatColor midColor;
    ofFloatColor endColor;
} ColorSpace;

class VectorField
{
private:
    static const int COLOR_SPACE_COLOR = 0;
    static const int COLOR_SPACE_GRAYSCALE = 1;

    string filename;
    vtkSmartPointer<vtkXMLImageDataReader> reader;
    vtkSmartPointer<vtkImageData> imageData;
    int dims[3];
    double origin[3];
    double spacing[3];
    double* rangeMagnitude;
    ofVec2f* data;
    float* screenData;

    vector<ofPolyline> streamLinesWS;

    float screenHeight, screenWidth;
    int maxImageWidth, maxImageHeight;
    int imageWidth, imageHeight;

    Coords worldCoords;
    Coords initialScreenCoords;
    Coords screenCoords;
    Coords dataArrayCoords;

    ColorSpace divergingCS;
    int colorSpace;

    void scaleWithAspectRatio(int width, int height, int* newWidth, int* newHeight);
    ofVec2f interpolate(ofVec2f worldPixel);
    void populateScreenData();
    ofFloatColor getColorFor(float frac);
public:
    VectorField();
    void setFilename(string fname, int colorSpace);
    void setDivergingCS(ColorSpace dCS);
    int *getDims();
    ofVec2f *getData();
    float *getScreenData();
    float getImageWidth();
    float getImageHeight();
    float adjustImageDims(float factor);
    float getImageMaxWidth();
    float getImageMaxHeight();
    void rk4IntegrateStreamLine(ofVec2f start, double dt, int numSteps);
    void changeColorSpace(int colorSpace);
    vector<ofPolyline> getStreamLines(float subSecX, float subSecY);
    double* getMagnitudeRange();
    inline Coords getWorldCoords() {
        return worldCoords;
    }
    inline Coords getScreenCoords() {
        return screenCoords;
    }
    inline Coords getDataArrayCoords() {
        return dataArrayCoords;
    }
    inline void clearStreamlines() {
        streamLinesWS.clear();
    }
};

#endif // IMAGEREADER_H
