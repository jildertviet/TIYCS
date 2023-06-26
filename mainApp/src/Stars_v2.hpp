#ifndef Stars_v2
#define Stars_v2

class Star {
public:
    float x;
    float y;
    float z;
    ofColor color;

    float speed = 0.025 * 0.3;
    int step = 2;
    int warpZ = 12;
    ofVec3f v;


    ofColor getColor() {
        return ofColor::fromHsb(200, 255, ofRandom(50, 100));
    }

    Star() {
        reset();
    }

    void setLoc(glm::vec3 l, bool dummy){
      x = l.x;
      y = l.y;
      z = l.z;
    }

    void reset() {
        v.set(ofRandom(0, 1280), ofRandom(0, 800), ofRandom(1, warpZ));
        x = v.x;
        y = v.y;
        color = getColor();
    }

    ofVec3f calcVel() {
        return ofVec3f(0, 0, -speed);
    }

    void display() {
        v += calcVel();
        float x = v.x / v.z;
        float y = v.y / v.z;
        float x2 = v.x / (v.z + speed * 0.50);
        float y2 = v.y / (v.z + speed * 0.50);

        // ofSetColor(color);
        ofSetColor(255);
        ofFill();
        ofDrawLine(x, y, x2, y2);

        if (x < 0 || x > ofGetWidth() || y < 0 || y > ofGetHeight()) {
            reset();
        }
    }

    void update(float v){};
};

class Stars {
public:

  int halfw;
  int halfh;


  // Dummy:
    float* height;
    float* hOffset;
    float* travelSpeed;
    float* planetID;

    int numOfStars = 250;
    vector<Star*> stars;

    Stars(glm::vec2 size, string prefix) {
        for (int i = 0; i < numOfStars; i++) {
            stars.push_back(new Star());
        }
    }

    void display(float& b) {
        // ofTranslate(1280*0.5, 800*0.5);

        for (int i = 0; i < stars.size(); i++) {
            stars[i]->display();
        }
    }

    void update(glm::vec3 l){}
};

#endif
