#ifndef _COLLISION_SYSTEM_H__
#define _COLLISION_SYSTEM_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "shield_system.h"
#include "enemy_health_system.h"
#include "upgrade_system.h"
#include "components.h"
#include "MyBodyParser.h"

USING_NS_CC;
using namespace entityx;

class CollisionSystem : public entityx::System < CollisionSystem >, public Receiver < CollisionSystem >
{
public:
	void configure(EventManager &events);

	void update(EntityManager &es, EventManager &events, double dt) override;
    
    void receive(const ComponentAddedEvent<CollisionComponent> &collisionComponentAddedEvent);
    
private:
    bool pixelPerfectCollisionTest(cocos2d::Sprite *spr1, cocos2d::Sprite *spr2) const;
    
    bool onContactBegin(PhysicsContact &contact);
    
    EventManager        *eventManager = nullptr;
};


#endif