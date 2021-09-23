#include "ofApp.h"

#define WIDTH  57
#define HEIGHT  57

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(HEIGHT * 4, WIDTH * 4);
    ofSetFrameRate(30);
    
    serial.setup("/dev/cu.usbserial-01A7ECB2", 230400);
    f.allocate(WIDTH, HEIGHT, GL_RED);
    p.allocate(WIDTH, HEIGHT, OF_PIXELS_MONO);
    
    for(int i=0; i<8; i++){
        r.push_back(new JRectangle(glm::vec2(WIDTH/8 * i, 0), glm::vec2(3, HEIGHT)));
        r.back()->setColor(ofColor(50, 0));
    }
    
    rH = new JRectangle(glm::vec2(0, HEIGHT-1), glm::vec2(WIDTH, 1));
    rH->setColor(ofColor(255));
    state = Perlin;
    
    receiver.setup(4343);
    
//    cam.setDeviceID(0);
//    cam.setDesiredFrameRate(30);
//    cam.setup(480, 320);
}

//--------------------------------------------------------------
void ofApp::update(){
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/setHeight"){
            rH->loc.y = m.getArgAsFloat(0);
        }
    }
    
    
    f.begin();
    switch(state){
        case Mouse:
            ofFill();
            ofBackground(2);
            ofSetColor(255);
            ofDrawRectangle(0, (ofGetMouseY() / 4) % 57, WIDTH / 1, 1);
            break;
        case Perlin:{
            ofImage img;
            img.allocate(WIDTH, HEIGHT, OF_IMAGE_GRAYSCALE);
            for(int i=0; i<WIDTH; i++){
                for(int j=0; j<HEIGHT; j++){
                    float noiseVal = ofNoise(
                                            (float)i/50. + (sin(ofGetFrameNum() * 0.008)),
                                            (float)j/50. + (ofGetFrameNum() * 0.005) + 100.,
                                            (float)ofGetFrameNum() * 0.005
                                            );
                    img.getPixels()[i + (j*WIDTH)] = pow(noiseVal, 4.0) * 255;
                }
            }
            img.update();
            img.draw(0,0);
        }
            break;
        case Gradient:
            break;
        case Rectangles:{
            ofClear(0);
            for(int i=0; i<r.size(); i++){
                r[i]->update();
                r[i]->display();
            }
        }
            break;
        case RectHor:{
            ofClear(0);
            rH->update();
            rH->display();
        }
            break;
        case Webcam:{
            cam.update();

            if(cam.isFrameNew()){
                ofSetColor(255);
                cam.draw(0,0,ofGetWidth(), ofGetHeight());
            }
        }
    }
    f.end();
    
    
    f.getTexture().readToPixels(p);
    
    unsigned char* d = p.getData(); // Scans the pixels in horizontal lines
    // Check the pixels
//    for(int i=0; i<HEIGHT; i++){
//        for(int j=0; j<WIDTH; j++){
//            cout << (int)d[(i*HEIGHT) + j] << endl;
//        }
//    }
    
    unsigned char data[6 + 1 + 228 + 3]; // Copy the first 4 bars
    unsigned char end[3] = {'e', 'n', 'd'};
    memcpy(data+228+6+1, end, 3);
    memset(data, 0xFF, 6); // Broadcast addr (will be cut of on sender-esp32)
    
    for(int i=0; i<4; i++){
        for(int j=0; j<HEIGHT; j++){
            int index = (i*HEIGHT) + j;
            data[index + 6 + 1] = d[(i * (WIDTH / 8)) + (j*HEIGHT)] * 0.9 + 20; // Does this work...
            // Half brightness, add 1
        }
    }
    data[6] = 0x01; // ledBeam channel 0
    serial.writeBytes(data, 6 + 1 + 228 + 3);
    
#ifdef  SERIAL2
    for(int i=4; i<8; i++){
        for(int j=0; j<HEIGHT; j++){
            int index = (i*HEIGHT) + j;
            data[index] = d[(i * (WIDTH / 8)) + (j*HEIGHT)];
        }
    }
    data[6] = 0x02; // ledBeam channel 1
    serial2.writeBytes(data, 228+3);
#endif
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
        ofScale(4);
        ofSetColor(255);
        f.draw(0,0);
    ofPopMatrix();
    
    ofSetLineWidth(1);
    ofNoFill();
    ofSetColor(255);
    for(int i=0; i<8; i++){
        ofDrawRectangle(i * (ofGetWidth() / 8), 0, 3, ofGetWindowHeight());
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'q':{
            int index = ofRandom(4);
            r[index]->setEnvelope(100, 100, 1000, ofVec2f(0, 200));
//            r[index]->addEnvAlpha(10, 10, 1000);
//            r[index]->getLastEnv()->active = true;
//            r[index]->deleteWithFade(1000);qq
        }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
