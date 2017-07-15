#include "VectorField.h"

VectorField::VectorField(){
    reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
}

void VectorField::scaleWithAspectRatio(int width, int height, int *newWidth, int *newHeight)
{
    float xScale = maxImageWidth / width;
    float yScale = maxImageHeight / height;
    float scale;

    scale = (xScale * height > maxImageHeight) ? yScale : xScale;

    *newWidth = width * scale;
    *newHeight = height * scale;
}

ofVec2f VectorField::interpolate(ofVec2f worldPixel) {
    int xMultiple = (int)floor(worldPixel.x / spacing[0]);
    int yMultiple = (int)floor(worldPixel.y / spacing[1]);

    double x1 = xMultiple * spacing[0];
    double x2 = (xMultiple+1) * spacing[0];
    double y1 = yMultiple * spacing[1];
    double y2 = (yMultiple+1) * spacing[1];

    ofVec2f interpolatedValue;

    if(x2 > worldCoords.xRange.second || y2 > worldCoords.yRange.second) {
        interpolatedValue.x = 0;
        interpolatedValue.y = 0;
    } else {
        ofVec2f q11DataPixel, q12DataPixel, q21DataPixel, q22DataPixel;
        q11DataPixel.x = ofMap(x1, worldCoords.xRange.first, worldCoords.xRange.second, dataArrayCoords.xRange.first, dataArrayCoords.xRange.second);
        q11DataPixel.y = ofMap(y1, worldCoords.yRange.first, worldCoords.yRange.second, dataArrayCoords.yRange.first, dataArrayCoords.yRange.second);

        q12DataPixel.x = ofMap(x1, worldCoords.xRange.first, worldCoords.xRange.second, dataArrayCoords.xRange.first, dataArrayCoords.xRange.second);
        q12DataPixel.y = ofMap(y2, worldCoords.yRange.first, worldCoords.yRange.second, dataArrayCoords.yRange.first, dataArrayCoords.yRange.second);

        q21DataPixel.x = ofMap(x2, worldCoords.xRange.first, worldCoords.xRange.second, dataArrayCoords.xRange.first, dataArrayCoords.xRange.second);
        q21DataPixel.y = ofMap(y1, worldCoords.yRange.first, worldCoords.yRange.second, dataArrayCoords.yRange.first, dataArrayCoords.yRange.second);

        q22DataPixel.x = ofMap(x2, worldCoords.xRange.first, worldCoords.xRange.second, dataArrayCoords.xRange.first, dataArrayCoords.xRange.second);
        q22DataPixel.y = ofMap(y2, worldCoords.yRange.first, worldCoords.yRange.second, dataArrayCoords.yRange.first, dataArrayCoords.yRange.second);

        ofVec2f q11, q12, q21, q22;
        q11 = data[(int)q11DataPixel.y * dims[0] + (int)q11DataPixel.x];
        q12 = data[(int)q12DataPixel.y * dims[0] + (int)q12DataPixel.x];
        q21 = data[(int)q21DataPixel.y * dims[0] + (int)q21DataPixel.x];
        q22 = data[(int)q22DataPixel.y * dims[0] + (int)q22DataPixel.x];

        ofVec2f r1, r2;
        r1 = ((x2-worldPixel.x)/(x2-x1))*q11 + ((worldPixel.x-x1)/(x2-x1))*q21;
        r2 = ((x2-worldPixel.x)/(x2-x1))*q12 + ((worldPixel.x-x1)/(x2-x1))*q22;

        interpolatedValue = ((y2-worldPixel.y)/(y2-y1))*r1 + ((worldPixel.y-y1)/(y2-y1))*r2;
    }
    return interpolatedValue;
}

