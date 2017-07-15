#include "ofApp.h"

bool ofApp::isInsideRect(float x, float y,
                         float rec_x_start, float rec_y_start,
                         float rec_x_end, float rec_y_end)
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

void ofApp::loadFile(string file)
{
    imageReader->setFilename(ofToDataPath(file).c_str());
    dims = imageReader->getDims();
    range = imageReader->getRange();
    visibleRange[0] = visibleTfRange[0] = range[0];
    visibleRange[1] = visibleTfRange[1] = range[1];
    data = imageReader->getData();
    int i=0;
    for(i=0;i<range[1]-range[0];i++) {
        pair<float, int> val;
        val.first = (int)range[0] + i;
        val.second = 0;
        histogramVals.push_back(val);
    }

    i=0;
    for(int z=0; z<dims[2];z++) {
        for(int y=0;y<dims[1];y++) {
            for(int x=0;x<dims[0];x++) {
                int index = (int)round(data[i]-range[0]);
                histogramVals[index].second++;
                i++;
            }
        }
    }

    int minCount = histogramVals[0].second;
    int maxCount = histogramVals[0].second;

    for(i=1; i<histogramVals.size();i++) {
        if(histogramVals[i].second > maxCount) {
            maxCount = histogramVals[i].second;
        } else if(histogramVals[i].second < minCount) {
            minCount = histogramVals[i].second;
        }
    }

    histogramRange[0] = minCount;
    histogramRange[1] = maxCount;

    histLogScale = false;
    histSliderMinXPos = SLIDER_HIST_X;
    histSliderMaxXPos = SLIDER_HIST_X + SLIDER_LENGTH;
    tfSliderMinXPos = SLIDER_TF_X;
    tfSliderMaxXPos = SLIDER_TF_X + SLIDER_LENGTH;

    // set it to diverging by default
    tfColorDataPoints.minVal.dataPoint = visibleTfRange[0];
    tfColorDataPoints.minVal.color = divergingCS.startColor;
    tfColorDataPoints.maxVal.dataPoint = visibleTfRange[1];
    tfColorDataPoints.maxVal.color = divergingCS.endColor;
    ColorTfValue midColorVal;
    midColorVal.dataPoint = visibleTfRange[0] + 0.5f*(visibleTfRange[1] - visibleTfRange[0]);
    midColorVal.color = divergingCS.midColor;
    tfColorDataPoints.intermediateVals.push_back(midColorVal);

    tfOpacityDataPoints.minVal.dataPoint = visibleTfRange[0];
    tfOpacityDataPoints.minVal.opacity = 0.0f;
    tfOpacityDataPoints.maxVal.dataPoint = visibleTfRange[1];
    tfOpacityDataPoints.maxVal.opacity = 1.0f;
    OpacityTfValue midOpacityVal;
    midOpacityVal.dataPoint = visibleTfRange[0] + 0.5f*(visibleTfRange[1] - visibleTfRange[0]);
    midOpacityVal.opacity = 0.5f;
    tfOpacityDataPoints.intermediateVals.push_back(midOpacityVal);

    vr.setup(imageReader);
    vr.setupRenderer();
    vr.color->SetAboveRangeColor(0.9491f, 0.9569f, 0.2588f);
    vr.color->SetBelowRangeColor(0.2824f, 0.8275f, 0.298f);
    if(useBelowRange)
        vr.color->UseBelowRangeColorOn();
    else
        vr.color->UseBelowRangeColorOff();
    if(useAboveRange)
        vr.color->UseAboveRangeColorOn();
    else
        vr.color->UseAboveRangeColorOff();
    double newRange[2] = {(double)visibleTfRange[0], (double)visibleTfRange[1]};
    vr.color->AdjustRange(newRange);
    updateColor();
    updateOpacity();
}

void ofApp::setup() {
    filename = string("tooth.vti");
    ofTrueTypeFont::setGlobalDpi(72);
    ofEnableSmoothing();
    ofSetSmoothLighting(true);

    ofBackground(140, 140, 140);

    buttonLabelFontSize = 22;
    titleLabelFontSize = 26;
    genLabelFontSize = 18;
    sliderLabelFontSize = 16;
    tickLabelFontSize = 14;

    verdanaButtonLabel.load("verdana.ttf", buttonLabelFontSize, true, true);
    verdanaButtonLabel.setLineHeight(20.0f);
    verdanaButtonLabel.setLetterSpacing(1.2f);

    verdanaTitleLabel.load("verdana.ttf", titleLabelFontSize, true, true);
    verdanaTitleLabel.setLineHeight(20.0f);
    verdanaTitleLabel.setLetterSpacing(1.2f);

    verdanaGenLabel.load("verdana.ttf", genLabelFontSize, true, true);
    verdanaGenLabel.setLineHeight(20.0f);
    verdanaGenLabel.setLetterSpacing(1.1f);

    verdanaSliderLabel.load("verdana.ttf", sliderLabelFontSize, true, true);
    verdanaSliderLabel.setLineHeight(20.0f);
    verdanaSliderLabel.setLetterSpacing(0.9f);

    verdanaTickLabel.load("verdana.ttf", tickLabelFontSize, true, true);
    verdanaTickLabel.setLineHeight(20.0f);
    verdanaTickLabel.setLetterSpacing(1.2f);

    histLogLabel = string("Y axis log scale");
    histSliderLabel = string("Visible data range: ");
    tfSliderLabel = string("Visible data range: ");
    resetColorLabel = string("Reset color TF");
    resetOpacityLabel = string("Reset opacity TF");
    aboveRangeLabel = string("Use above range");
    belowRangeLabel = string("Use below range");

    checkBox.load("checkbox.png");
    checkMark.load("checkmark.png");

    buttonBgColor.set(66, 134, 244);
    buttonLabelColor.set(255, 255, 255);
    titleLabelColor.set(0, 0, 0);
    genLabelColor.set(255, 255, 255);
    sliderColor.set(244, 66, 182);
    sliderLabelColor.set(244, 66, 182);
    sliderInactiveColor.set(90, 90, 90);
    optionsLabelColor.set(90, 90, 90);
    tfThumbColor.set(60, 60, 60);
    tickLabelColor.set(90, 90, 90);

    // setup color spaces
    divergingCS.startColor.set(0.231373f, 0.298039f, 0.752941f);
    divergingCS.midColor.set(0.865003f, 0.865003f, 0.865003f);
    divergingCS.endColor.set(0.705882f, 0.015686f, 0.14902f);

    imageReader = new ImageReader();
    titleLabel = filename;
    useBelowRange = false;
    useAboveRange = false;
    loadFile(filename);
    startedVTK = false;

    drawState = DRAW_STATE_INVALIDATE;
    mouseMode = MOUSE_MODE_INACTIVE;
    thumbClicked = SLIDER_NO_THUMB;
    tfColorClickIndex =-1;
    tfOpacityClickIndex = -1;
}

