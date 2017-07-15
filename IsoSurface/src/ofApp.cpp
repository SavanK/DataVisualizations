#include "ofApp.h"

bool ofApp::isInsideRect(float x, float y, float rec_x_start, float rec_y_start, float rec_x_end, float rec_y_end)
{
    float x_start, x_end, y_start, y_end;

    if (rec_x_start < rec_x_end && rec_y_start > rec_y_end) {
        x_start = rec_x_start;
        x_end = rec_x_end;
        y_start = rec_y_start;
        y_end = rec_y_end;
    }
    else if (rec_x_start > rec_x_end && rec_y_start > rec_y_end) {
        x_start = rec_x_end;
        x_end = rec_x_start;
        y_start = rec_y_start;
        y_end = rec_y_end;
    }
    else if (rec_x_start > rec_x_end && rec_y_start < rec_y_end) {
        x_start = rec_x_end;
        x_end = rec_x_start;
        y_start = rec_y_end;
        y_end = rec_y_start;
    }
    else {
        x_start = rec_x_start;
        x_end = rec_x_end;
        y_start = rec_y_end;
        y_end = rec_y_start;
    }

    return (x >= x_start && x <= x_end) && (y <= y_start && y >= y_end);
}

bool ofApp::loadFile(string filepath)
{
    imageReader->setFilename(filepath);
    setupMesh();
    return true;
}

void ofApp::addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofFloatColor color)
{
    vboMesh.addColor(color);
    ofVec3f normal = ((b - a).cross(c - a)).normalize();
    vboMesh.addNormal(normal);
    vboMesh.addVertex(a);
    vboMesh.addNormal(normal);
    vboMesh.addVertex(b);
    vboMesh.addNormal(normal);
    vboMesh.addVertex(c);
}

void ofApp::setupMesh()
{
    dims = imageReader->getDims();
    range = imageReader->getRange();
    data = imageReader->getData();

    xPos = dims[0]/2.0f;
    yPos = dims[1]/2.0f;
    zPos = dims[2]/2.0f;

    xPlane.set(dims[1], dims[2]);
    yPlane.set(dims[0], dims[2]);
    zPlane.set(dims[0], dims[1]);

    xPlane.enableColors();
    xPlane.setUseVbo(true);
    yPlane.enableColors();
    yPlane.setUseVbo(true);
    zPlane.enableColors();
    zPlane.setUseVbo(true);

    xPlane.rotate(90, 0, 1, 0);
    yPlane.rotate(90, 1, 0, 0);

    vboMesh.setMode(OF_PRIMITIVE_POINTS);
    vboMesh.enableColors();

    int i=0;
    for(int z=0;z<dims[2];z++) {
        for(int y=0;y<dims[1];y++) {
            for(int x=0;x<dims[0];x++) {
                if(data[i] > 0) {
                    ofVec3f point(x,y,z);
                    vboMesh.addVertex(point);
                    int red = (int)(MIN_RED - data[i]*(MIN_RED - MAX_RED));
                    int green = (int)(MIN_GREEN - data[i]*(MIN_GREEN - MAX_GREEN));
                    int blue = (int)(MIN_BLUE - data[i]*(MIN_BLUE - MAX_BLUE));
                    ofColor color;
                    color.set(red, green, blue);
                    vboMesh.addColor(color);
                }
                i++;
            }
        }
    }

    sliderXPos = SLIDER_X;

    drawState = DRAW_STATE_INVALIDATE;
}

void ofApp::clearMesh()
{
    dims[0] = dims[1] = dims[2] = -1;
    xPos = yPos = 0;
    vboMesh.clear();
    cam.reset();
    xPlane.rotate(-90, 0, 1, 0);
    yPlane.rotate(-90, 1, 0, 0);
    viewRendered = MODEL_3D_VIEW;
    activePlane = PLANE_NONE;
}

