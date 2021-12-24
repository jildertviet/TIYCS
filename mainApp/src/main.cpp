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
//    settings.orientation = OF_ORIENTATION_180;
    
    shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);
//    window->setOrientation(OF_ORIENTATION_180);

    shared_ptr<ofApp> app(new ofApp);
    
    ofRunApp(window, app);

    ofRunMainLoop();
}