void ofApp::update() {
    if(drawState == DRAW_STATE_INVALIDATE) {
        // Measure title position
        float str_height = verdanaTitleLabel.stringHeight(titleLabel);
        titleX = 20;
        titleY = 20 + 1.3f*str_height;

        // Measure histogram
        histogramBoxHeight = 400;
        histogramBoxWidth = ofGetWindowWidth();
        histogramX = 0;
        histogramY = titleY + 10;
        histogramBottomOffset = 0.20f*histogramBoxHeight;
        histogramTopOffset = 0.15f*histogramBoxHeight;
        histogramLeftOffset = 0.15f*histogramBoxWidth;
        histogramHeight = histogramBoxHeight - histogramBottomOffset - histogramTopOffset;
        histogramWidth = histogramBoxWidth - histogramLeftOffset;
        histogramOrigin.x = histogramLeftOffset;
        histogramOrigin.y = histogramBoxHeight - histogramBottomOffset;
        histBlockLength = histogramWidth / (visibleRange[1]-visibleRange[0]);

        str_height = verdanaGenLabel.stringHeight(histLogLabel);
        histLogX = histogramLeftOffset;
        histLogY = str_height*1.8f;

        // Measure transfer functions
        tfBoxHeight = 500;
        tfBoxWidth = ofGetWindowWidth();
        tfX = 0;
        tfY = histogramY + histogramBoxHeight + 10;
        tfLROffset = 0.05f*tfBoxWidth;
        tfTopOffset = tfBottomOffset = 0.05f*tfBoxHeight;
        tfOpacityHeight = (int)(0.35f*tfBoxHeight);
        tfOpacityWidth = (int)(tfBoxWidth - 2*tfLROffset);
        tfOpacityOrigin.x = tfLROffset;
        tfOpacityOrigin.y = tfTopOffset + tfOpacityHeight;
        tfColorHeight = 30;
        tfColorWidth = (int)(tfBoxWidth - 2*tfLROffset);
        tfColorOrigin.x = tfLROffset;
        tfColorOrigin.y = tfOpacityOrigin.y + 10 + tfColorHeight;

        // Measure color tf
        float* tfColorImageData = new float[tfColorWidth*tfColorHeight*3];
        for(int p=0;p<tfColorDataPoints.intermediateVals.size();p++) {
            if(p == 0) {
                // min to first intermediate data point
                int startPoint = 0;
                int endPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                     visibleTfRange[0], visibleTfRange[1], 0, tfColorWidth);
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*3;
                    ofFloatColor startColor = tfColorDataPoints.minVal.color;
                    ofFloatColor endColor = tfColorDataPoints.intermediateVals.at(p).color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfColorHeight;j++) {
                        tfColorImageData[j*tfColorWidth*3 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfColorImageData[j*tfColorWidth*3 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfColorImageData[j*tfColorWidth*3 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                    }
                }
            }

            if(p+1 < tfColorDataPoints.intermediateVals.size()) {
                // between intermediate data points
                int startPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                            visibleTfRange[0], visibleTfRange[1], 0, tfColorWidth);
                int endPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p+1).dataPoint,
                                          visibleTfRange[0], visibleTfRange[1], 0, tfColorWidth);
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*3;
                    ofFloatColor startColor = tfColorDataPoints.intermediateVals.at(p).color;
                    ofFloatColor endColor = tfColorDataPoints.intermediateVals.at(p+1).color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfColorHeight;j++) {
                        tfColorImageData[j*tfColorWidth*3 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfColorImageData[j*tfColorWidth*3 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfColorImageData[j*tfColorWidth*3 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                    }
                }
            }

            if(p+1 == tfColorDataPoints.intermediateVals.size()) {
                // last intermediate data point to max
                int startPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                            visibleTfRange[0], visibleTfRange[1], 0, tfColorWidth);
                int endPoint = tfColorWidth;
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*3;
                    ofFloatColor startColor = tfColorDataPoints.intermediateVals.at(p).color;
                    ofFloatColor endColor = tfColorDataPoints.maxVal.color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfColorHeight;j++) {
                        tfColorImageData[j*tfColorWidth*3 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfColorImageData[j*tfColorWidth*3 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfColorImageData[j*tfColorWidth*3 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                    }
                }
            }
        }
        tfColor.setFromPixels(tfColorImageData, tfColorWidth, tfColorHeight, OF_IMAGE_COLOR);

        // Measure opacity tf
        float* tfOpacityImageData = new float[tfOpacityWidth*tfOpacityHeight*4];
        for(int p=0;p<tfColorDataPoints.intermediateVals.size();p++) {
            if(p == 0) {
                // min to first intermediate data point
                int startPoint = 0;
                int endPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                     visibleTfRange[0], visibleTfRange[1], 0, tfOpacityWidth);
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*4;
                    ofFloatColor startColor = tfColorDataPoints.minVal.color;
                    ofFloatColor endColor = tfColorDataPoints.intermediateVals.at(p).color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfOpacityHeight;j++) {
                        tfOpacityImageData[j*tfOpacityWidth*4 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+3)] = 1 - (j/(float)(tfOpacityHeight-1));
                    }
                }
            }

            if(p+1 < tfColorDataPoints.intermediateVals.size()) {
                // between intermediate data points
                int startPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                            visibleTfRange[0], visibleTfRange[1], 0, tfOpacityWidth);
                int endPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p+1).dataPoint,
                                          visibleTfRange[0], visibleTfRange[1], 0, tfOpacityWidth);
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*4;
                    ofFloatColor startColor = tfColorDataPoints.intermediateVals.at(p).color;
                    ofFloatColor endColor = tfColorDataPoints.intermediateVals.at(p+1).color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfOpacityHeight;j++) {
                        tfOpacityImageData[j*tfOpacityWidth*4 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+3)] = 1 - (j/(float)(tfOpacityHeight-1));
                    }
                }
            }

            if(p+1 == tfColorDataPoints.intermediateVals.size()) {
                // last intermediate data point to max
                int startPoint = (int)ofMap(tfColorDataPoints.intermediateVals.at(p).dataPoint,
                                            visibleTfRange[0], visibleTfRange[1], 0, tfOpacityWidth);
                int endPoint = tfOpacityWidth;
                for(int i=startPoint;i<endPoint;i++) {
                    int k=i*4;
                    ofFloatColor startColor = tfColorDataPoints.intermediateVals.at(p).color;
                    ofFloatColor endColor = tfColorDataPoints.maxVal.color;
                    float frac = (i-startPoint)/(float)(endPoint - startPoint);

                    for(int j=0;j<tfOpacityHeight;j++) {
                        tfOpacityImageData[j*tfOpacityWidth*4 + k] = startColor.r + frac*(endColor.r - startColor.r);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+1)] = startColor.g + frac*(endColor.g - startColor.g);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+2)] = startColor.b + frac*(endColor.b - startColor.b);
                        tfOpacityImageData[j*tfOpacityWidth*4 + (k+3)] = 1 - (j/(float)(tfOpacityHeight-1));
                    }
                }
            }
        }
        tfOpacity.setFromPixels(tfOpacityImageData, tfOpacityWidth, tfOpacityHeight, OF_IMAGE_COLOR_ALPHA);

        // Measure reset buttons
        float str_width = verdanaButtonLabel.stringWidth(resetOpacityLabel);
        str_height = verdanaButtonLabel.stringHeight(resetOpacityLabel);
        resetOpacityWidth = str_width + 40;
        resetOpacityX = tfColorOrigin.x + 0.5f*tfColorWidth - resetOpacityWidth - 10;
        resetOpacityY = tfColorOrigin.y + 75;
        resetOpacityHeight = str_height + 20;

        str_width = verdanaButtonLabel.stringWidth(resetColorLabel);
        resetColorWidth = str_width + 40;
        resetColorX = tfColorOrigin.x + 0.5f*tfColorWidth + 10;
        resetColorY = tfColorOrigin.y + 75;
        resetColorHeight = str_height + 20;

        // Measure above range & below range
        aboveRangeX = tfColorOrigin.x;
        aboveRangeY = resetColorY + resetColorHeight + 30;
        belowRangeX = tfColorOrigin.x;
        belowRangeY = resetColorY + resetColorHeight + 60;

        drawState = DRAW_STATE_STABLE;
    }
}

