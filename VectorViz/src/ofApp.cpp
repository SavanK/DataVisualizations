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
    vectorField->setDivergingCS(divergingCS);
    vectorField->setFilename(ofToDataPath(filepath), colorSpace);
    dims = vectorField->getDims();
    imageData = vectorField->getScreenData();

    image.setFromPixels(imageData, vectorField->getImageWidth(), vectorField->getImageHeight(), OF_IMAGE_GRAYSCALE);
    imageWidth = vectorField->getImageWidth();
    imageHeight = vectorField->getImageHeight();
    imageMaxHeight = vectorField->getImageMaxHeight();
    imageMaxWidth = vectorField->getImageMaxWidth();
    imageXPos = (screenWidth-imageWidth)*0.5f;
    imageYPos = (screenHeight-imageHeight)*0.5f;

    minTd = 0.00001*MAX(
                vectorField->getWorldCoords().xRange.second,
                vectorField->getWorldCoords().yRange.second);
    maxTd = 0.01*MAX(
                vectorField->getWorldCoords().xRange.second,
                vectorField->getWorldCoords().yRange.second);
    minIter = 1000;
    maxIter = 60000;

    // default values
    tdFactor = 0.0005*MAX(
                vectorField->getWorldCoords().xRange.second,
                vectorField->getWorldCoords().yRange.second);
    iterFactor = 5000;

    return true;
}

void ofApp::refreshLegend() {
    if(colorSpace == COLOR_SPACE_GRAYSCALE) {
        legendData = new float[LEGEND_HEIGHT*LEGEND_WIDTH];
        for(int x=0;x<LEGEND_WIDTH;x++) {
            for(int y=0;y<LEGEND_HEIGHT;y++) {
                legendData[y*LEGEND_WIDTH + x] = 1 - (y/(float)LEGEND_HEIGHT);
            }
        }
        legend.setFromPixels(legendData, LEGEND_WIDTH, LEGEND_HEIGHT, OF_IMAGE_GRAYSCALE);
    } else if(colorSpace == COLOR_SPACE_COLOR) {
        legendData = new float[LEGEND_HEIGHT*LEGEND_WIDTH*3];
        for(int x=0;x<LEGEND_WIDTH;x++) {
            for(int y=0;y<LEGEND_HEIGHT;y++) {
                int offset = (y*LEGEND_WIDTH + x)*3;
                if(y > LEGEND_HEIGHT/2.f) {
                    legendData[offset] = divergingCS.midColor.r +
                            ((y-(LEGEND_HEIGHT/2.f))/(LEGEND_HEIGHT/2.f))*(divergingCS.startColor.r-divergingCS.midColor.r);
                    legendData[offset+1] = divergingCS.midColor.g +
                            ((y-(LEGEND_HEIGHT/2.f))/(LEGEND_HEIGHT/2.f))*(divergingCS.startColor.g-divergingCS.midColor.g);
                    legendData[offset+2] = divergingCS.midColor.b +
                            ((y-(LEGEND_HEIGHT/2.f))/(LEGEND_HEIGHT/2.f))*(divergingCS.startColor.b-divergingCS.midColor.b);
                } else {
                    legendData[offset] = divergingCS.endColor.r +
                            (y/(LEGEND_HEIGHT/2.f))*(divergingCS.midColor.r-divergingCS.endColor.r);
                    legendData[offset+1] = divergingCS.endColor.g +
                            (y/(LEGEND_HEIGHT/2.f))*(divergingCS.midColor.g-divergingCS.endColor.g);
                    legendData[offset+2] = divergingCS.endColor.b +
                            (y/(LEGEND_HEIGHT/2.f))*(divergingCS.midColor.b-divergingCS.endColor.b);
                }
            }
        }
        legend.setFromPixels(legendData, LEGEND_WIDTH, LEGEND_HEIGHT, OF_IMAGE_COLOR);
    }
}


ofApp::ofApp()
{
    vectorField = new VectorField();
}