void ofApp::prepareSlicedView()
{
    float *imageData;
    int i=0, k=0;
    int x=0, y=0, z=0;
    int offset = 0;
    float newWidth, newHeight;
    int size = 0;

    switch (activePlane) {
    case PLANE_Z:
        size = slicedImageFormat == IMAGE_COLOR ? dims[0]*dims[1]*3 : dims[0]*dims[1];
        imageData = new float[size];

        z = (int)ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[2]-1);
        offset = (z*dims[1]*dims[0]);
        for(y=0;y<dims[1];y++) {
            for(x=0;x<dims[0];x++) {
                if(slicedImageFormat == IMAGE_COLOR) {
                    imageData[i] = (MIN_RED - data[offset+k]*(MIN_RED - MAX_RED))/255.0f;
                    imageData[i+1] = (MIN_GREEN - data[offset+k]*(MIN_GREEN - MAX_GREEN))/255.0f;
                    imageData[i+2] = (MIN_BLUE - data[offset+k]*(MIN_BLUE - MAX_BLUE))/255.0f;
                    i+=3;
                    k++;
                } else {
                    imageData[i] = 1-data[offset+i];
                    i++;
                }
            }
        }
        if(slicedImageFormat == IMAGE_COLOR) {
            slicedImage.setFromPixels(imageData, dims[0], dims[1], OF_IMAGE_COLOR);
            scaleWithAspectRatio(dims[0], dims[1], &newWidth, &newHeight);
        } else {
            slicedImage.setFromPixels(imageData, dims[0], dims[1], OF_IMAGE_GRAYSCALE);
            scaleWithAspectRatio(dims[0], dims[1], &newWidth, &newHeight);
        }
        break;

    case PLANE_Y:
        size = slicedImageFormat == IMAGE_COLOR ? dims[0]*dims[2]*3 : dims[0]*dims[2];
        imageData = new float[size];

        y = (int)ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[1]-1);
        for(z=0;z<dims[2];z++) {
            offset = z*dims[1]*dims[0] + y*dims[0];
            for(x=0;x<dims[0];x++) {
                if(slicedImageFormat == IMAGE_COLOR) {
                    imageData[i] = (MIN_RED - data[offset+x]*(MIN_RED - MAX_RED))/255.0f;
                    imageData[i+1] = (MIN_GREEN - data[offset+x]*(MIN_GREEN - MAX_GREEN))/255.0f;
                    imageData[i+2] = (MIN_BLUE - data[offset+x]*(MIN_BLUE - MAX_BLUE))/255.0f;
                    i+=3;
                    k++;
                } else {
                    imageData[i] = 1-data[offset+x];
                    i++;
                }
            }
        }
        if(slicedImageFormat == IMAGE_COLOR) {
            slicedImage.setFromPixels(imageData, dims[0], dims[2], OF_IMAGE_COLOR);
            scaleWithAspectRatio(dims[0], dims[2], &newWidth, &newHeight);
        } else {
            slicedImage.setFromPixels(imageData, dims[0], dims[2], OF_IMAGE_GRAYSCALE);
            scaleWithAspectRatio(dims[0], dims[2], &newWidth, &newHeight);
        }
        break;

    case PLANE_X:
        size = slicedImageFormat == IMAGE_COLOR ? dims[1]*dims[2]*3 : dims[1]*dims[2];
        imageData = new float[size];

        x = (int)ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[0]-1);
        for(z=0;z<dims[2];z++) {
            for(y=0;y<dims[1];y++) {
                offset = z*dims[1]*dims[0] + y*dims[0] + x;
                if(slicedImageFormat == IMAGE_COLOR) {
                    imageData[i] = (MIN_RED - data[offset]*(MIN_RED - MAX_RED))/255.0f;
                    imageData[i+1] = (MIN_GREEN - data[offset]*(MIN_GREEN - MAX_GREEN))/255.0f;
                    imageData[i+2] = (MIN_BLUE - data[offset]*(MIN_BLUE - MAX_BLUE))/255.0f;
                    i+=3;
                    k++;
                } else {
                    imageData[i] = 1-data[offset];
                    i++;
                }
            }
        }
        if(slicedImageFormat == IMAGE_COLOR) {
            slicedImage.setFromPixels(imageData, dims[1], dims[2], OF_IMAGE_COLOR);
            scaleWithAspectRatio(dims[1], dims[2], &newWidth, &newHeight);
        } else {
            slicedImage.setFromPixels(imageData, dims[1], dims[2], OF_IMAGE_GRAYSCALE);
            scaleWithAspectRatio(dims[1], dims[2], &newWidth, &newHeight);
        }
        break;

    default:
        size = slicedImageFormat == IMAGE_COLOR ? dims[0]*dims[1]*3 : dims[0]*dims[1];
        imageData = new float[size];

        z = (int)ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[2]-1);
        offset = (z*dims[1]*dims[0]);
        for(y=0;y<dims[1];y++) {
            for(x=0;x<dims[0];x++) {
                if(slicedImageFormat == IMAGE_COLOR) {
                    imageData[i] = (MIN_RED - data[offset+k]*(MIN_RED - MAX_RED))/255.0f;
                    imageData[i+1] = (MIN_GREEN - data[offset+k]*(MIN_GREEN - MAX_GREEN))/255.0f;
                    imageData[i+2] = (MIN_BLUE - data[offset+k]*(MIN_BLUE - MAX_BLUE))/255.0f;
                    i+=3;
                    k++;
                } else {
                    imageData[i] = 1-data[offset+i];
                    i++;
                }
            }
        }
        if(slicedImageFormat == IMAGE_COLOR) {
            slicedImage.setFromPixels(imageData, dims[0], dims[1], OF_IMAGE_COLOR);
            scaleWithAspectRatio(dims[0], dims[1], &newWidth, &newHeight);
        } else {
            slicedImage.setFromPixels(imageData, dims[0], dims[1], OF_IMAGE_GRAYSCALE);
            scaleWithAspectRatio(dims[0], dims[1], &newWidth, &newHeight);
        }
        break;
    }
    slicedImage.resize(newWidth, newHeight);

    size = slicedImageFormat == IMAGE_COLOR ? legendWidth*legendHeight*3 : legendWidth*legendHeight;
    float *legendData = new float[size];
    k=0;
    for(int i=0;i<legendHeight;i++) {
        for(int j=0;j<legendWidth;j++) {
            if(slicedImageFormat == IMAGE_COLOR) {
                legendData[k] = (MIN_RED - (1 - i/(float)legendHeight)*(MIN_RED - MAX_RED))/255.0f;
                legendData[k+1] = (MIN_GREEN - (1 - i/(float)legendHeight)*(MIN_GREEN - MAX_GREEN))/255.0f;
                legendData[k+2] = (MIN_BLUE - (1 - i/(float)legendHeight)*(MIN_BLUE - MAX_BLUE))/255.0f;
                k+=3;
            } else {
                legendData[k] = (i/(float)legendHeight);
                k++;
            }
        }
    }
    if(slicedImageFormat == IMAGE_COLOR) {
        legend.setFromPixels(legendData, legendWidth, legendHeight, OF_IMAGE_COLOR);
    } else {
        legend.setFromPixels(legendData, legendWidth, legendHeight, OF_IMAGE_GRAYSCALE);
    }
}

