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

int ofApp::compare(ofVec3f a, ofVec3f b)
{
    if(a.x < b.x)
        return -1;
    else if(a.x > b.x)
        return 1;

    if(a.y < b.y)
        return -1;
    else if(a.y > b.y)
        return 1;

    if(a.z < b.z)
        return -1;
    else if(a.z > b.z)
        return 1;

    return 1;
}

ofVec3f ofApp::interpolateVertex(pair<ofVec3f, float> a, pair<ofVec3f, float> b)
{
    ofVec3f iPoint;

    if(compare(b.first, a.first) == -1) {
        // b < a
        pair<ofVec3f, float> temp;
        temp = a;
        a = b;
        b = temp;
    }

    if(fabs(b.second-a.second) > 0.0001) {
        iPoint.x = a.first.x + (int)ceil((b.first.x - a.first.x)/(b.second-a.second)*(isoValue-a.second));
        iPoint.y = a.first.y + (int)ceil((b.first.y - a.first.y)/(b.second-a.second)*(isoValue-a.second));
        iPoint.z = a.first.z + (int)ceil((b.first.z - a.first.z)/(b.second-a.second)*(isoValue-a.second));
    } else {
        iPoint = a.first;
    }

    return iPoint;
}

void ofApp::addFace(ofVec3f a, ofVec3f b, ofVec3f c)
{
    ofVec3f normal = ((b - a).cross(c - a)).normalize();
    mesh.addNormal(normal);
    mesh.addVertex(a);
    mesh.addNormal(normal);
    mesh.addVertex(b);
    mesh.addNormal(normal);
    mesh.addVertex(c);
}

void ofApp::setupMesh()
{
    dims = imageReader->getDims();
    range = imageReader->getRange();
    data = imageReader->getData();

    isoValue = 0.5f;

    xPos = dims[0]/2.0f;
    yPos = dims[1]/2.0f;
    zPos = dims[2]/2.0f;

    bboxPos = ofVec3f(0,0,0);
    bboxSize = ofVec3f(dims[0]-1,dims[1]-1,dims[2]-1);
    bboxPos = 0.5*(bboxSize-bboxPos);

    pointLight.setPosition(xPos, yPos, zPos);

    vboMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    sliderXPos = SLIDER_X + SLIDER_LENGTH * isoValue;

    int size = legendWidth*legendHeight*3;
    float *legendData = new float[size];
    int k=0;
    for(int i=0;i<legendHeight;i++) {
        for(int j=0;j<legendWidth;j++) {
            legendData[k] = (MIN_RED - (1 - i/(float)legendHeight)*(MIN_RED - MAX_RED))/255.0f;
            legendData[k+1] = (MIN_GREEN - (1 - i/(float)legendHeight)*(MIN_GREEN - MAX_GREEN))/255.0f;
            legendData[k+2] = (MIN_BLUE - (1 - i/(float)legendHeight)*(MIN_BLUE - MAX_BLUE))/255.0f;
            k+=3;
        }
    }
    legend.setFromPixels(legendData, legendWidth, legendHeight, OF_IMAGE_COLOR);

    updateIsoSurface();

    drawState = DRAW_STATE_INVALIDATE;
}

void ofApp::clearMesh()
{
    dims[0] = dims[1] = dims[2] = -1;
    xPos = yPos = 0;
    vboMesh.clear();
    mesh.clear();
    cam.reset();
    isoValue = 0.5f;
    sliderXPos = SLIDER_X + 0.5f*SLIDER_LENGTH;
    showWireframe = true;
    enableLight = false;
}