void ofApp::setup()
{
    filename = "focus.vti";
    //old OF default is 96 - but this results in fonts looking larger than in other programs.
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(0);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetSmoothLighting(true);
    ofSetFullscreen(true);

    ofBackground(255, 255, 255);

    sliderLabelFontSize = 18;

    verdanaSliderLabel.load("verdana.ttf", sliderLabelFontSize, true, true);
    verdanaSliderLabel.setLineHeight(20.0f);
    verdanaSliderLabel.setLetterSpacing(1.2f);

    fontSize = 18;

    verdana.load("verdana.ttf", fontSize, true, true);
    verdana.setLineHeight(20.0f);
    verdana.setLetterSpacing(1.2f);

    tickLabelFontSize = 14;

    verdanaTickLabel.load("verdana.ttf", tickLabelFontSize, true, true);
    verdanaTickLabel.setLineHeight(20.0f);
    verdanaTickLabel.setLetterSpacing(1.2f);

    zoomLabel = "Adjust zoom level:";
    csCheckboxLabel = "Color?";
    legendLabel = "Magnitude";
    tdLabel = "Adjust time differential:";
    iterLabel = "Adjust iterations:";
    resetLabel = "Clear streamlines";

    checkbox.load("checkbox.png");
    checkmark.load("checkmark.png");

    sliderColor.set(90, 90, 90);
    sliderThumbColor.set(244, 66, 182);
    sliderLabelColor.set(244, 66, 182);
    labelColor.set(90, 90, 90);
    tickLabelColor.set(90, 90, 90);

    // setup color spaces
    divergingCS.endColor.set(0.2509f, 0.0f, 0.2941f);
    divergingCS.midColor.set(0.9686f, 0.9686f, 0.9686f);
    divergingCS.startColor.set(0.0f, 0.2666f, 0.1058f);

    screenHeight = ofGetScreenHeight();
    screenWidth = ofGetScreenWidth();

    zoomSliderXPos = (screenWidth - SLIDER_LENGTH)*0.5f;
    zoomSliderYPos = 30.f;

    tdSliderXPos = screenWidth*0.5f - SLIDER_LENGTH - 50;
    tdSliderYPos = screenHeight - 100.f;

    iterSliderXPos = screenWidth*0.5f + 50;
    iterSliderYPos = screenHeight - 100.f;

    legendX = screenWidth - 0.05f*screenWidth;
    legendY = (screenHeight-LEGEND_HEIGHT)*0.5f;

    csCheckboxX = legendX - 30;
    csCheckboxY = legendY - 50;

    resetX = legendX - 140;
    resetY = legendY + LEGEND_HEIGHT + 100;

    informationX = screenWidth - 0.2f*screenWidth;
    informationY = 20;

    colorSpace = COLOR_SPACE_GRAYSCALE;
    loadFile(filename);

    zoomFactor = 0.0f;
    subsectionStatus = SUBSECTION_INACTIVE;
    drawState = DRAW_STATE_INVALIDATE;
    mouseMode = MOUSE_MODE_INACTIVE;
    mouseFeature = FEATURE_NONE;    

    refreshLegend();
}

