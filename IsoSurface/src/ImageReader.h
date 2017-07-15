#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <string>
#include "vtkXMLImageDataReader.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "ofMain.h"

using namespace std;

class ImageReader
{
private:
    string filename;
    vtkSmartPointer<vtkXMLImageDataReader> reader;
    vtkSmartPointer<vtkImageData> imageData;
    int dims[3];
    float* data;
    double range[2];

public:
    ImageReader();
    void setFilename(string fname);
    int *getDims();
    float *getData();
    double *getRange();
};

#endif // IMAGEREADER_H
