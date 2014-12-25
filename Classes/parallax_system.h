//
//  parallax_system.h
//  space
//
//  Created by jarod on 9/28/14.
//
//

#ifndef _PARALLAX_SYSTEM_H__
#define _PARALLAX_SYSTEM_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"

USING_NS_CC;
using namespace entityx;

class ParallaxScrollingSystem : public entityx::System< ParallaxScrollingSystem >
{
public:
    
    void configure(EventManager &events);
    
    void update(EntityManager &es, EventManager &events, double dt) override;
    
private:
    float backgroundScrollSpeed       = 18;
    float foregroundScrollSpeed       = backgroundScrollSpeed / 3;
};

#endif
