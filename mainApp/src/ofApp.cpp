#include "ofApp.h"

float windowScaler = 1;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetWindowShape(1280*3 * windowScaler, 800 * windowScaler);
    
#ifdef  TARGET_RASPBERRY_PI
    ofHideCursor();
#endif
//    ofHideCursor();
    
    // Shared resources
    commercials = new ofVideoPlayer[3];
    for(int i=0; i<3; i++){
        commercials[i].load("1280/commercial/" + ofToString(i) + ".mp4"); // Hard coded prefix :(
        commercials[i].setLoopState(ofLoopType::OF_LOOP_NONE);
        commercials[i].setVolume(0); // Audio off
    }
    
    for(int i=0; i<3; i++){
        screens[i].setup(i, glm::vec2(1280 * windowScaler, 800 * windowScaler));
        screens[i].commercials = commercials;
    }
    
    ofSetVerticalSync(false);
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i<3; i++){
        screens[i].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    for(int i=0; i<3; i++){
        ofPushMatrix();
        ofTranslate(1280*i * windowScaler, 0);
        screens[i].draw();
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            ofSetFullscreen(true);
            break;
    }
    for(int i=0; i<3; i++){
        screens[i].keyPressed(key);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    int i = floor(x / (1280. * windowScaler));
    screens[i].mousePressed(x - (i * 1280 * windowScaler), y, button);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
