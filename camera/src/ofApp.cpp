#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(30);
    grabber.setURI("http://192.168.1.101:8081");
    grabber.connect();
}

void ofApp::update(){
    grabber.update();
}

void ofApp::draw(){
    ofSetColor(255);
    grabber.draw(0, 0, cameraWidth, cameraHeight);
    
    std::stringstream ss;
    // Show connection statistics if desired.
    if (showStats){
        // Metadata about the connection state if needed.
        float kbps = grabber.getBitRate() / 1000.0f; // kilobits / second, not kibibits / second
        float fps = grabber.getFrameRate();
        
        ss << "          NAME: " << grabber.getCameraName() << std::endl;
        ss << "          HOST: " << grabber.getHost() << std::endl;
        ss << "           FPS: " << ofToString(fps,  2, 13, ' ') << std::endl;
        ss << "          Kb/S: " << ofToString(kbps, 2, 13, ' ') << std::endl;
        ss << " #Bytes Recv'd: " << ofToString(grabber.getNumBytesReceived(),  0, 10, ' ') << std::endl;
        ss << "#Frames Recv'd: " << ofToString(grabber.getNumFramesReceived(), 0, 10, ' ') << std::endl;
        ss << "Auto Reconnect: " << (grabber.getAutoReconnect() ? "YES" : "NO") << std::endl;
        ss << " Needs Connect: " << (grabber.getNeedsReconnect() ? "YES" : "NO") << std::endl;
        ss << "Time Till Next: " << grabber.getTimeTillNextAutoRetry() << " ms" << std::endl;
        ss << "Num Reconnects: " << ofToString(grabber.getReconnectCount()) << std::endl;
        ss << "Max Reconnects: " << ofToString(grabber.getMaxReconnects()) << std::endl;
        ss << "  Connect Fail: " << (grabber.hasConnectionFailed() ? "YES" : "NO");
    } else{
        ss << "Press any key to show connection stats.";
    }
    
    ofSetColor(255);
    ofDrawBitmapStringHighlight(ss.str(), 10, 10+12, ofColor(0, 80));
}


void ofApp::keyPressed(int key){
    showStats = !showStats;
}
