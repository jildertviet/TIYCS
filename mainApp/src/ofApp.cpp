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
        screens[i].setup(i, glm::vec2(1280 * windowScaler, 800 * windowScaler), &screenOrder[0]);
        screens[i].commercials = commercials;
    }
    
    ofSetVerticalSync(false);
    blob.load("shadersGL2/shader");
    post.init(1024*4, 1024, false);
    post.createPass<BloomPass>()->setEnabled(true);
    post.createPass<GodRaysPass>()->setEnabled(true);
    light.setPosition(ofGetWidth()*0.5, ofGetHeight()*0.5, 4000);
    light.setPointLight();
//    light.setAmbientColor(ofFloatColor::white);
//    light.setDiffuseColor(ofFloatColor(1.0, 0.5, 0.5));
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i=0; i<3; i++){
        screens[i].update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(screens[0].scene == scenes::StarsFinal){
        float v = (float)(ofGetElapsedTimeMillis()) / 4000.;
        float v2 = (float)(1000 + ofGetElapsedTimeMillis()) / 4000.;
        
        light.enable();
        post.setFlip(false);
        post.begin();
        ofEnableDepthTest();
        for(int i=0; i<3; i++){
            ofPushMatrix();
            ofTranslate(1280*i * windowScaler, 0);
    //        screens[screenOrder[i]].draw();
            for(int j=0; j<screens[i].stars->stars.size(); j++){
                screens[i].stars->stars[j]->update();
                screens[i].stars->stars[j]->display();
            }
            ofPopMatrix();
        }
        blob.begin();
            blob.setUniform3f("iResolution", ofGetWidth(), ofGetHeight(), 0);
            blob.setUniform1f("iTime", ofGetElapsedTimeMillis() / 1000.);
            blob.setUniform4f("iMouse", ofGetMouseX(), ofGetMouseY(), 0, 0);
    //        blob.setUniform2f("offset", (ofNoise(v)-0.5)*2.0, (ofNoise(v2)-0.5)*2.0);
            blob.setUniform2f("offset", screens[0].busses[0],screens[0].busses[1]);
            blob.setUniform1f("Radius", screens[0].busses[2]);
            blob.setUniform1f("NoiseAmplitude", screens[0].busses[3]);
    //        blob.setUniform2f("offset", 0,0);
            ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        blob.end();
        
        post.end();
    } else{
        for(int i=0; i<3; i++){
            ofPushMatrix();
            ofTranslate(1280*i * windowScaler, 0);
            screens[screenOrder[i]].draw();
            ofPopMatrix();
        }
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
