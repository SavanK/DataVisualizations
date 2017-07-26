# DataVisualizations
Designed and implemented various interactive data visualization tools as part of class curriculum.
## Dependency
1. Openframeworks https://github.com/openframeworks/openFrameworks
2. VTK http://www.vtk.org/download/
## Build
```
cd <viz>
mkdir build
cd build
cmake ../CMakeLists.txt
cd ..
make
```
Example:
```
cd ScatterPlot
mkdir build
cd build
cmake ../CMakeLists.txt
cd ..
make
```
## Run
```
cd <viz>
./bin/<viz>
```
Example:
```
cd ScatterPlot
./bin/ScatterPlot
```
## Scatterplot
![Image of Scatterplot](ScatterPlot/Scatterplot.png)
## Scatterplot Matrix
![Image of Scatterplot Matrix](ScatterPlotMatrix/ScatterplotMatrix.png)
## Parallel Coordinates
![Image of Parallel Coordinates](ParallelCoordinates/ParallelCoords.png)
## Hybrid Parallel Coordinates
![Image of Hybrid Parallel Coordinates](ParallelCoordHybrid/ParallelCoordsHybrid.png)
## Slice View
![Image of Slice 1](SliceView/Slice_1.png)
![Image of Slice 2](SliceView/Slice_2.png)
## Isosurface
![Image of Isosurface](IsoSurface/IsoSurface.png)
## Transfer Function
![Image of Transfer Function](TransferFunction/TransferFunction.png)
## Vector Field
![Image of Vector Field](VectorViz/VectorFields.png)