void ofApp::update()
{
    if(drawState == DRAW_STATE_INVALIDATE) {
        imageData = vectorField->getScreenData();
        image.setFromPixels(imageData, vectorField->getImageWidth(), vectorField->getImageHeight(),
                            colorSpace == COLOR_SPACE_COLOR ? OF_IMAGE_COLOR : OF_IMAGE_GRAYSCALE);

        imageWidth = vectorField->getImageWidth();
        imageHeight = vectorField->getImageHeight();
        imageXPos = (screenWidth-imageWidth)*0.5f;
        imageYPos = (screenHeight-imageHeight)*0.5f;

        imageDrawXPos = imageXPos;
        imageDrawYPos = imageYPos;
        imageDrawWidth = imageWidth;
        imageDrawHeight = imageHeight;
        imageSubsectionXPos = 0;
        imageSubsectionYPos = 0;

        if(imageWidth > imageMaxWidth && imageHeight > imageMaxHeight) {
            imageDrawWidth = imageMaxWidth;
            imageDrawHeight = imageMaxHeight;
            imageDrawXPos = (screenWidth - imageDrawWidth)*0.5f;
            imageDrawYPos = (screenHeight - imageDrawHeight)*0.5f;
            imageSubsectionXPos = (imageWidth - imageMaxHeight)*0.5f;
            imageSubsectionYPos = (imageHeight - imageMaxHeight)*0.5f;

            if(imageSubsectionXPos < 0) {
                imageSubsectionXPos=0;
            } else if(imageSubsectionXPos > imageWidth-imageDrawWidth) {
                imageSubsectionXPos=imageWidth-imageDrawWidth;
            }

            if(imageSubsectionYPos < 0) {
                imageSubsectionYPos=0;
            } else if(imageSubsectionYPos > imageHeight-imageDrawHeight) {
                imageSubsectionYPos=imageHeight-imageDrawHeight;
            }

            subsectionStatus = SUBSECTION_ACTIVE;
        } else if(imageWidth > imageMaxWidth) {
            imageDrawWidth = imageMaxWidth;
            imageDrawXPos = (screenWidth - imageDrawWidth)*0.5f;
            imageSubsectionXPos = (imageWidth - imageMaxHeight)*0.5f;

            if(imageSubsectionXPos < 0) {
                imageSubsectionXPos=0;
            } else if(imageSubsectionXPos > imageWidth-imageDrawWidth) {
                imageSubsectionXPos=imageWidth-imageDrawWidth;
            }

            subsectionStatus = SUBSECTION_ACTIVE;
        } else if(imageHeight > imageMaxHeight) {
            imageDrawHeight = imageMaxHeight;
            imageDrawYPos = (screenHeight - imageDrawHeight)*0.5f;
            imageSubsectionYPos = (imageHeight - imageMaxHeight)*0.5f;

            if(imageSubsectionYPos < 0) {
                imageSubsectionYPos=0;
            } else if(imageSubsectionYPos > imageHeight-imageDrawHeight) {
                imageSubsectionYPos=imageHeight-imageDrawHeight;
            }

            subsectionStatus = SUBSECTION_ACTIVE;
        } else {
            subsectionStatus = SUBSECTION_INACTIVE;
        }

        streamLines = vectorField->getStreamLines(imageSubsectionXPos, imageSubsectionYPos);
        drawState = DRAW_STATE_STABLE;
    }
}

