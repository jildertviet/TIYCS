#include "ofApp.h"

int main(){
	ofSetupOpenGL(640 * 1, 480 * 1, OF_WINDOW);
    return ofRunApp(std::make_shared<ofApp>());
}
