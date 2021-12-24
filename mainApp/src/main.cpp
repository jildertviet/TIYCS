#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    ofSetupOpenGL(1280,800,OF_FULLSCREEN);

    ofGLFWWindowSettings settings;

    settings.setSize(1280,800);
    settings.setPosition(glm::vec2(0,0));
    settings.decorated = false;
    settings.resizable = false;
    
    shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);

    shared_ptr<ofApp> app(new ofApp);
    
    ofRunApp(window, app);

    ofRunMainLoop();
}