void VectorField::setFilename(string fname, int colorSpace)
{
    filename = fname;
    reader->SetFileName(filename.c_str());
    reader->Update();
    imageData = reader->GetOutput();

    imageData->GetDimensions(dims);

    // Allocate an array to read the data into
    data = new ofVec2f[dims[0]*dims[1]];

    // Get origin & spacing
    imageData->GetOrigin(origin);
    imageData->GetSpacing(spacing);

    cout << "dims: " << dims[0] << "," << dims[1] << "," << dims[2] << endl;
    cout << "origin: " << origin[0] << "," << origin[1] << "," << origin[2] << endl;
    cout << "spacing: " << spacing[0] << "," << spacing[1] << "," << spacing[2] << endl;

    screenHeight = ofGetScreenHeight();
    screenWidth = ofGetScreenWidth();
    maxImageHeight = 0.7f*screenHeight;
    maxImageWidth = 0.8f*screenWidth;

    cout << "maxImageDims: " << maxImageWidth << "x" << maxImageHeight << endl;

    dataArrayCoords.xRange.first = 0;
    dataArrayCoords.xRange.second = dims[0]-1;
    dataArrayCoords.yRange.first = 0;
    dataArrayCoords.yRange.second = dims[1]-1;

    worldCoords.xRange.first= origin[0];
    worldCoords.xRange.second = origin[0] + (spacing[0]*(dims[0]-1));
    worldCoords.yRange.first = origin[1];
    worldCoords.yRange.second = origin[1] + (spacing[1]*(dims[1]-1));

    scaleWithAspectRatio(dims[0], dims[1], &imageWidth, &imageHeight);
    initialScreenCoords.xRange.first = 0;
    initialScreenCoords.xRange.second = imageWidth;
    initialScreenCoords.yRange.first = 0;
    initialScreenCoords.yRange.second = imageHeight;

    screenCoords.xRange.first = initialScreenCoords.xRange.first;
    screenCoords.xRange.second = initialScreenCoords.xRange.second;
    screenCoords.yRange.first = initialScreenCoords.yRange.first;
    screenCoords.yRange.second = initialScreenCoords.yRange.second;

    cout << "dataArrayCoords: [" << dataArrayCoords.xRange.first << "," << dataArrayCoords.xRange.second <<
            "]x[" << dataArrayCoords.yRange.first << "," << dataArrayCoords.yRange.second << "]" << endl;
    cout << "worldCoords: [" << worldCoords.xRange.first << "," << worldCoords.xRange.second <<
            "]x[" << worldCoords.yRange.first << "," << worldCoords.yRange.second << "]" << endl;
    cout << "initialScreenCoords: [" << initialScreenCoords.xRange.first << "," << initialScreenCoords.xRange.second <<
            "]x[" << initialScreenCoords.yRange.first << "," << initialScreenCoords.yRange.second << "]" << endl;
    cout << "screenCoords: [" << screenCoords.xRange.first << "," << screenCoords.xRange.second <<
            "]x[" << screenCoords.yRange.first << "," << screenCoords.yRange.second << "]" << endl;

    vtkSmartPointer<vtkDataArray> vectors = imageData->GetPointData()->GetVectors();
    double* vec = new double[vectors->GetNumberOfComponents()];
    double* rangeX = new double[2];
    double* rangeY = new double[2];
    rangeMagnitude = new double[2];
    vectors->GetRange(rangeX, 0);
    vectors->GetRange(rangeY, 1);
    vectors->GetRange(rangeMagnitude, -1);

    // Read vectors values into data
    for(int i=0;i<dims[0]*dims[1];i++) {
        vectors->GetTuple(i, vec);
        data[i].x = vec[0];
        data[i].y = vec[1];
        //cout << i << "th vector length:" << data[i].length() << endl;
    }
    delete[] vec;

    cout << "RangeX: [" << rangeX[0] << "," << rangeX[1] << "]" << endl;
    cout << "RangeY: [" << rangeY[0] << "," << rangeY[1] << "]" << endl;
    cout << "RangeMagnitude: [" << rangeMagnitude[0] << "," << rangeMagnitude[1] << "]" << endl;

    this->colorSpace = colorSpace;

    populateScreenData();
}

void VectorField::setDivergingCS(ColorSpace dCS)
{
    divergingCS = dCS;
}

void VectorField::populateScreenData() {
    if(colorSpace == COLOR_SPACE_GRAYSCALE) {
        screenData = new float[(int)screenCoords.xRange.second*(int)screenCoords.yRange.second];
        for(int y=0;y<screenCoords.yRange.second;y++) {
            for(int x=0;x<screenCoords.xRange.second;x++) {
                ofVec2f worldPixel;
                worldPixel.x = ofMap(x, screenCoords.xRange.first, screenCoords.xRange.second, worldCoords.xRange.first, worldCoords.xRange.second);
                worldPixel.y = ofMap(screenCoords.yRange.second - y - 1, screenCoords.yRange.first, screenCoords.yRange.second, worldCoords.yRange.first, worldCoords.yRange.second);

                ofVec2f vector = interpolate(worldPixel);
                screenData[y*(int)screenCoords.xRange.second+x] = (vector.length() - rangeMagnitude[0]) / (rangeMagnitude[1] - rangeMagnitude[0]);
            }
        }
    } else {
        screenData = new float[(int)screenCoords.xRange.second*(int)screenCoords.yRange.second*3];
        for(int y=0;y<screenCoords.yRange.second;y++) {
            for(int x=0;x<screenCoords.xRange.second;x++) {
                ofVec2f worldPixel;
                worldPixel.x = ofMap(x, screenCoords.xRange.first, screenCoords.xRange.second, worldCoords.xRange.first, worldCoords.xRange.second);
                worldPixel.y = ofMap(screenCoords.yRange.second - y - 1, screenCoords.yRange.first, screenCoords.yRange.second, worldCoords.yRange.first, worldCoords.yRange.second);

                ofVec2f vector = interpolate(worldPixel);
                ofFloatColor color = getColorFor((vector.length() - rangeMagnitude[0]) / (rangeMagnitude[1] - rangeMagnitude[0]));
                int offset = (y*(int)screenCoords.xRange.second+x)*3;
                screenData[offset] = color.r;
                screenData[offset+1] = color.g;
                screenData[offset+2] = color.b;
            }
        }
    }
}