void ofApp::draw()
{
    // Draw zoom slider
    ofSetColor(sliderColor);
    ofSetLineWidth(SLIDER_WIDTH);
    ofDrawLine(zoomSliderXPos, zoomSliderYPos, zoomSliderXPos + SLIDER_LENGTH, zoomSliderYPos + SLIDER_WIDTH);
    ofSetLineWidth(1);
    ofSetColor(sliderThumbColor);
    ofDrawCircle(((zoomFactor/MAX_ZOOM_FACTOR)*SLIDER_LENGTH) + zoomSliderXPos, zoomSliderYPos, SLIDER_THUMB_RADIUS);

    ofSetColor(labelColor);
    float strWidth = verdanaSliderLabel.stringWidth(zoomLabel);
    float strHeight = verdanaSliderLabel.stringHeight(zoomLabel);
    verdanaSliderLabel.drawString(zoomLabel, zoomSliderXPos - 1.2f*strWidth, zoomSliderYPos+0.3f*strHeight);

    // Draw zoom slider ticks
    ofSetColor(tickLabelColor);
    float tick_offset = SLIDER_LENGTH/(float)(ZOOM_TICK_COUNT-1);
    for(int i=0;i<ZOOM_TICK_COUNT;i++) {
        string tickStr = to_string(1+(i/(float)(ZOOM_TICK_COUNT-1))*MAX_ZOOM_FACTOR);
        strWidth = verdanaTickLabel.stringWidth(tickStr);
        verdanaTickLabel.drawString(tickStr, zoomSliderXPos + i*tick_offset - strWidth*0.5f, zoomSliderYPos+25);
    }

    // Draw td slider
    ofSetColor(sliderColor);
    ofSetLineWidth(SLIDER_WIDTH);
    ofDrawLine(tdSliderXPos, tdSliderYPos, tdSliderXPos + SLIDER_LENGTH, tdSliderYPos + SLIDER_WIDTH);
    ofSetLineWidth(1);
    ofSetColor(sliderThumbColor);
    ofDrawCircle((((tdFactor-minTd)/(maxTd-minTd))*SLIDER_LENGTH) + tdSliderXPos, tdSliderYPos, SLIDER_THUMB_RADIUS);

    ofSetColor(labelColor);
    strWidth = verdanaSliderLabel.stringWidth(tdLabel);
    strHeight = verdanaSliderLabel.stringHeight(tdLabel);
    verdanaSliderLabel.drawString(tdLabel, tdSliderXPos, tdSliderYPos - 1.3f*strHeight);

    // Draw td slider ticks
    ofSetColor(tickLabelColor);
    tick_offset = SLIDER_LENGTH/(float)(ZOOM_TICK_COUNT-1);
    for(int i=0;i<ZOOM_TICK_COUNT;i++) {
        string tickStr = to_string(minTd+i*(maxTd-minTd)/(ZOOM_TICK_COUNT-1));
        strWidth = verdanaTickLabel.stringWidth(tickStr);
        verdanaTickLabel.drawString(tickStr, tdSliderXPos + i*tick_offset - strWidth*0.5f, tdSliderYPos+25);
    }

    // Draw iter slider
    ofSetColor(sliderColor);
    ofSetLineWidth(SLIDER_WIDTH);
    ofDrawLine(iterSliderXPos, iterSliderYPos, iterSliderXPos + SLIDER_LENGTH, iterSliderYPos + SLIDER_WIDTH);
    ofSetLineWidth(1);
    ofSetColor(sliderThumbColor);

    ofDrawCircle((((iterFactor-minIter)/(float)(maxIter-minIter))*SLIDER_LENGTH) + iterSliderXPos, iterSliderYPos, SLIDER_THUMB_RADIUS);

    ofSetColor(labelColor);
    strWidth = verdanaSliderLabel.stringWidth(iterLabel);
    strHeight = verdanaSliderLabel.stringHeight(iterLabel);
    verdanaSliderLabel.drawString(iterLabel, iterSliderXPos, iterSliderYPos - 1.3f*strHeight);

    // Draw iter slider ticks
    ofSetColor(tickLabelColor);
    tick_offset = SLIDER_LENGTH/(float)(ZOOM_TICK_COUNT-1);
    for(int i=0;i<ZOOM_TICK_COUNT;i++) {
        string tickStr = to_string((int)(minIter+i*(maxIter-minIter)/(ZOOM_TICK_COUNT-1)));
        strWidth = verdanaTickLabel.stringWidth(tickStr);
        verdanaTickLabel.drawString(tickStr, iterSliderXPos + i*tick_offset - strWidth*0.5f, iterSliderYPos+25);
    }

    // Draw legend
    ofSetColor(ofColor::white);
    legend.draw(legendX, legendY);
    ofSetColor(labelColor);
    strWidth = verdana.stringWidth(legendLabel);
    verdana.drawString(legendLabel, legendX-strWidth*0.5f+15, legendY+LEGEND_HEIGHT+30);

    // Draw legend ticks
    ofSetColor(tickLabelColor);
    double* magnitudeRange = vectorField->getMagnitudeRange();
    tick_offset = LEGEND_HEIGHT/(float)(MAGNITUDE_TICK_COUNT-1);
    for(int i=0;i<MAGNITUDE_TICK_COUNT;i++) {
        string tickStr = to_string(magnitudeRange[0] + i*(magnitudeRange[1] - magnitudeRange[0]));
        strWidth = verdanaTickLabel.stringWidth(tickStr);
        verdanaTickLabel.drawString(tickStr, legendX - strWidth - 5, legendY + LEGEND_HEIGHT - i*tick_offset + 5);
    }

    // Draw information
    ofSetColor(tickLabelColor);
    char str[100];
    sprintf(str, "worldCoords: [%d,%d]x[%d,%d]",
            (int)vectorField->getWorldCoords().xRange.first,
            (int)vectorField->getWorldCoords().xRange.second,
            (int)vectorField->getWorldCoords().yRange.first,
            (int)vectorField->getWorldCoords().yRange.second);
    verdanaTickLabel.drawString(str, informationX, informationY);
    sprintf(str, "screenCoords: [%d,%d]x[%d,%d]",
            (int)vectorField->getScreenCoords().xRange.first,
            (int)vectorField->getScreenCoords().xRange.second,
            (int)vectorField->getScreenCoords().yRange.first,
            (int)vectorField->getScreenCoords().yRange.second);
    verdanaTickLabel.drawString(str, informationX, informationY+20);
    sprintf(str, "dataArrayCoords: [%d,%d]x[%d,%d]",
            (int)vectorField->getDataArrayCoords().xRange.first,
            (int)vectorField->getDataArrayCoords().xRange.second,
            (int)vectorField->getDataArrayCoords().yRange.first,
            (int)vectorField->getDataArrayCoords().yRange.second);
    verdanaTickLabel.drawString(str, informationX, informationY+40);
    sprintf(str, "time differential: %f", tdFactor);
    verdanaTickLabel.drawString(str, informationX, informationY+60);
    sprintf(str, "number of iterations: %d", iterFactor);
    verdanaTickLabel.drawString(str, informationX, informationY+80);

    // Draw colorspace chooser
    ofSetColor(labelColor);
    checkbox.draw(csCheckboxX, csCheckboxY, CHECK_BOX_SIZE, CHECK_BOX_SIZE);
    if(colorSpace == COLOR_SPACE_COLOR) {
        checkmark.draw(csCheckboxX, csCheckboxY, CHECK_BOX_SIZE, CHECK_BOX_SIZE);
    }
    verdana.drawString(csCheckboxLabel, csCheckboxX + CHECK_BOX_SIZE + 5, csCheckboxY + 0.7f*CHECK_BOX_SIZE);

    // Draw reset streamlines button
    ofSetColor(labelColor);
    ofDrawRectangle(resetX, resetY, RESET_BUTTON_SIZE, RESET_BUTTON_SIZE);
    verdana.drawString(resetLabel, resetX + RESET_BUTTON_SIZE + 5, resetY + 0.7f*RESET_BUTTON_SIZE);

    ofPushMatrix();
        ofTranslate(imageDrawXPos, imageDrawYPos);
        ofSetColor(ofColor::white);

        // Draw image
        image.drawSubsection(0, 0, imageDrawWidth, imageDrawHeight, imageSubsectionXPos, imageSubsectionYPos);

        // Draw stream lines
        ofSetLineWidth(2);
        ofSetColor(ofColor::red);
        for(int i=0;i<streamLines.size();i++) {
            ofPolyline streamline = streamLines[i];
            streamline.draw();
        }
        ofSetLineWidth(1);
    ofPopMatrix();
}