void ofApp::scaleWithAspectRatio(float width, float height, float *newWidth, float *newHeight)
{
    float xScale = slicedImgMaxWidth / width;
    float yScale = slicedImgMaxHeight / height;
    float scale;

    scale = (xScale * height > slicedImgMaxHeight) ? yScale : xScale;

    *newWidth = width * scale;
    *newHeight = height * scale;
}

ofApp::ofApp()
{
    imageReader = new ImageReader();
    file_loaded = false;
    activePlane = PLANE_NONE;
    mouseMode = MOUSE_MODE_INACTIVE;
    sliderXPos = SLIDER_X;
    activePlane = PLANE_NONE;
    viewRendered = MODEL_3D_VIEW;
    slicedImageFormat = IMAGE_COLOR;
    slicedImage.clear();
    legend.clear();
}

void ofApp::setup()
{
    //old OF default is 96 - but this results in fonts looking larger than in other programs.
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(0);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetFullscreen(true);

    screenHeight = ofGetScreenHeight();
    screenWidth = ofGetScreenWidth();

    slicedImgMaxHeight = 0.6f*screenHeight;
    slicedImgMaxWidth = 0.8f*screenWidth;

    ofBackground(180, 180, 180);

    buttonLabelFontSize = 22;

    verdanaButtonLabel.load("verdana.ttf", buttonLabelFontSize, true, true);
    verdanaButtonLabel.setLineHeight(20.0f);
    verdanaButtonLabel.setLetterSpacing(1.2f);

    tickLabelFontSize = 14;

    verdanaTickLabel.load("verdana.ttf", tickLabelFontSize, true, true);
    verdanaTickLabel.setLineHeight(20.0f);
    verdanaTickLabel.setLetterSpacing(1.2f);

    sliderLabelFontSize = 18;

    verdanaSliderLabel.load("verdana.ttf", sliderLabelFontSize, true, true);
    verdanaSliderLabel.setLineHeight(20.0f);
    verdanaSliderLabel.setLetterSpacing(1.2f);

    fuelLabel = string("Fuel");
    engineLabel = string("Engine");
    stagLabel = string("Stag");
    sliceLabel = string("Slice");
    redoLabel = string("Redo");
    colorChooserLabel = string("Colored");
    hintLabel = string("Use keyboard to activate slice axis by pressing 'x' or 'y' or 'z'\n'r' to reset");
    activeAxisLabel = string("Active plane: ");

    checkBox.load("checkbox.png");
    checkMark.load("checkmark.png");

    buttonBgColor.set(66, 134, 244);
    buttonLabelColor.set(255, 255, 255);

    tickLabelColor.set(255, 255, 255);

    planeColor.set(90, 90, 90, 140);
    activePlaneColor.set(244, 66, 182, 100);
    sliderColor.set(244, 66, 182);
    sliderLabelColor.set(244, 66, 182);
    sliderInactiveColor.set(90, 90, 90);

    legendWidth = 20;
    legendHeight = (int)(0.6f*screenHeight);
    legendXFromRight = 0.08f*screenWidth;
    legendY = 0.2f*screenHeight;

    sliderXPos = SLIDER_X;

    drawState = DRAW_STATE_INVALIDATE;
}