void ofApp::updateIsoSurface()
{
    mesh.clear();
    int i=0;
    for(int z=0;(z+CUBE_SIZE)<dims[2];z+=CUBE_SIZE) {
        for(int y=0;(y+CUBE_SIZE)<dims[1];y+=CUBE_SIZE) {
            for(int x=0;(x+CUBE_SIZE)<dims[0];x+=CUBE_SIZE) {
                pair<ofVec3f, float> v[8];
                v[0].first.set(x, y, z);
                v[1].first.set(x+CUBE_SIZE, y, z);
                v[2].first.set(x+CUBE_SIZE, y, z+CUBE_SIZE);
                v[3].first.set(x, y, z+CUBE_SIZE);
                v[4].first.set(x, y+CUBE_SIZE, z);
                v[5].first.set(x+CUBE_SIZE, y+CUBE_SIZE, z);
                v[6].first.set(x+CUBE_SIZE, y+CUBE_SIZE, z+CUBE_SIZE);
                v[7].first.set(x, y+CUBE_SIZE, z+CUBE_SIZE);

                v[0].second = data[(int)(v[0].first.z*dims[1]*dims[0] + v[0].first.y*dims[0] + v[0].first.x)];
                v[1].second = data[(int)(v[1].first.z*dims[1]*dims[0] + v[1].first.y*dims[0] + v[1].first.x)];
                v[2].second = data[(int)(v[2].first.z*dims[1]*dims[0] + v[2].first.y*dims[0] + v[2].first.x)];
                v[3].second = data[(int)(v[3].first.z*dims[1]*dims[0] + v[3].first.y*dims[0] + v[3].first.x)];
                v[4].second = data[(int)(v[4].first.z*dims[1]*dims[0] + v[4].first.y*dims[0] + v[4].first.x)];
                v[5].second = data[(int)(v[5].first.z*dims[1]*dims[0] + v[5].first.y*dims[0] + v[5].first.x)];
                v[6].second = data[(int)(v[6].first.z*dims[1]*dims[0] + v[6].first.y*dims[0] + v[6].first.x)];
                v[7].second = data[(int)(v[7].first.z*dims[1]*dims[0] + v[7].first.y*dims[0] + v[7].first.x)];

                int edgeTableIndex = 0;
                if(v[0].second < isoValue) edgeTableIndex |= 1;
                if(v[1].second < isoValue) edgeTableIndex |= 2;
                if(v[2].second < isoValue) edgeTableIndex |= 4;
                if(v[3].second < isoValue) edgeTableIndex |= 8;
                if(v[4].second < isoValue) edgeTableIndex |= 16;
                if(v[5].second < isoValue) edgeTableIndex |= 32;
                if(v[6].second < isoValue) edgeTableIndex |= 64;
                if(v[7].second < isoValue) edgeTableIndex |= 128;

                int edgeMask = EDGE_TABLE[edgeTableIndex];

                if(edgeMask == 0) {
                    continue;
                }

                ofVec3f vertices[12];

                if(edgeMask & 1) {
                   vertices[0] = interpolateVertex(v[0], v[1]);
                }
                if(edgeMask & 2) {
                   vertices[1] = interpolateVertex(v[1], v[2]);
                }
                if(edgeMask & 4) {
                   vertices[2] = interpolateVertex(v[2], v[3]);
                }
                if(edgeMask & 8) {
                   vertices[3] = interpolateVertex(v[3], v[0]);
                }
                if(edgeMask & 16) {
                   vertices[4] = interpolateVertex(v[4], v[5]);
                }
                if(edgeMask & 32) {
                   vertices[5] = interpolateVertex(v[5], v[6]);
                }
                if(edgeMask & 64) {
                   vertices[6] = interpolateVertex(v[6], v[7]);
                }
                if(edgeMask & 128) {
                   vertices[7] = interpolateVertex(v[7], v[4]);
                }
                if(edgeMask & 256) {
                   vertices[8] = interpolateVertex(v[0], v[4]);
                }
                if(edgeMask & 512) {
                   vertices[9] = interpolateVertex(v[1], v[5]);
                }
                if(edgeMask & 1024) {
                   vertices[10] = interpolateVertex(v[2], v[6]);
                }
                if(edgeMask & 2048) {
                   vertices[11] = interpolateVertex(v[3], v[7]);
                }

                for(int k=0; TRIANGLE_TABLE[edgeTableIndex][k] != -1;k+=3) {
                    addFace(vertices[TRIANGLE_TABLE[edgeTableIndex][k]],
                            vertices[TRIANGLE_TABLE[edgeTableIndex][k+1]],
                            vertices[TRIANGLE_TABLE[edgeTableIndex][k+2]]);
                }
            }
        }
    }

    vboMesh = mesh;
}