void ofApp::keyPressed(int key)
{
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
        if(mouseFeature == FEATURE_ZOOM_SLIDER) {
            zoomFactor = ((x-zoomSliderXPos) / (float) SLIDER_LENGTH) * MAX_ZOOM_FACTOR;
            if(zoomFactor < 0) {
                zoomFactor = 0;
            } else if(zoomFactor > MAX_ZOOM_FACTOR) {
                zoomFactor = MAX_ZOOM_FACTOR;
            }
            //vectorField->adjustImageDims(zoomFactor);
            //drawState = DRAW_STATE_INVALIDATE;
        } else if(mouseFeature == FEATURE_TD_SLIDER) {
            tdFactor = minTd + ((x-tdSliderXPos) / (float) SLIDER_LENGTH) * (maxTd-minTd);
            if(tdFactor < minTd) {
                tdFactor = minTd;
            } else if(tdFactor > maxTd) {
                tdFactor = maxTd;
            }
        } else if(mouseFeature == FEATURE_ITER_SLIDER) {
            iterFactor = minIter + ((x-iterSliderXPos) / (float) SLIDER_LENGTH) * (maxIter-minIter);
            if(iterFactor < minIter) {
                iterFactor = minIter;
            } else if(iterFactor > maxIter) {
                iterFactor = maxIter;
            }
        } else if(mouseFeature == FEATURE_SEED_PICKER) {
            if(subsectionStatus == SUBSECTION_ACTIVE) {
                mouseFeature = FEATURE_PAN;
                savedSubsecXpos = imageSubsectionXPos;
                savedSubsecYpos = imageSubsectionYPos;
            }
        }

        if(mouseFeature == FEATURE_PAN && subsectionStatus == SUBSECTION_ACTIVE) {
            // Pan
            imageSubsectionXPos = savedSubsecXpos - (x-mouseClick.x);
            imageSubsectionYPos = savedSubsecYpos - (y-mouseClick.y);

            if(imageSubsectionXPos < 0) {
                imageSubsectionXPos=0;
            } else if(imageSubsectionXPos > imageWidth-imageDrawWidth) {
                imageSubsectionXPos=imageWidth-imageDrawWidth;
            }

            if(imageSubsectionYPos < 0) {
                imageSubsectionYPos=0;
            } else if(imageSubsectionYPos > imageHeight-imageDrawHeight) {
                imageSubsectionYPos=imageHeight-imageDrawHeight;
            }
        }
    }
}

