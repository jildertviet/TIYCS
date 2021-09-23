//
//  Star.hpp
//  spaceTravel
//
//  Created by Jildert Viet on 09/09/2021.
//

#ifndef Star_hpp
#define Star_hpp

#include <stdio.h>
#include "ofMain.h"

class Star{
public:
    Star();
    void update(float speedMul=1);
    void display();
    float r = 10;
    glm::vec3 speed;
    glm::vec3 originalLoc;
    void setLoc(glm::vec3 loc, bool bSetOrigin = false);
private:
    glm::vec3 loc;
};
#endif /* Star_hpp */
