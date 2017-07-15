#include "ImageReader.h"

ImageReader::ImageReader()
{
    reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
}

void ImageReader::setFilename(string fname)
{
    filename = fname;
    reader->SetFileName(filename.c_str());
    reader->Update();
    imageData = reader->GetOutput();

    imageData->GetDimensions(dims);

    //Allocate an array to read the data into
    data = new float[dims[0]*dims[1]*dims[2]];

    vtkSmartPointer<vtkDataArray> scalars = imageData->GetPointData()->GetScalars();

    scalars->GetRange(range);

    int i=0;
    for(int z=0; z<dims[2];z++) {
        for(int y=0;y<dims[1];y++) {
            for(int x=0;x<dims[0];x++) {
                data[i] = (scalars->GetTuple1(i) - range[0]) / (range[1]-range[0]);
                i++;
            }
        }
    }
}

int *ImageReader::getDims()
{
    return dims;
}

float *ImageReader::getData()
{
    return data;
}

double *ImageReader::getRange()
{
    return range;
}