void ofApp::update()
{
    if(drawState == DRAW_STATE_INVALIDATE) {
        float str_width = verdanaButtonLabel.stringWidth(fuelLabel);
        float str_height = verdanaButtonLabel.stringHeight(fuelLabel);

        fuelRectStart.x = BUTTON_X;
        fuelRectStart.y = BUTTON_Y;
        fuelWidth = str_width + 2 * BUTTON_OFFSET;
        fuelHeight = str_height + 2 * BUTTON_OFFSET;

        str_width = verdanaButtonLabel.stringWidth(engineLabel);

        engineRectStart.x = BUTTON_X + fuelRectStart.x + fuelWidth + 2 * BUTTON_OFFSET;
        engineRectStart.y = BUTTON_Y;
        engineWidth = str_width + 2 * BUTTON_OFFSET;
        engineHeight = str_height + 2 * BUTTON_OFFSET;

        str_width = verdanaButtonLabel.stringWidth(stagLabel);

        stagRectStart.x = BUTTON_X + engineRectStart.x + engineWidth + 2 * BUTTON_OFFSET;
        stagRectStart.y = BUTTON_Y;
        stagWidth = str_width + 2 * BUTTON_OFFSET;
        stagHeight = str_height + 2 * BUTTON_OFFSET;

        sliceRectStart.x = SLICE_BUTTON_X;
        sliceRectStart.y = screenHeight - SLICE_BUTTON_Y_FROM_BOTTOM;

        colorChooserRectStart.x = COLOR_CHECKBOX_X;
        colorChooserRectStart.y = screenHeight - COLOR_CHECKBOX_Y_FROM_BOTTOM;

        drawState = DRAW_STATE_STABLE;
    }
}