void ofApp::draw() {
    // Draw title
    ofSetColor(titleLabelColor);
    verdanaTitleLabel.drawString(titleLabel, titleX, titleY);

    // Draw histogram
    ofPushMatrix();
        ofTranslate(histogramX, histogramY);
        // Draw canvas
        ofSetColor(255, 255, 255);
        ofDrawRectangle(0, 0, histogramBoxWidth, histogramBoxHeight);

        // Draw log scale option
        ofSetColor(optionsLabelColor);
        verdanaGenLabel.drawString(histLogLabel, histLogX + 1.2f*CHECKBOX_SIZE, histLogY);
        checkBox.draw(histLogX, histLogY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        if(histLogScale) {
            checkMark.draw(histLogX, histLogY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        }

        // Draw axis
        ofSetColor(titleLabelColor);
        ofVec3f origin, endX, endY;
        origin.x = histogramOrigin.x;
        origin.y = histogramOrigin.y;
        origin.z = 0;
        endX.x = origin.x + histogramWidth;
        endX.y = origin.y;
        endX.z = 0;
        endY.x = origin.x;
        endY.y = origin.y - histogramHeight;
        endY.z = 0;
        ofDrawArrow(origin, endX, 4.f);
        ofDrawArrow(origin, endY, 4.f);

        // Draw values (rectangles)
        ofSetColor(buttonBgColor);
        for(int i=visibleRange[0];i<visibleRange[1];i++) {
            float h;
            if(histLogScale) {
                float minRange = log(MAX(histogramRange[0], 1));
                float maxRange = log(histogramRange[1]);
                if(histogramVals[i].second != 0) {
                    h = ofMap(log(histogramVals[i].second), minRange, maxRange, 0, histogramHeight);
                } else {
                    h = 0;
                }
            } else {
                h = ofMap(histogramVals[i].second, histogramRange[0], histogramRange[1], 0, histogramHeight);
            }
            ofDrawRectangle(histogramOrigin.x + (i - (int)visibleRange[0])*histBlockLength, histogramOrigin.y - h, histBlockLength, h);
        }

        // Draw histogram ticks
        ofSetColor(tickLabelColor);
        float tickXOff = histogramWidth / (float)HIST_X_TICK_COUNT;
        float tickXValOff = (visibleRange[1]-visibleRange[0]) / (float)HIST_X_TICK_COUNT;
        for(int i=0;i<=HIST_X_TICK_COUNT;i++) {
            string str = to_string((int)(visibleRange[0] + i*tickXValOff));
            float strW = verdanaTickLabel.stringWidth(str);
            if(i==0) {
                verdanaTickLabel.drawString(str, histogramOrigin.x + i*tickXOff, histogramOrigin.y + 15);
            } else if(i == HIST_X_TICK_COUNT) {
                verdanaTickLabel.drawString(str, histogramOrigin.x + i*tickXOff - strW, histogramOrigin.y + 15);
            } else {
                verdanaTickLabel.drawString(str, histogramOrigin.x + i*tickXOff - strW*0.5f, histogramOrigin.y + 15);
            }
        }

        float tickYOff = histogramHeight / (float)HIST_Y_TICK_COUNT;
        float tickYValOff = (histogramRange[1]-histogramRange[0]) / (float)HIST_Y_TICK_COUNT;
        for(int i=0;i<=HIST_Y_TICK_COUNT;i++) {
            string str;
            if(histLogScale) {
                float val;
                val = histogramRange[0] + i*tickYValOff;
                if(val == 0) {
                    val = 1;
                } else {
                    val = log(val);
                }
                str = to_string(val);
            } else {
                int val;
                val = (int)(histogramRange[0] + i*tickYValOff);
                str = to_string(val);
            }
            float strW = verdanaTickLabel.stringWidth(str);
            verdanaTickLabel.drawString(str, histogramOrigin.x - strW - 5, histogramOrigin.y - i*tickYOff);
        }

        // Draw histogram slider
        ofSetColor(sliderInactiveColor);
        ofSetLineWidth(SLIDER_WIDTH);
        ofDrawLine(SLIDER_HIST_X, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM,
                   SLIDER_HIST_X + SLIDER_LENGTH, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM);
        ofSetColor(sliderColor);
        ofSetLineWidth(SLIDER_ACTIVE_WIDTH);
        ofDrawLine(histSliderMinXPos, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM,
                   histSliderMaxXPos, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM);
        ofSetLineWidth(1);

        ofDrawCircle(histSliderMinXPos, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM, SLIDER_THUMB_RADIUS);
        ofDrawCircle(histSliderMaxXPos, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM, SLIDER_THUMB_RADIUS);

        ofSetColor(optionsLabelColor);
        float str_height = verdanaGenLabel.stringHeight(histSliderLabel);
        verdanaGenLabel.drawString(histSliderLabel, 10, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM + 0.2f * str_height);

        string minStr = to_string((int)range[0]);
        string maxStr = to_string((int)range[1]);
        float maxStrWidth = verdanaTickLabel.stringWidth(maxStr);
        verdanaTickLabel.drawString(minStr, SLIDER_HIST_X, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM - 10);
        verdanaTickLabel.drawString(maxStr, SLIDER_HIST_X + SLIDER_LENGTH - maxStrWidth, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM - 10);

        ofSetColor(sliderColor);
        minStr = to_string((int)ofMap(histSliderMinXPos - SLIDER_HIST_X, 0, SLIDER_LENGTH, range[0], range[1]));
        maxStr = to_string((int)ofMap(histSliderMaxXPos - SLIDER_HIST_X, 0, SLIDER_LENGTH, range[0], range[1]));
        maxStrWidth = verdanaSliderLabel.stringWidth(maxStr);
        verdanaSliderLabel.drawString(minStr, histSliderMinXPos, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM + 25);
        verdanaSliderLabel.drawString(maxStr, histSliderMaxXPos - maxStrWidth, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM + 25);
    ofPopMatrix();

    // Draw transfer functions
    ofPushMatrix();
        ofTranslate(tfX, tfY);
        // Draw canvas
        ofSetColor(255, 255, 255);
        ofDrawRectangle(0, 0, tfBoxWidth, tfBoxHeight);

        // Draw opacity tf
        ofSetColor(255, 255, 255);
        tfOpacity.draw(tfOpacityOrigin.x, tfOpacityOrigin.y-tfOpacityHeight);
        ofSetColor(tfThumbColor);
        ofNoFill();
        ofSetLineWidth(TF_THUMB_LINE_WIDTH);
        ofDrawCircle(tfOpacityOrigin.x,
                     tfOpacityOrigin.y - tfOpacityDataPoints.minVal.opacity*tfOpacityHeight, TF_THUMB_RADIUS);
        ofDrawCircle(tfOpacityOrigin.x + tfOpacityWidth,
                     tfOpacityOrigin.y - tfOpacityDataPoints.maxVal.opacity*tfOpacityHeight, TF_THUMB_RADIUS);
        for(int i=0;i<tfOpacityDataPoints.intermediateVals.size();i++) {
            OpacityTfValue tfVal = tfOpacityDataPoints.intermediateVals.at(i);
            float xPos = ofMap(tfVal.dataPoint, visibleTfRange[0], visibleTfRange[1],
                    tfOpacityOrigin.x, tfOpacityOrigin.x + tfOpacityWidth);
            ofDrawCircle(xPos, tfOpacityOrigin.y - tfVal.opacity*tfOpacityHeight, TF_THUMB_RADIUS);

            if(i == 0) {
                ofPoint start, end;
                start.x = tfOpacityOrigin.x + 0;
                start.y = tfOpacityOrigin.y - tfOpacityDataPoints.minVal.opacity*tfOpacityHeight;
                end.x = xPos;
                end.y = tfOpacityOrigin.y - tfVal.opacity*tfOpacityHeight;
                ofDrawLine(start, end);
            }

            if(i+1 < tfOpacityDataPoints.intermediateVals.size()) {
                ofPoint start, end;
                OpacityTfValue tfVal2 = tfOpacityDataPoints.intermediateVals.at(i+1);
                float xPos2 = ofMap(tfVal2.dataPoint, visibleTfRange[0], visibleTfRange[1],
                        tfOpacityOrigin.x, tfOpacityOrigin.x + tfOpacityWidth);
                start.x = xPos;
                start.y = tfOpacityOrigin.y - tfVal.opacity*tfOpacityHeight;
                end.x = xPos2;
                end.y = tfOpacityOrigin.y - tfVal2.opacity*tfOpacityHeight;
                ofDrawLine(start, end);
            }

            if(i+1 == tfOpacityDataPoints.intermediateVals.size()) {
                ofPoint start, end;
                start.x = xPos;
                start.y = tfOpacityOrigin.y - tfVal.opacity*tfOpacityHeight;
                end.x = tfOpacityOrigin.x + tfOpacityWidth;
                end.y = tfOpacityOrigin.y - tfOpacityDataPoints.maxVal.opacity*tfOpacityHeight;
                ofDrawLine(start, end);
            }
        }
        ofSetLineWidth(1);
        ofFill();

        // Draw color tf
        ofSetColor(255, 255, 255);
        tfColor.draw(tfColorOrigin.x, tfColorOrigin.y-tfColorHeight);
        ofSetColor(tfThumbColor);
        ofNoFill();
        ofSetLineWidth(TF_THUMB_LINE_WIDTH);
        ofDrawCircle(tfColorOrigin.x, tfColorOrigin.y - 0.5f*tfColorHeight, TF_THUMB_RADIUS);
        ofDrawCircle(tfColorOrigin.x + tfColorWidth, tfColorOrigin.y - 0.5f*tfColorHeight, TF_THUMB_RADIUS);
        for(int i=0;i<tfColorDataPoints.intermediateVals.size();i++) {
            float xPos = ofMap(tfColorDataPoints.intermediateVals.at(i).dataPoint,
                               visibleTfRange[0], visibleTfRange[1], tfColorOrigin.x, tfColorOrigin.x + tfColorWidth);
            ofDrawCircle(xPos, tfColorOrigin.y - 0.5f*tfColorHeight, TF_THUMB_RADIUS);
        }
        ofSetLineWidth(1);
        ofFill();

        // Draw tf slider
        ofSetColor(sliderInactiveColor);
        ofSetLineWidth(SLIDER_WIDTH);
        ofDrawLine(SLIDER_TF_X, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP,
                   SLIDER_TF_X + SLIDER_LENGTH, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP);
        ofSetColor(sliderColor);
        ofSetLineWidth(SLIDER_ACTIVE_WIDTH);
        ofDrawLine(tfSliderMinXPos, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP,
                   tfSliderMaxXPos, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP);
        ofSetLineWidth(1);

        ofDrawCircle(tfSliderMinXPos, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP, SLIDER_THUMB_RADIUS);
        ofDrawCircle(tfSliderMaxXPos, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP, SLIDER_THUMB_RADIUS);

        ofSetColor(optionsLabelColor);
        str_height = verdanaGenLabel.stringHeight(tfSliderLabel);
        verdanaGenLabel.drawString(tfSliderLabel, 10, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP + 0.2f * str_height);

        minStr = to_string((int)range[0]);
        maxStr = to_string((int)range[1]);
        maxStrWidth = verdanaTickLabel.stringWidth(maxStr);
        verdanaTickLabel.drawString(minStr, SLIDER_TF_X, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP - 10);
        verdanaTickLabel.drawString(maxStr, SLIDER_TF_X + SLIDER_LENGTH - maxStrWidth, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP - 10);

        ofSetColor(sliderColor);
        minStr = to_string((int)ofMap(tfSliderMinXPos - SLIDER_TF_X, 0, SLIDER_LENGTH, range[0], range[1]));
        maxStr = to_string((int)ofMap(tfSliderMaxXPos - SLIDER_TF_X, 0, SLIDER_LENGTH, range[0], range[1]));
        maxStrWidth = verdanaSliderLabel.stringWidth(maxStr);
        verdanaSliderLabel.drawString(minStr, tfSliderMinXPos, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP + 25);
        verdanaSliderLabel.drawString(maxStr, tfSliderMaxXPos - maxStrWidth, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP + 25);

        // Draw reset buttons
        ofSetColor(buttonBgColor);
        ofDrawRectangle(resetOpacityX, resetOpacityY, resetOpacityWidth, resetOpacityHeight);
        ofSetColor(buttonLabelColor);
        verdanaButtonLabel.drawString(resetOpacityLabel, resetOpacityX + 20, resetOpacityY + resetOpacityHeight - 10);

        ofSetColor(buttonBgColor);
        ofDrawRectangle(resetColorX, resetColorY, resetColorWidth, resetColorHeight);
        ofSetColor(buttonLabelColor);
        verdanaButtonLabel.drawString(resetColorLabel, resetColorX + 20, resetColorY + resetColorHeight - 10);

        // Draw above & below range option
        ofSetColor(optionsLabelColor);
        verdanaGenLabel.drawString(belowRangeLabel, belowRangeX + 1.2f*CHECKBOX_SIZE, belowRangeY);
        checkBox.draw(belowRangeX, belowRangeY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        if(useBelowRange) {
            checkMark.draw(belowRangeX, belowRangeY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        }

        verdanaGenLabel.drawString(aboveRangeLabel, aboveRangeX + 1.2f*CHECKBOX_SIZE, aboveRangeY);
        checkBox.draw(aboveRangeX, aboveRangeY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        if(useAboveRange) {
            checkMark.draw(aboveRangeX, aboveRangeY - 0.7f*CHECKBOX_SIZE, CHECKBOX_SIZE, CHECKBOX_SIZE);
        }

    ofPopMatrix();

    if (!startedVTK) {
        vr.setupRenderer();
        vr.startThread();
        startedVTK = true;
        updateColor();
        updateOpacity();
    }
}

void ofApp::exit() {
    vr.stopThread();
    vr.waitForThread();
}

void ofApp::updateColor() {
    if (startedVTK) {
        vr.color->RemoveAllPoints();
        ColorTfValue min = tfColorDataPoints.minVal;
        ColorTfValue max = tfColorDataPoints.maxVal;
        vr.color->AddRGBPoint(min.dataPoint, min.color.r, min.color.g, min.color.b);
        for(int i=0;i<tfColorDataPoints.intermediateVals.size();i++) {
            ColorTfValue tfVal = tfColorDataPoints.intermediateVals.at(i);
            vr.color->AddRGBPoint(tfVal.dataPoint, tfVal.color.r, tfVal.color.g, tfVal.color.b);
        }
        vr.color->AddRGBPoint(max.dataPoint, max.color.r, max.color.g, max.color.b);
        vr.update();
    }
}

void ofApp::updateOpacity()
{
    if (startedVTK) {
        vr.compositeOpacity->RemoveAllPoints();
        OpacityTfValue min = tfOpacityDataPoints.minVal;
        OpacityTfValue max = tfOpacityDataPoints.maxVal;
        vr.compositeOpacity->AddPoint(min.dataPoint, min.opacity);
        for(int i=0;i<tfOpacityDataPoints.intermediateVals.size();i++) {
            OpacityTfValue tfVal = tfOpacityDataPoints.intermediateVals.at(i);
            vr.compositeOpacity->AddPoint(tfVal.dataPoint, tfVal.opacity);
        }
        vr.compositeOpacity->AddPoint(max.dataPoint, max.opacity);
        vr.update();
    }
}

void ofApp::toggleShading(bool& shadeOn) {
    if (startedVTK) {
        if (shadeOn) {
            vr.volumeProperty->ShadeOn();
        } else {
            vr.volumeProperty->ShadeOff();
        }
        vr.update();
    }
}

void ofApp::resetColorTf()
{
    tfColorDataPoints.intermediateVals.clear();
    tfColorDataPoints.minVal.dataPoint = visibleTfRange[0];
    tfColorDataPoints.maxVal.dataPoint = visibleTfRange[1];
    tfColorDataPoints.minVal.color = divergingCS.startColor;
    tfColorDataPoints.maxVal.color = divergingCS.endColor;
    ColorTfValue midColorVal;
    midColorVal.dataPoint = visibleTfRange[0] + 0.5f*(visibleTfRange[1] - visibleTfRange[0]);
    midColorVal.color = divergingCS.midColor;
    tfColorDataPoints.intermediateVals.push_back(midColorVal);
    updateColor();
}

void ofApp::resetOpacityTf()
{
    tfOpacityDataPoints.intermediateVals.clear();
    tfOpacityDataPoints.minVal.dataPoint = visibleTfRange[0];
    tfOpacityDataPoints.minVal.opacity = 0.0f;
    tfOpacityDataPoints.maxVal.dataPoint = visibleTfRange[1];
    tfOpacityDataPoints.maxVal.opacity = 1.0f;
    OpacityTfValue midOpacityVal;
    midOpacityVal.dataPoint = visibleTfRange[0] + 0.5f*(visibleTfRange[1] - visibleTfRange[0]);
    midOpacityVal.opacity = 0.5f;
    tfOpacityDataPoints.intermediateVals.push_back(midOpacityVal);
    updateOpacity();
}

void ofApp::keyPressed(int key){

}

void ofApp::keyReleased(int key){

}

void ofApp::mouseMoved(int x, int y ){

}

void ofApp::mouseDragged(int x, int y, int button){
    if(mouseMode == MOUSE_MODE_CLICK) {
        mouseMode = MOUSE_MODE_DRAG;
    }

    if(mouseMode == MOUSE_MODE_DRAG) {
        int dataPoint;
        int minDiff;
        switch(thumbClicked) {
        case HIST_SLIDER_MAX_THUMB:
            histSliderMaxXPos = x;
            if(histSliderMaxXPos < histSliderMinXPos) {
                histSliderMaxXPos = histSliderMinXPos + 1;
            } else if(histSliderMaxXPos > SLIDER_HIST_X + SLIDER_LENGTH) {
                histSliderMaxXPos = SLIDER_HIST_X + SLIDER_LENGTH;
            }
            break;

        case HIST_SLIDER_MIN_THUMB:
            histSliderMinXPos = x;
            if(histSliderMinXPos > histSliderMaxXPos) {
                histSliderMinXPos = histSliderMaxXPos - 1;
            } else if(histSliderMinXPos < SLIDER_HIST_X) {
                histSliderMinXPos = SLIDER_HIST_X;
            }
            break;

        case TF_SLIDER_MAX_THUMB:
            tfSliderMaxXPos = x;
            if(tfSliderMaxXPos < tfSliderMinXPos) {
                tfSliderMaxXPos = tfSliderMinXPos + 1;
            } else if(tfSliderMaxXPos > SLIDER_TF_X + SLIDER_LENGTH) {
                tfSliderMaxXPos = SLIDER_TF_X + SLIDER_LENGTH;
            }
            break;

        case TF_SLIDER_MIN_THUMB:
            tfSliderMinXPos = x;
            if(tfSliderMinXPos > tfSliderMaxXPos) {
                tfSliderMinXPos = tfSliderMaxXPos - 1;
            } else if(tfSliderMinXPos < SLIDER_TF_X) {
                tfSliderMinXPos = SLIDER_TF_X;
            }
            break;

        case TF_COLOR_THUMB:
            dataPoint = (int)ofMap(x-tfX-tfColorOrigin.x,
                               0, tfColorWidth, visibleTfRange[0], visibleTfRange[1]);
            minDiff = 0.01f*(visibleTfRange[1] - visibleTfRange[0]);
            if(tfColorClickIndex == 0 && tfColorDataPoints.intermediateVals.size() == 1) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                }
            } else if(tfColorClickIndex == 0) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint - minDiff;
                }
            } else if(tfColorClickIndex == tfColorDataPoints.intermediateVals.size()-1) {
                if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                } else if(dataPoint < tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint + minDiff;
                }
            } else {
                if(dataPoint < tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint + minDiff;
                } else if(dataPoint > tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint - minDiff;
                }
            }
            tfColorDataPoints.intermediateVals.at(tfColorClickIndex).dataPoint = dataPoint;
            drawState = DRAW_STATE_INVALIDATE;
            break;

        case TF_OPACITY_THUMB:
            dataPoint = (int)ofMap(x-tfX-tfOpacityOrigin.x,
                               0, tfOpacityWidth, visibleTfRange[0], visibleTfRange[1]);
            minDiff = 0.01f*(visibleTfRange[1] - visibleTfRange[0]);
            if(tfOpacityClickIndex == 0 && tfOpacityDataPoints.intermediateVals.size() == 1) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                }
            } else if(tfOpacityClickIndex == 0) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint - minDiff;
                }
            } else if(tfOpacityClickIndex == tfOpacityDataPoints.intermediateVals.size()-1) {
                if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                } else if(dataPoint < tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint + minDiff;
                }
            } else {
                if(dataPoint < tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint + minDiff;
                } else if(dataPoint > tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint - minDiff;
                }
            }
            tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex).dataPoint = dataPoint;
            float opacity = (tfOpacityOrigin.y - (y-tfY))/tfOpacityHeight;
            if(opacity < 0) {
                opacity = 0.0f;
            } else if(opacity > 1) {
                opacity = 1.0f;
            }
            tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex).opacity = opacity;
            drawState = DRAW_STATE_INVALIDATE;
            break;
        }
    }
}