ofApp::ofApp()
{
    imageReader = new ImageReader();
    fileLoaded = false;
    mouseMode = MOUSE_MODE_INACTIVE;
    isoValue = 0.5f;
    sliderXPos = SLIDER_X + SLIDER_LENGTH * isoValue;
    showWireframe = true;
    enableLight = false;
}

void ofApp::setup()
{
    //old OF default is 96 - but this results in fonts looking larger than in other programs.
    ofTrueTypeFont::setGlobalDpi(72);
    ofSetFrameRate(0);
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    ofSetSmoothLighting(true);
    ofSetFullscreen(true);

    screenHeight = ofGetScreenHeight();
    screenWidth = ofGetScreenWidth();

    ofBackground(140, 140, 140);

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

    hydrogenLabel = string("Hydrogen");
    toothLabel = string("Tooth");
    updateLabel = string("Update");
    wireframeLabel = string("Wireframe");
    lightLabel = string("Enable light");

    checkBox.load("checkbox.png");
    checkMark.load("checkmark.png");

    buttonBgColor.set(66, 134, 244);
    buttonLabelColor.set(255, 255, 255);

    tickLabelColor.set(255, 255, 255);

    sliderColor.set(244, 200, 66);
    sliderLabelColor.set(244, 200, 66);

    legendWidth = 20;
    legendHeight = (int)(0.6f*screenHeight);
    legendXFromRight = 0.08f*screenWidth;
    legendY = 0.2f*screenHeight;

    isoValue = 0.5f;

    sliderXPos = SLIDER_X + SLIDER_LENGTH * isoValue;

    pointLight.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
    pointLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    lightColor.setBrightness(180.f);
    lightColor.setSaturation(150.f);
    lightColor.setHue(30.0f);

    drawState = DRAW_STATE_INVALIDATE;
}

void ofApp::update()
{
    if(drawState == DRAW_STATE_INVALIDATE) {
        float str_width = verdanaButtonLabel.stringWidth(hydrogenLabel);
        float str_height = verdanaButtonLabel.stringHeight(hydrogenLabel);

        hydrogenRectStart.x = BUTTON_X;
        hydrogenRectStart.y = BUTTON_Y;
        hydrogenWidth = str_width + 2 * BUTTON_OFFSET;
        hydrogenHeight = str_height + 2 * BUTTON_OFFSET;

        str_width = verdanaButtonLabel.stringWidth(toothLabel);

        toothRectStart.x = BUTTON_X + hydrogenRectStart.x + hydrogenWidth + 2 * BUTTON_OFFSET;
        toothRectStart.y = BUTTON_Y;
        toothWidth = str_width + 2 * BUTTON_OFFSET;
        toothHeight = str_height + 2 * BUTTON_OFFSET;

        updateRectStart.x = UPDATE_BUTTON_X;
        updateRectStart.y = screenHeight - UPDATE_BUTTON_Y_FROM_BOTTOM;

        wireframeRectStart.x = WIREFRAME_CHECKBOX_X;
        wireframeRectStart.y = screenHeight - WIREFRAME_CHECKBOX_Y_FROM_BOTTOM;

        lightRectStart.x = LIGHT_CHECKBOX_X;
        lightRectStart.y = screenHeight - LIGHT_CHECKBOX_Y_FROM_BOTTOM;

        pointLight.setDiffuseColor(lightColor);

        drawState = DRAW_STATE_STABLE;
    }
}

