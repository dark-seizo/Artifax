//
//  parallax_system.cpp
//  space
//
//  Created by jarod on 9/28/14.
//
//

#include "parallax_system.h"

void ParallaxScrollingSystem::configure(EventManager &em)
{}

void ParallaxScrollingSystem::update(EntityManager &es, EventManager &events, double dt)
{
    ParallaxComponent::Handle parallaxComponent;
    for(auto entity : es.entities_with_components(parallaxComponent))
    {
        auto bg_1 = entity.component<ParallaxComponent>()->background_1;
        auto bg_2 = entity.component<ParallaxComponent>()->background_2;
        auto fg_1 = entity.component<ParallaxComponent>()->foreground_1;
        
        if (fg_1 != nullptr)
        {
            Size contentSize_fg = fg_1->getContentSize();
            //don't update the foreground if it has already passed by the screen once
            if (fg_1->getPositionX() > -(contentSize_fg.width * 0.5))
            {
                fg_1->setPositionX(fg_1->getPositionX() - foregroundScrollSpeed * dt);
            }
        }
        
        bg_1->setPositionX(bg_1->getPositionX() - backgroundScrollSpeed * dt);
        bg_2->setPositionX(bg_2->getPositionX() - backgroundScrollSpeed * dt);
        
        Size contentSize_1 = bg_1->getContentSize();
        Size contentSize_2 = bg_2->getContentSize();
        if(bg_1->getPositionX() <= -(contentSize_1.width * 0.5))
        {
            bg_1->setPositionX(bg_2->getPositionX() + contentSize_2.width);
        }
        
        if(bg_2->getPositionX() <= -(contentSize_2.width * 0.5f))
        {
            bg_2->setPositionX(bg_1->getPositionX() + contentSize_1.width);
        }
    }
}