void ofApp::mousePressed(int x, int y, int button){
    if(isInsideRect(x-histogramX, y-histogramY,
                    histLogX, histLogY - 0.7f*CHECKBOX_SIZE, histLogX + CHECKBOX_SIZE, histLogY - 0.7f*CHECKBOX_SIZE + CHECKBOX_SIZE)) {
        histLogScale = !histLogScale;
        return;
    }

    if(isInsideRect(x-tfX, y-tfY,
                    belowRangeX, belowRangeY - 0.7f*CHECKBOX_SIZE, belowRangeX + CHECKBOX_SIZE, belowRangeY - 0.7f*CHECKBOX_SIZE + CHECKBOX_SIZE)) {
        useBelowRange = !useBelowRange;
        if(useBelowRange)
            vr.color->UseBelowRangeColorOn();
        else
            vr.color->UseBelowRangeColorOff();
        vr.update();
        return;
    }

    if(isInsideRect(x-tfX, y-tfY,
                    aboveRangeX, aboveRangeY - 0.7f*CHECKBOX_SIZE, aboveRangeX + CHECKBOX_SIZE, aboveRangeY - 0.7f*CHECKBOX_SIZE + CHECKBOX_SIZE)) {
        useAboveRange = !useAboveRange;
        if(useAboveRange)
            vr.color->UseAboveRangeColorOn();
        else
            vr.color->UseAboveRangeColorOff();
        vr.update();
        return;
    }

    if(isInsideRect(x-tfX, y-tfY,
                    tfColorOrigin.x, tfColorOrigin.y - tfColorHeight, tfColorOrigin.x + tfColorWidth, tfColorOrigin.y)) {
        for(int i=0;i<tfColorDataPoints.intermediateVals.size();i++) {
            float xPos = ofMap(tfColorDataPoints.intermediateVals.at(i).dataPoint,
                               visibleTfRange[0], visibleTfRange[1], tfColorOrigin.x, tfColorOrigin.x + tfColorWidth);
            if(isInsideRect(x-tfX, y-tfY,
                            xPos - TF_THUMB_RADIUS, tfColorOrigin.y - tfColorHeight,
                            xPos + TF_THUMB_RADIUS, tfColorOrigin.y)) {
                tfColorClickIndex = i;
                break;
            }
        }
        if(tfColorClickIndex == -1) {
            // new color data point
            int dataPoint = (int)ofMap(x-tfX-tfColorOrigin.x,
                               0, tfColorWidth, visibleTfRange[0], visibleTfRange[1]);
            ColorTfValue tfVal;
            tfVal.dataPoint = dataPoint;
            int index = 0;
            int i=0;
            for(;i<tfColorDataPoints.intermediateVals.size();i++) {
                if(tfVal.dataPoint < tfColorDataPoints.intermediateVals.at(i).dataPoint) {
                    break;
                }
            }
            index = i;

            ColorTfValue startTfVal, endTfVal;
            if(index == 0) {
                startTfVal = tfColorDataPoints.minVal;
            } else {
                startTfVal = tfColorDataPoints.intermediateVals.at(index-1);
            }
            if(index == tfColorDataPoints.intermediateVals.size()) {
                endTfVal = tfColorDataPoints.maxVal;
            } else {
                endTfVal = tfColorDataPoints.intermediateVals.at(index);
            }
            float frac = (tfVal.dataPoint-startTfVal.dataPoint)/(float)(endTfVal.dataPoint - startTfVal.dataPoint);
            tfVal.color.r = startTfVal.color.r + frac*(endTfVal.color.r - startTfVal.color.r);
            tfVal.color.g = startTfVal.color.g + frac*(endTfVal.color.g - startTfVal.color.g);
            tfVal.color.b = startTfVal.color.b + frac*(endTfVal.color.g - startTfVal.color.b);

            if(index < tfColorDataPoints.intermediateVals.size()) {
                tfColorDataPoints.intermediateVals.insert(tfColorDataPoints.intermediateVals.begin()+index, tfVal);
            } else {
                tfColorDataPoints.intermediateVals.push_back(tfVal);
            }

            tfColorClickIndex = index;
        }
        mouseMode = MOUSE_MODE_CLICK;
        thumbClicked = TF_COLOR_THUMB;
        return;
    }

    if(isInsideRect(x-tfX, y-tfY,
                    tfOpacityOrigin.x, tfOpacityOrigin.y - tfOpacityHeight, tfOpacityOrigin.x + tfOpacityWidth, tfOpacityOrigin.y)) {
        for(int i=0;i<tfOpacityDataPoints.intermediateVals.size();i++) {
            float xPos = ofMap(tfOpacityDataPoints.intermediateVals.at(i).dataPoint,
                               visibleTfRange[0], visibleTfRange[1], tfOpacityOrigin.x, tfOpacityOrigin.x + tfOpacityWidth);
            float yPos = tfOpacityOrigin.y - tfOpacityDataPoints.intermediateVals.at(i).opacity * tfOpacityHeight;
            if(isInsideRect(x-tfX, y-tfY,
                            xPos - TF_THUMB_RADIUS, yPos - TF_THUMB_RADIUS,
                            xPos + TF_THUMB_RADIUS, yPos + TF_THUMB_RADIUS)) {
                tfOpacityClickIndex = i;
                break;
            }
        }
        if(tfOpacityClickIndex == -1) {
            // new opacity data point
            int dataPoint = (int)ofMap(x-tfX-tfOpacityOrigin.x,
                               0, tfOpacityWidth, visibleTfRange[0], visibleTfRange[1]);
            OpacityTfValue tfVal;
            tfVal.dataPoint = dataPoint;
            tfVal.opacity = (tfOpacityOrigin.y - (y-tfY))/tfOpacityHeight;

            int index = 0;
            int i=0;
            for(;i<tfOpacityDataPoints.intermediateVals.size();i++) {
                if(tfVal.dataPoint < tfOpacityDataPoints.intermediateVals.at(i).dataPoint) {
                    break;
                }
            }
            index = i;
            if(index < tfOpacityDataPoints.intermediateVals.size()) {
                tfOpacityDataPoints.intermediateVals.insert(tfOpacityDataPoints.intermediateVals.begin()+index, tfVal);
            } else {
                tfOpacityDataPoints.intermediateVals.push_back(tfVal);
            }

            tfOpacityClickIndex = index;
        }
        mouseMode = MOUSE_MODE_CLICK;
        thumbClicked = TF_OPACITY_THUMB;
        return;
    }

    if(thumbClicked == SLIDER_NO_THUMB) {
        if(isInsideRect(x-histogramX, y-histogramY,
                          histSliderMaxXPos - SLIDER_THUMB_RADIUS, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM + SLIDER_THUMB_RADIUS,
                          histSliderMaxXPos + SLIDER_THUMB_RADIUS, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM - SLIDER_THUMB_RADIUS)) {
            histSliderMaxXPos = x;
            mouseMode = MOUSE_MODE_CLICK;
            thumbClicked = HIST_SLIDER_MAX_THUMB;
            return;
        }

        if(isInsideRect(x-histogramX, y-histogramY,
                          histSliderMinXPos - SLIDER_THUMB_RADIUS, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM + SLIDER_THUMB_RADIUS,
                          histSliderMinXPos + SLIDER_THUMB_RADIUS, histogramBoxHeight - SLIDER_HIST_Y_FROM_BOTTOM - SLIDER_THUMB_RADIUS)) {
            histSliderMinXPos = x;
            mouseMode = MOUSE_MODE_CLICK;
            thumbClicked = HIST_SLIDER_MIN_THUMB;
            return;
        }

        if(isInsideRect(x-tfX, y-tfY,
                          tfSliderMaxXPos - SLIDER_THUMB_RADIUS, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP + SLIDER_THUMB_RADIUS,
                          tfSliderMaxXPos + SLIDER_THUMB_RADIUS, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP - SLIDER_THUMB_RADIUS)) {
            tfSliderMaxXPos = x;
            mouseMode = MOUSE_MODE_CLICK;
            thumbClicked = TF_SLIDER_MAX_THUMB;
            return;
        }

        if(isInsideRect(x-tfX, y-tfY,
                          tfSliderMinXPos - SLIDER_THUMB_RADIUS, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP + SLIDER_THUMB_RADIUS,
                          tfSliderMinXPos + SLIDER_THUMB_RADIUS, tfColorOrigin.y + SLIDER_TF_Y_FROM_TOP - SLIDER_THUMB_RADIUS)) {
            tfSliderMinXPos = x;
            mouseMode = MOUSE_MODE_CLICK;
            thumbClicked = TF_SLIDER_MIN_THUMB;
            return;
        }
    }

    if(isInsideRect(x-tfX, y-tfY, resetColorX, resetColorY, resetColorX+resetColorWidth, resetColorY+resetColorHeight)) {
        resetColorTf();
        return;
    }

    if(isInsideRect(x-tfX, y-tfY, resetOpacityX, resetOpacityY, resetOpacityX+resetOpacityWidth, resetOpacityY+resetOpacityHeight)) {
        resetOpacityTf();
        return;
    }

}

