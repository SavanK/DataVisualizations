#pragma once

#include "ofMain.h"

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "ImageReader.h"

class volumeRenderThread : public ofThread {
private:
    ImageReader* imageReader;

    bool updated;
public:
    void setup(ImageReader* reader);
    void setupRenderer();
    void update();
    void threadedFunction();

    vtkSmartPointer<vtkRenderWindowInteractor> iren;
    vtkSmartPointer<vtkRenderWindow> renWin;
    vtkSmartPointer<vtkOpenGLRenderWindow> oglRenWin;
    vtkSmartPointer<vtkVolumeProperty> volumeProperty;
    vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper;
    vtkSmartPointer<vtkRenderer> ren1;
    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity;
    vtkSmartPointer<vtkColorTransferFunction> color;
    vtkSmartPointer<vtkVolume> volume;
};