void ofApp::mousePressed(int x, int y, int button)
{
    if(x >= imageDrawXPos && x <= imageDrawXPos+imageDrawWidth && y >= imageDrawYPos && y <= imageDrawYPos+imageDrawHeight) {
        mouseMode = MOUSE_MODE_CLICK;
        mouseFeature = FEATURE_SEED_PICKER;
        mouseClick.x = x;
        mouseClick.y = y;
        return;
    }

    if(x >= resetX && x<=resetX+RESET_BUTTON_SIZE && y >= resetY && y <= resetY+RESET_BUTTON_SIZE) {
        streamLines.clear();
        vectorField->clearStreamlines();
        return;
    }

    float zoomThumbX, zoomThumbY;
    zoomThumbX = zoomSliderXPos + (zoomFactor/MAX_ZOOM_FACTOR)*SLIDER_LENGTH;
    zoomThumbY = zoomSliderYPos;
    if(isInsideRect(x, y,
                      zoomThumbX - SLIDER_THUMB_RADIUS, zoomThumbY + SLIDER_THUMB_RADIUS,
                      zoomThumbX + SLIDER_THUMB_RADIUS, zoomThumbY - SLIDER_THUMB_RADIUS)) {
        zoomFactor = ((x-zoomSliderXPos) / (float) SLIDER_LENGTH) * MAX_ZOOM_FACTOR;
        mouseMode = MOUSE_MODE_CLICK;
        mouseFeature = FEATURE_ZOOM_SLIDER;
        return;
    }

    float tdThumbX, tdThumbY;
    tdThumbX = tdSliderXPos + (((tdFactor-minTd)/(maxTd-minTd))*SLIDER_LENGTH);
    tdThumbY = tdSliderYPos;
    if(isInsideRect(x, y,
                      tdThumbX - SLIDER_THUMB_RADIUS, tdThumbY + SLIDER_THUMB_RADIUS,
                      tdThumbX + SLIDER_THUMB_RADIUS, tdThumbY - SLIDER_THUMB_RADIUS)) {
        tdFactor = minTd + ((x-tdSliderXPos) / (float) SLIDER_LENGTH) * (maxTd-minTd);
        mouseMode = MOUSE_MODE_CLICK;
        mouseFeature = FEATURE_TD_SLIDER;
        return;
    }

    float iterThumbX, iterThumbY;
    iterThumbX = iterSliderXPos + (((iterFactor-minIter)/(float)(maxIter-minIter))*SLIDER_LENGTH);
    iterThumbY = iterSliderYPos;
    if(isInsideRect(x, y,
                      iterThumbX - SLIDER_THUMB_RADIUS, iterThumbY + SLIDER_THUMB_RADIUS,
                      iterThumbX + SLIDER_THUMB_RADIUS, iterThumbY - SLIDER_THUMB_RADIUS)) {
        iterFactor = minIter + ((x-iterSliderXPos) / (float) SLIDER_LENGTH) * (maxIter-minIter);
        mouseMode = MOUSE_MODE_CLICK;
        mouseFeature = FEATURE_ITER_SLIDER;
        return;
    }

    if(x>=csCheckboxX && x<=csCheckboxX+CHECK_BOX_SIZE && y>=csCheckboxY && y<=csCheckboxY+CHECK_BOX_SIZE) {
        if(colorSpace == COLOR_SPACE_COLOR)
            colorSpace = COLOR_SPACE_GRAYSCALE;
        else
            colorSpace = COLOR_SPACE_COLOR;

        refreshLegend();
        vectorField->changeColorSpace(colorSpace);
        drawState = DRAW_STATE_INVALIDATE;
        return;
    }
}