ofFloatColor VectorField::getColorFor(float frac)
{
    if(frac > 0.5f) {
        ofFloatColor color;
        color.r = divergingCS.midColor.r + frac*(divergingCS.startColor.r-divergingCS.midColor.r);
        color.g = divergingCS.midColor.g + frac*(divergingCS.startColor.g-divergingCS.midColor.g);
        color.b = divergingCS.midColor.b + frac*(divergingCS.startColor.b-divergingCS.midColor.b);
        return color;
    } else if(frac < 0.5f) {
        ofFloatColor color;
        color.r = divergingCS.endColor.r + (0.5f+frac)*(divergingCS.midColor.r-divergingCS.endColor.r);
        color.g = divergingCS.endColor.g + (0.5f+frac)*(divergingCS.midColor.g-divergingCS.endColor.g);
        color.b = divergingCS.endColor.b + (0.5f+frac)*(divergingCS.midColor.b-divergingCS.endColor.b);
        return color;
    } else {
        return divergingCS.midColor;
    }
}

int *VectorField::getDims()
{
    return dims;
}

ofVec2f *VectorField::getData()
{
    return data;
}

float *VectorField::getScreenData()
{
    return screenData;
}

float VectorField::getImageWidth()
{
    return screenCoords.xRange.second;
}

float VectorField::getImageHeight()
{
    return screenCoords.yRange.second;
}

float VectorField::adjustImageDims(float factor)
{
    screenCoords.xRange.second = (int) (initialScreenCoords.xRange.second +
            factor*(initialScreenCoords.xRange.second - initialScreenCoords.xRange.first));
    screenCoords.yRange.second = (int) (initialScreenCoords.yRange.second +
            factor*(initialScreenCoords.yRange.second - initialScreenCoords.yRange.first));
    populateScreenData();
}

float VectorField::getImageMaxWidth()
{
    return maxImageWidth;
}

float VectorField::getImageMaxHeight()
{
    return maxImageHeight;
}

void VectorField::rk4IntegrateStreamLine(ofVec2f start, double dt, int numSteps)
{
    ofPolyline polyline;
    ofVec2f worldPixel;
    worldPixel.x = ofMap(start.x, screenCoords.xRange.first, screenCoords.xRange.second, worldCoords.xRange.first, worldCoords.xRange.second);
    worldPixel.y = ofMap(screenCoords.yRange.second - start.y - 1, screenCoords.yRange.first, screenCoords.yRange.second, worldCoords.yRange.first, worldCoords.yRange.second);

    polyline.addVertex(worldPixel);

    for(int i=0;i<numSteps;i++) {
        //cout << "\tworldPixel:" << worldPixel.x << "," << worldPixel.y << endl;
        ofVec2f velocity = interpolate(worldPixel);
        //cout << "\tvelocity:" << velocity.x << "," << velocity.y << endl;
        ofVec2f a,b,c,d;
        a = dt*velocity;
        b = dt*(velocity+a/2);
        c = dt*(velocity+b/2);
        d = dt*(velocity+c);

        //cout << "\ta:" << a.x << "," << a.y << endl;
        //cout << "\tb:" << b.x << "," << b.y << endl;
        //cout << "\tc:" << c.x << "," << c.y << endl;
        //cout << "\td:" << d.x << "," << d.y << endl;

        worldPixel += (1/6.0f)*(a + 2*b + 2*c + d);

        polyline.addVertex(worldPixel);
    }
    streamLinesWS.push_back(polyline);
}

void VectorField::changeColorSpace(int colorSpace)
{
    if(colorSpace != this->colorSpace) {
        this->colorSpace = colorSpace;
        populateScreenData();
    }
}

vector<ofPolyline> VectorField::getStreamLines(float subSecX, float subSecY)
{
    vector<ofPolyline> streamLines;
    for(int i=0;i<streamLinesWS.size();i++) {
        ofPolyline line = streamLinesWS[i];
        ofPolyline screenLine;
        for(int j=0;j<line.size();j++) {
            ofVec2f screenPixel;
            screenPixel.x = ofMap(line[j].x, worldCoords.xRange.first, worldCoords.xRange.second, screenCoords.xRange.first, screenCoords.xRange.second);
            screenPixel.y = screenCoords.yRange.second - 1 -
                    ofMap(line[j].y, worldCoords.yRange.first, worldCoords.yRange.second, screenCoords.yRange.first, screenCoords.yRange.second);
            screenPixel.x = screenPixel.x - subSecX;
            screenPixel.y = screenPixel.y - subSecY;
            screenLine.addVertex(screenPixel);
        }
        streamLines.push_back(screenLine);
    }
    return streamLines;
}

double *VectorField::getMagnitudeRange()
{
    return rangeMagnitude;
}