void ofApp::draw()
{
    // Draw button background
    ofSetColor(buttonBgColor);
    ofDrawRectangle(fuelRectStart.x, fuelRectStart.y, fuelWidth, fuelHeight);
    ofDrawRectangle(engineRectStart.x, engineRectStart.y, engineWidth, engineHeight);
    ofDrawRectangle(stagRectStart.x, stagRectStart.y, stagWidth, stagHeight);

    // Draw button label
    ofSetColor(buttonLabelColor);
    verdanaButtonLabel.drawString(fuelLabel,
        fuelRectStart.x + BUTTON_OFFSET, BUTTON_Y + fuelHeight - BUTTON_OFFSET);
    verdanaButtonLabel.drawString(engineLabel,
        engineRectStart.x + BUTTON_OFFSET, BUTTON_Y + engineHeight - BUTTON_OFFSET);
    verdanaButtonLabel.drawString(stagLabel,
        stagRectStart.x + BUTTON_OFFSET, BUTTON_Y + stagHeight - BUTTON_OFFSET);

    if(file_loaded) {
        // Draw slice button
        ofSetColor(buttonBgColor);
        ofDrawRectangle(sliceRectStart.x, sliceRectStart.y, SLICE_BUTTON_WIDTH, SLICE_BUTTON_HEIGHT);
        ofSetColor(buttonLabelColor);
        string sLabel = viewRendered == MODEL_3D_VIEW ? sliceLabel : redoLabel;
        verdanaButtonLabel.drawString(sLabel, sliceRectStart.x + BUTTON_OFFSET, sliceRectStart.y + SLICE_BUTTON_HEIGHT - BUTTON_OFFSET);

        if(viewRendered == MODEL_3D_VIEW) {
            // Draw slider & color chooser
            ofSetColor(activePlane != PLANE_NONE ? sliderColor : sliderInactiveColor);
            ofSetLineWidth(SLIDER_WIDTH);
            ofDrawLine(SLIDER_X, screenHeight - SLIDER_Y_FROM_BOTTOM, SLIDER_X + SLIDER_LENGTH, screenHeight - SLIDER_Y_FROM_BOTTOM);
            ofDrawCircle(sliderXPos, screenHeight - SLIDER_Y_FROM_BOTTOM, SLIDER_THUMB_RADIUS);

            checkBox.draw(colorChooserRectStart.x, colorChooserRectStart.y, COLOR_CHECKBOX_SIZE, COLOR_CHECKBOX_SIZE);
            if(slicedImageFormat == IMAGE_COLOR) {
                checkMark.draw(colorChooserRectStart.x, colorChooserRectStart.y, COLOR_CHECKBOX_SIZE, COLOR_CHECKBOX_SIZE);
            }
            ofSetColor(buttonLabelColor);
            verdanaButtonLabel.drawString(colorChooserLabel, colorChooserRectStart.x + COLOR_CHECKBOX_SIZE + COLOR_CHECKBOX_MARGIN, colorChooserRectStart.y + 30);

            // Draw hint & active axis
            ofSetColor(buttonLabelColor);
            verdanaTickLabel.drawString(hintLabel, HINT_X, screenHeight - HINT_Y_FROM_BOTTOM);
            string activeAxis;
            switch(activePlane) {
            case PLANE_NONE:
                activeAxis = string("NONE");
                break;
            case PLANE_X:
                activeAxis = string("X plane");
                break;
            case PLANE_Y:
                activeAxis = string("Y plane");
                break;
            case PLANE_Z:
                activeAxis = string("Z plane");
                break;
            default:
                activeAxis = string("NONE");
                break;
            }
            string str;
            str.append(activeAxisLabel);
            str.append(activeAxis);
            verdanaTickLabel.drawString(str, ACTIVE_AXIS_X, screenHeight - ACTIVE_AXIS_Y_FROM_BOTTOM);

            if(activePlane != PLANE_NONE) {
                // draw min-max values
                float min = 0, max;
                if(activePlane == PLANE_X) {
                    max = dims[0]-1;
                } else if(activePlane == PLANE_Y) {
                    max = dims[1]-1;
                } else {
                    max = dims[2]-1;
                }
                string minStr = to_string(min);
                string maxStr = to_string(max);
                float maxStrWidth = verdanaTickLabel.stringWidth(maxStr);
                verdanaTickLabel.drawString(minStr, SLIDER_X, screenHeight - SLIDER_Y_FROM_BOTTOM - 10);
                verdanaTickLabel.drawString(maxStr, SLIDER_X + SLIDER_LENGTH - maxStrWidth, screenHeight - SLIDER_Y_FROM_BOTTOM - 10);

                // draw isoVal
                float planeVal;
                if(activePlane == PLANE_X) {
                    planeVal = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[0]-1);
                } else if(activePlane == PLANE_Y) {
                    planeVal = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[1]-1);
                } else {
                    planeVal = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[2]-1);
                }
                string planeValStr = to_string(planeVal);
                float planeValStrHeight = verdanaSliderLabel.stringHeight(planeValStr);
                float planeValStrWidth = verdanaSliderLabel.stringWidth(planeValStr);
                ofSetColor(sliderLabelColor);
                float sliderLabelXPos = sliderXPos - planeValStrWidth/2.0f;
                if(sliderLabelXPos < SLIDER_X) {
                    sliderLabelXPos = SLIDER_X;
                } else if(sliderLabelXPos + planeValStrWidth > SLIDER_X + SLIDER_LENGTH) {
                    sliderLabelXPos = (SLIDER_X + SLIDER_LENGTH) - planeValStrWidth;
                }
                verdanaSliderLabel.drawString(planeValStr, sliderLabelXPos,
                                              screenHeight - SLIDER_Y_FROM_BOTTOM + 10 + planeValStrHeight);
            }

            ofEnableDepthTest();
            cam.begin();
            ofPushMatrix();
                ofTranslate(-xPos, -yPos, -zPos);
                ofDrawAxis(AXIS_LENGTH);
                vboMesh.draw();

                float xPlanePos = 0.0f;
                if(activePlane == PLANE_X) {
                    xPlanePos = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[0]-1);
                }
                xPlane.setPosition(xPlanePos, yPos, zPos);

                float yPlanePos = 0.0f;
                if(activePlane == PLANE_Y) {
                    yPlanePos = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[1]-1);
                }
                yPlane.setPosition(xPos, yPlanePos, zPos);

                float zPlanePos = 0.0f;
                if(activePlane == PLANE_Z) {
                    zPlanePos = ofMap(sliderXPos, SLIDER_X, SLIDER_X + SLIDER_LENGTH, 0, dims[2]-1);
                }
                zPlane.setPosition(xPos, yPos, zPlanePos);

                ofSetColor(activePlane == PLANE_X ? activePlaneColor : planeColor);
                xPlane.draw();
                ofSetColor(activePlane == PLANE_Y ? activePlaneColor : planeColor);
                yPlane.draw();
                ofSetColor(activePlane == PLANE_Z ? activePlaneColor : planeColor);
                zPlane.draw();
            ofPopMatrix();
            cam.end();
            ofDisableDepthTest();
        } else if(viewRendered == SLICE_VIEW) {
            slicedImage.draw((screenWidth - slicedImage.getWidth()) / 2.0f, (screenHeight - slicedImage.getHeight()) / 2.0f);
            legend.draw(screenWidth - legendXFromRight, legendY);

            ofSetColor(tickLabelColor);
            string minStr = to_string(range[0]);
            string maxStr = to_string(range[1]);
            verdanaTickLabel.drawString(minStr, screenWidth - legendXFromRight + legendWidth + 10, legendY + legendHeight + 5);
            verdanaTickLabel.drawString(maxStr, screenWidth - legendXFromRight + legendWidth + 10, legendY + 5);

            float tick_offset = legendHeight/(float)(TICK_COUNT+1);
            for(int i=1;i<=TICK_COUNT;i++) {
                string tickStr = to_string(range[0] + i*(range[1] - range[0]));
                verdanaTickLabel.drawString(tickStr, screenWidth - legendXFromRight + legendWidth + 10, legendY + legendHeight - i*tick_offset + 5);
            }
        }
    }
}