void ofApp::mouseReleased(int x, int y, int button)
{
    if(mouseMode == MOUSE_MODE_DRAG || mouseMode == MOUSE_MODE_CLICK) {
        if(mouseFeature == FEATURE_ZOOM_SLIDER) {
            zoomFactor = ((x-zoomSliderXPos) / (float) SLIDER_LENGTH) * MAX_ZOOM_FACTOR;
            if(zoomFactor < 0) {
                zoomFactor = 0;
            } else if(zoomFactor > MAX_ZOOM_FACTOR) {
                zoomFactor = MAX_ZOOM_FACTOR;
            }

            vectorField->adjustImageDims(zoomFactor);
            drawState = DRAW_STATE_INVALIDATE;
        } else if(mouseFeature == FEATURE_TD_SLIDER) {
            tdFactor = minTd + ((x-tdSliderXPos) / (float) SLIDER_LENGTH) * (maxTd-minTd);
            if(tdFactor < minTd) {
                tdFactor = minTd;
            } else if(tdFactor > maxTd) {
                tdFactor = maxTd;
            }
        } else if(mouseFeature == FEATURE_ITER_SLIDER) {
            iterFactor = minIter + ((x-iterSliderXPos) / (float) SLIDER_LENGTH) * (maxIter-minIter);
            if(iterFactor < minIter) {
                iterFactor = minIter;
            } else if(iterFactor > maxIter) {
                iterFactor = maxIter;
            }
        } else if(mouseFeature == FEATURE_PAN) {
            // end Pan
            imageSubsectionXPos = savedSubsecXpos - (x-mouseClick.x);
            imageSubsectionYPos = savedSubsecYpos - (y-mouseClick.y);

            if(imageSubsectionXPos < 0) {
                imageSubsectionXPos=0;
            } else if(imageSubsectionXPos > imageWidth-imageDrawWidth) {
                imageSubsectionXPos=imageWidth-imageDrawWidth;
            }

            if(imageSubsectionYPos < 0) {
                imageSubsectionYPos=0;
            } else if(imageSubsectionYPos > imageHeight-imageDrawHeight) {
                imageSubsectionYPos=imageHeight-imageDrawHeight;
            }

            streamLines = vectorField->getStreamLines(imageSubsectionXPos, imageSubsectionYPos);
            savedSubsecXpos = -1;
            savedSubsecYpos = -1;
        } else if(mouseFeature == FEATURE_SEED_PICKER) {
            ofVec2f startPoint;
            startPoint.x = (int)(imageSubsectionXPos + mouseClick.x-imageDrawXPos);
            startPoint.y = (int)(imageSubsectionYPos + mouseClick.y-imageDrawYPos);
            vectorField->rk4IntegrateStreamLine(startPoint, tdFactor, iterFactor);
            streamLines = vectorField->getStreamLines(imageSubsectionXPos, imageSubsectionYPos);
        }

        mouseFeature = FEATURE_NONE;
        mouseMode = MOUSE_MODE_INACTIVE;
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
