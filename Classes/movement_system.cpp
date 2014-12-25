#include "movement_system.h"

void MovementSystem::configure(EventManager &events)
{
	
}

void MovementSystem::update(EntityManager &es, EventManager &events, double dt)
{
	VelocityComponent::Handle velocityComponent;
	SpriteComponent::Handle spriteComponent;
	for (Entity entity : es.entities_with_components(spriteComponent, velocityComponent))
	{
		auto sprite = spriteComponent->sprite;
		auto pos = sprite->getPosition();
		auto velocity = velocityComponent->velocity;
        float newX = pos.x + velocity.x * dt;
        float newY = pos.y + velocity.y * dt;
        
        if(entity.has_component<BoundaryComponent>())
        {
            //check to make sure the new x position is not outside of its boundarys
            auto contentSize = sprite->getContentSize();
            auto bounds = entity.component<BoundaryComponent>()->boundary;
            bool topXBound = newX+contentSize.width / 3 > bounds.size.width;
            bool bottomXBound = newX-contentSize.width / 3 < bounds.origin.x;
            bool topYBound = newY+contentSize.height / 3 > bounds.size.height;
            bool bottomYBound = newY-contentSize.height / 3 < bounds.origin.y;
            Vec2 newPos(newX, newY);
            
            if(topXBound || bottomXBound)
            {
                //only update the y coordinate
                newPos = Vec2(pos.x, newY);
            }
            if(bottomYBound || topYBound)
            {
                //only update the x coordinate
                newPos = Vec2(newX, pos.y);
            }
            if((topXBound || bottomXBound) && (topYBound || bottomYBound))
            {
                //no position update
                newPos = Vec2(pos.x,pos.y);
            }
            
            sprite->setPosition(newPos);
            
        }
        else
        {
            sprite->setPosition(newX, newY);
        }


	}
}