void ofApp::keyPressed(int key)
{
    if(viewRendered == MODEL_3D_VIEW) {
        switch (key) {
        case 'x':
            activePlane = PLANE_X;
            sliderXPos = SLIDER_X;
            break;

        case 'y':
            activePlane = PLANE_Y;
            sliderXPos = SLIDER_X;
            break;

        case 'z':
            activePlane = PLANE_Z;
            sliderXPos = SLIDER_X;
            break;

        case 'r':
            activePlane = PLANE_NONE;
            sliderXPos = SLIDER_X;
            break;
        default:
            break;
        }
    }
}

void ofApp::keyReleased(int key)
{

}

void ofApp::mouseMoved(int x, int y)
{
}

void ofApp::mouseDragged(int x, int y, int button)
{
    if(mouseMode == MOUSE_MODE_CLICK) {
        mouseMode = MOUSE_MODE_DRAG;
    }

    if(mouseMode == MOUSE_MODE_DRAG) {
        sliderXPos = x;
        if(sliderXPos < SLIDER_X) {
            sliderXPos = SLIDER_X;
        } else if(sliderXPos > SLIDER_X + SLIDER_LENGTH) {
            sliderXPos = SLIDER_X + SLIDER_LENGTH;
        }
    }
}

void ofApp::mousePressed(int x, int y, int button)
{
    string filepath = "";
    if (isInsideRect(x, y, fuelRectStart.x, fuelRectStart.y, fuelRectStart.x + fuelWidth, fuelRectStart.y + fuelHeight)) {
        filepath = ofToDataPath("fuel.vti").c_str();
    } else if(isInsideRect(x, y, engineRectStart.x, engineRectStart.y, engineRectStart.x + engineWidth, engineRectStart.y + engineHeight)) {
        filepath = ofToDataPath("engine.vti").c_str();
    } else if(isInsideRect(x, y, stagRectStart.x, stagRectStart.y, stagRectStart.x + stagWidth, stagRectStart.y + stagHeight)) {
        filepath = ofToDataPath("stag.vti").c_str();
    }

    if(!filepath.empty()) {
        if (file_loaded) {
            clearMesh();
        }
        file_loaded = loadFile(filepath);

        return;
    }

    if(isInsideRect(x, y, colorChooserRectStart.x, colorChooserRectStart.y, colorChooserRectStart.x + COLOR_CHECKBOX_SIZE, colorChooserRectStart.y + COLOR_CHECKBOX_SIZE)) {
        slicedImageFormat = slicedImageFormat == IMAGE_COLOR ? IMAGE_GRAYSCALE : IMAGE_COLOR;
        return;
    }

    if(isInsideRect(x, y, sliceRectStart.x, sliceRectStart.y, sliceRectStart.x + SLICE_BUTTON_WIDTH, sliceRectStart.y + SLICE_BUTTON_HEIGHT)) {
        if(viewRendered == MODEL_3D_VIEW) {
            prepareSlicedView();
            viewRendered = SLICE_VIEW;
        } else {
            activePlane = PLANE_NONE;
            sliderXPos = SLIDER_X;
            slicedImage.clear();
            legend.clear();
            viewRendered = MODEL_3D_VIEW;
        }
        return;
    }

    if(isInsideRect(x, y,
                      sliderXPos - SLIDER_THUMB_RADIUS, screenHeight - SLIDER_Y_FROM_BOTTOM + SLIDER_THUMB_RADIUS,
                      sliderXPos + SLIDER_THUMB_RADIUS, screenHeight - SLIDER_Y_FROM_BOTTOM - SLIDER_THUMB_RADIUS)) {
        if(activePlane != PLANE_NONE) {
            sliderXPos = x;
            mouseMode = MOUSE_MODE_CLICK;
            cam.disableMouseInput();
        }
    }
}

void ofApp::mouseReleased(int x, int y, int button)
{
    if(mouseMode == MOUSE_MODE_DRAG || mouseMode == MOUSE_MODE_CLICK) {
        sliderXPos = x;
        if(sliderXPos < SLIDER_X) {
            sliderXPos = SLIDER_X;
        } else if(sliderXPos > SLIDER_X + SLIDER_LENGTH) {
            sliderXPos = SLIDER_X + SLIDER_LENGTH;
        }
        mouseMode = MOUSE_MODE_INACTIVE;
        cam.enableMouseInput();
    }
}

void ofApp::mouseEntered(int x, int y)
{

}

void ofApp::mouseExited(int x, int y)
{

}

void ofApp::windowResized(int w, int h)
{

}

void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

void ofApp::gotMessage(ofMessage msg)
{

}
