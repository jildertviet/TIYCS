#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

#define WIDTH   1280
#define HEIGHT  800

#define NUM_WINDOWS 3

//========================================================================
int main( ){
#ifdef  TARGET_RASPBERRY_PI
    ofSetupOpenGL(WIDTH,HEIGHT,OF_FULLSCREEN);
    ofGLFWWindowSettings settings;

    settings.setSize(WIDTH,HEIGHT);
    settings.setPosition(glm::vec2(0,0));
    settings.decorated = false;
    settings.resizable = false;
//    settings.orientation = OF_ORIENTATION_180; // Doesn't work
    
    shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);
    shared_ptr<ofApp> app(new ofApp);
    
    ofRunApp(window, app);

    ofRunMainLoop();
#else
    for(int i=0; i<NUM_WINDOWS; i++){
        ofGLFWWindowSettings settings;

        settings.setSize(WIDTH, HEIGHT);
        settings.setPosition(glm::vec2(0,0));
        settings.resizable = true;
        shared_ptr<ofAppBaseWindow> window = ofCreateWindow(settings);

        shared_ptr<ofApp> mainApp(new ofApp);
        mainApp->portNumAdd = i;
        
        ofRunApp(window, mainApp);
    
//        outterWindow->setOsc(5001);
    }
    ofRunMainLoop();
    
#endif
}