void ofApp::mouseReleased(int x, int y, int button){
    if(mouseMode == MOUSE_MODE_DRAG || mouseMode == MOUSE_MODE_CLICK) {
        if(thumbClicked == HIST_SLIDER_MAX_THUMB || thumbClicked == HIST_SLIDER_MIN_THUMB) {
            switch(thumbClicked) {
            case HIST_SLIDER_MAX_THUMB:
                histSliderMaxXPos = x;
                if(histSliderMaxXPos < histSliderMinXPos) {
                    histSliderMaxXPos = histSliderMinXPos + 1;
                } else if(histSliderMaxXPos > SLIDER_HIST_X + SLIDER_LENGTH) {
                    histSliderMaxXPos = SLIDER_HIST_X + SLIDER_LENGTH;
                }
                break;

            case HIST_SLIDER_MIN_THUMB:
                histSliderMinXPos = x;
                if(histSliderMinXPos > histSliderMaxXPos) {
                    histSliderMinXPos = histSliderMaxXPos - 1;
                } else if(histSliderMinXPos < SLIDER_HIST_X) {
                    histSliderMinXPos = SLIDER_HIST_X;
                }
                break;
            }
            visibleRange[0] = ofMap(histSliderMinXPos - SLIDER_HIST_X, 0, SLIDER_LENGTH, range[0], range[1]);
            visibleRange[1] = ofMap(histSliderMaxXPos - SLIDER_HIST_X, 0, SLIDER_LENGTH, range[0], range[1]);
        } else if(thumbClicked == TF_SLIDER_MAX_THUMB || thumbClicked == TF_SLIDER_MIN_THUMB) {
            switch(thumbClicked) {
            case TF_SLIDER_MAX_THUMB:
                tfSliderMaxXPos = x;
                if(tfSliderMaxXPos < tfSliderMinXPos) {
                    tfSliderMaxXPos = tfSliderMinXPos + 1;
                } else if(tfSliderMaxXPos > SLIDER_TF_X + SLIDER_LENGTH) {
                    tfSliderMaxXPos = SLIDER_TF_X + SLIDER_LENGTH;
                }
                break;

            case TF_SLIDER_MIN_THUMB:
                tfSliderMinXPos = x;
                if(tfSliderMinXPos > tfSliderMaxXPos) {
                    tfSliderMinXPos = tfSliderMaxXPos - 1;
                } else if(tfSliderMinXPos < SLIDER_TF_X) {
                    tfSliderMinXPos = SLIDER_TF_X;
                }
                break;
            }
            visibleTfRange[0] = ofMap(tfSliderMinXPos - SLIDER_TF_X, 0, SLIDER_LENGTH, range[0], range[1]);
            visibleTfRange[1] = ofMap(tfSliderMaxXPos - SLIDER_TF_X, 0, SLIDER_LENGTH, range[0], range[1]);
            double newRange[2] = {(double)visibleTfRange[0], (double)visibleTfRange[1]};
            vr.color->AdjustRange(newRange);

            resetColorTf();
            resetOpacityTf();
        } else if(thumbClicked == TF_COLOR_THUMB) {
            int dataPoint = (int)ofMap(x-tfX-tfColorOrigin.x,
                               0, tfColorWidth, visibleTfRange[0], visibleTfRange[1]);
            int minDiff = 0.01f*(visibleTfRange[1] - visibleTfRange[0]);
            if(tfColorClickIndex == 0 && tfColorDataPoints.intermediateVals.size() == 1) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                }
            } else if(tfColorClickIndex == 0) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint - minDiff;
                }
            } else if(tfColorClickIndex == tfColorDataPoints.intermediateVals.size()-1) {
                if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                } else if(dataPoint < tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint + minDiff;
                }
            } else {
                if(dataPoint < tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex-1).dataPoint + minDiff;
                } else if(dataPoint > tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint) {
                    dataPoint = tfColorDataPoints.intermediateVals.at(tfColorClickIndex+1).dataPoint - minDiff;
                }
            }
            tfColorDataPoints.intermediateVals.at(tfColorClickIndex).dataPoint = dataPoint;

            updateColor();
            tfColorClickIndex = -1;
        } else if(thumbClicked == TF_OPACITY_THUMB) {
            int dataPoint = (int)ofMap(x-tfX-tfOpacityOrigin.x,
                               0, tfOpacityWidth, visibleTfRange[0], visibleTfRange[1]);
            int minDiff = 0.01f*(visibleTfRange[1] - visibleTfRange[0]);
            if(tfOpacityClickIndex == 0 && tfOpacityDataPoints.intermediateVals.size() == 1) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                }
            } else if(tfOpacityClickIndex == 0) {
                if(dataPoint < visibleTfRange[0]) {
                    dataPoint = visibleRange[0] + minDiff;
                } else if(dataPoint > tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint - minDiff;
                }
            } else if(tfOpacityClickIndex == tfOpacityDataPoints.intermediateVals.size()-1) {
                if(dataPoint > visibleTfRange[1]) {
                    dataPoint = visibleRange[1] - minDiff;
                } else if(dataPoint < tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint + minDiff;
                }
            } else {
                if(dataPoint < tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex-1).dataPoint + minDiff;
                } else if(dataPoint > tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint) {
                    dataPoint = tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex+1).dataPoint - minDiff;
                }
            }
            tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex).dataPoint = dataPoint;
            float opacity = (tfOpacityOrigin.y - (y-tfY))/tfOpacityHeight;
            if(opacity < 0) {
                opacity = 0.0f;
            } else if(opacity > 1) {
                opacity = 1.0f;
            }
            tfOpacityDataPoints.intermediateVals.at(tfOpacityClickIndex).opacity = opacity;

            updateOpacity();
            tfOpacityClickIndex = -1;
        }

        thumbClicked = SLIDER_NO_THUMB;
        mouseMode = MOUSE_MODE_INACTIVE;
        drawState = DRAW_STATE_INVALIDATE;
    }
}

void ofApp::mouseEntered(int x, int y){

}

void ofApp::mouseExited(int x, int y){

}

void ofApp::windowResized(int w, int h){

}

void ofApp::gotMessage(ofMessage msg){

}

void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