void ofApp::draw()
{
    // Draw button background
    ofSetColor(buttonBgColor);
    ofDrawRectangle(hydrogenRectStart.x, hydrogenRectStart.y, hydrogenWidth, hydrogenHeight);
    ofDrawRectangle(toothRectStart.x, toothRectStart.y, toothWidth, toothHeight);

    // Draw button label
    ofSetColor(buttonLabelColor);
    verdanaButtonLabel.drawString(hydrogenLabel,
        hydrogenRectStart.x + BUTTON_OFFSET, BUTTON_Y + hydrogenHeight - BUTTON_OFFSET);
    verdanaButtonLabel.drawString(toothLabel,
        toothRectStart.x + BUTTON_OFFSET, BUTTON_Y + toothHeight - BUTTON_OFFSET);

    if(fileLoaded) {
        // Draw slice button
        ofSetColor(buttonBgColor);
        ofDrawRectangle(updateRectStart.x, updateRectStart.y, UPDATE_BUTTON_WIDTH, UPDATE_BUTTON_HEIGHT);
        ofSetColor(buttonLabelColor);
        string sLabel = updateLabel;
        verdanaButtonLabel.drawString(sLabel,
                                        updateRectStart.x + BUTTON_OFFSET,
                                        updateRectStart.y + UPDATE_BUTTON_HEIGHT - BUTTON_OFFSET);

        // Draw slider & color chooser
        ofSetColor(sliderColor);
        ofSetLineWidth(SLIDER_WIDTH);
        ofDrawLine(SLIDER_X,
                   screenHeight - SLIDER_Y_FROM_BOTTOM,
                   SLIDER_X + SLIDER_LENGTH, screenHeight - SLIDER_Y_FROM_BOTTOM);
        ofDrawCircle(sliderXPos, screenHeight - SLIDER_Y_FROM_BOTTOM, SLIDER_THUMB_RADIUS);

        // draw min-max values
        ofSetColor(buttonLabelColor);
        string minStr = to_string(range[0]);
        string maxStr = to_string(range[1]);
        float maxStrWidth = verdanaTickLabel.stringWidth(maxStr);
        verdanaTickLabel.drawString(minStr, SLIDER_X, screenHeight - SLIDER_Y_FROM_BOTTOM - 10);
        verdanaTickLabel.drawString(maxStr, SLIDER_X + SLIDER_LENGTH - maxStrWidth, screenHeight - SLIDER_Y_FROM_BOTTOM - 10);

        // draw isoVal
        string isoValueStr = to_string(range[0] +
                ((sliderXPos - SLIDER_X)/(float)SLIDER_LENGTH) * (range[1]-range[0]));
        float isoValStrHeight = verdanaSliderLabel.stringHeight(isoValueStr);
        float isoValStrWidth = verdanaSliderLabel.stringWidth(isoValueStr);
        ofSetColor(sliderLabelColor);
        float sliderLabelXPos = sliderXPos - isoValStrWidth/2.0f;
        if(sliderLabelXPos < SLIDER_X) {
            sliderLabelXPos = SLIDER_X;
        } else if(sliderLabelXPos + isoValStrWidth > SLIDER_X + SLIDER_LENGTH) {
            sliderLabelXPos = (SLIDER_X + SLIDER_LENGTH) - isoValStrWidth;
        }
        verdanaSliderLabel.drawString(isoValueStr, sliderLabelXPos,
                                      screenHeight - SLIDER_Y_FROM_BOTTOM + 10 + isoValStrHeight);

        checkBox.draw(wireframeRectStart.x, wireframeRectStart.y, WIREFRAME_CHECKBOX_SIZE, WIREFRAME_CHECKBOX_SIZE);
        if(showWireframe) {
            checkMark.draw(wireframeRectStart.x, wireframeRectStart.y, WIREFRAME_CHECKBOX_SIZE, WIREFRAME_CHECKBOX_SIZE);
        }

        checkBox.draw(lightRectStart.x, lightRectStart.y, LIGHT_CHECKBOX_SIZE, LIGHT_CHECKBOX_SIZE);
        if(enableLight) {
            checkMark.draw(lightRectStart.x, lightRectStart.y, LIGHT_CHECKBOX_SIZE, LIGHT_CHECKBOX_SIZE);
        }

        ofSetColor(buttonLabelColor);
        verdanaButtonLabel.drawString(wireframeLabel,
                                        wireframeRectStart.x + WIREFRAME_CHECKBOX_SIZE + WIREFRAME_CHECKBOX_MARGIN,
                                        wireframeRectStart.y + 30);
        verdanaButtonLabel.drawString(lightLabel,
                                        lightRectStart.x + LIGHT_CHECKBOX_SIZE + LIGHT_CHECKBOX_MARGIN,
                                        lightRectStart.y + 30);

        ofEnableDepthTest();
        cam.begin();
        ofPushMatrix();
            ofTranslate(-xPos, -yPos, -zPos);

            ofSetColor(255,255,255);
            ofNoFill();
            ofDrawBox(bboxPos,bboxSize[0],bboxSize[1],bboxSize[2]);
            ofFill();

            if(enableLight) {
                ofEnableLighting();
                pointLight.enable();
            }
            if(showWireframe) {
                ofSetColor(lineColor);
                vboMesh.draw(OF_MESH_WIREFRAME);
            }

            ofColor color;
            color.set(MIN_RED - isoValue*(MIN_RED-MAX_RED),
                      MIN_GREEN - isoValue*(MIN_GREEN-MAX_GREEN),
                      MIN_BLUE - isoValue*(MIN_BLUE-MAX_BLUE));
            ofSetColor(color);
            vboMesh.draw();

            if(enableLight) {
                ofDisableLighting();
            }
        ofPopMatrix();
        cam.end();
        ofDisableDepthTest();

        ofSetColor(255, 255, 255);
        legend.draw(screenWidth - legendXFromRight, legendY);

        ofSetColor(tickLabelColor);
        verdanaTickLabel.drawString(minStr,
                                      screenWidth - legendXFromRight + legendWidth + 10,
                                      legendY + legendHeight + 5);
        verdanaTickLabel.drawString(maxStr,
                                      screenWidth - legendXFromRight + legendWidth + 10,
                                      legendY + 5);

        float tick_offset = legendHeight/(float)(TICK_COUNT+1);
        for(int i=1;i<=TICK_COUNT;i++) {
            string tickStr = to_string(range[0] + i*(range[1] - range[0]));
            verdanaTickLabel.drawString(tickStr,
                                          screenWidth - legendXFromRight + legendWidth + 10,
                                          legendY + legendHeight - i*tick_offset + 5);
        }
    }
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
    if (isInsideRect(x, y,
                       hydrogenRectStart.x, hydrogenRectStart.y,
                       hydrogenRectStart.x + hydrogenWidth, hydrogenRectStart.y + hydrogenHeight)) {
        filepath = ofToDataPath("hydrogen.vti").c_str();
    } else if(isInsideRect(x, y,
                             toothRectStart.x, toothRectStart.y,
                             toothRectStart.x + toothWidth, toothRectStart.y + toothHeight)) {
        filepath = ofToDataPath("tooth.vti").c_str();
    }

    if(!filepath.empty()) {
        if (fileLoaded) {
            clearMesh();
        }
        fileLoaded = loadFile(filepath);

        return;
    }

    if(isInsideRect(x, y,
                      wireframeRectStart.x, wireframeRectStart.y,
                      wireframeRectStart.x + WIREFRAME_CHECKBOX_SIZE, wireframeRectStart.y + WIREFRAME_CHECKBOX_SIZE)) {
        showWireframe = !showWireframe;
        return;
    } else if(isInsideRect(x, y,
        lightRectStart.x, lightRectStart.y,
        lightRectStart.x + LIGHT_CHECKBOX_SIZE, lightRectStart.y + LIGHT_CHECKBOX_SIZE)) {
        enableLight = !enableLight;
        return;
    }

    if(isInsideRect(x, y,
                      updateRectStart.x, updateRectStart.y,
                      updateRectStart.x + UPDATE_BUTTON_WIDTH, updateRectStart.y + UPDATE_BUTTON_HEIGHT)) {
        isoValue = (sliderXPos - SLIDER_X)/(float)SLIDER_LENGTH;
        updateIsoSurface();
        return;
    }

    if(isInsideRect(x, y,
                      sliderXPos - SLIDER_THUMB_RADIUS, screenHeight - SLIDER_Y_FROM_BOTTOM + SLIDER_THUMB_RADIUS,
                      sliderXPos + SLIDER_THUMB_RADIUS, screenHeight - SLIDER_Y_FROM_BOTTOM - SLIDER_THUMB_RADIUS)) {
        sliderXPos = x;
        mouseMode = MOUSE_MODE_CLICK;
        cam.disableMouseInput();
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
