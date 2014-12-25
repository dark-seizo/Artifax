#ifndef _BULLET_SYSTEM_H__
#define _BULLET_SYSTEM_H__

#include <math.h>
#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;

//this is the event which will be emitted when any entity wants to fire a bullet
struct BulletFiredEvent : public entityx::Event < BulletFiredEvent >
{
	BulletFiredEvent(cocos2d::Vec2 new_startPosition, cocos2d::Vec2 new_velocity, BulletType new_bulletType, float new_power)
		: startPosition(new_startPosition), velocity(new_velocity), bulletType(new_bulletType), power(new_power) {}

	Vec2				startPosition,
						velocity;
	BulletType			bulletType;
    float               power;
};

struct TargetedBulletFiredEvent : public entityx::Event < TargetedBulletFiredEvent >
{
	TargetedBulletFiredEvent(cocos2d::Vec2 new_startPosition, float new_bulletSpeed, BulletType new_bulletType, float new_power, entityx::Entity new_target)
		: startPosition(new_startPosition), bulletSpeed(new_bulletSpeed), bulletType(new_bulletType), target(new_target), power(new_power) {};

    Vec2				startPosition;
    float               bulletSpeed;
	BulletType			bulletType;
	entityx::Entity		target;
    float               power;
};

class BulletSystem : public entityx::System < BulletSystem >, public Receiver < BulletSystem >
{
public:

	BulletSystem(EntityManager &es);

	void configure(EventManager &events);

	void update(EntityManager &es, EventManager &events, double dt) override;

	void receive(const BulletFiredEvent &bulletEvent);
	void receive(const TargetedBulletFiredEvent &targetedBulletEvent);
    
    static const entityx::Entity SelectTarget(EntityManager &entityManager, CollisionType type, Vec2 startingPos);

private:
	//configures a new bullet from an bulletFiredEvent and returns its entity id
	entityx::Entity configureBullet(cocos2d::Vec2 startPosition, cocos2d::Vec2 projection, BulletType type, float power);

	//reference to the entity system for creating entities outside of update method.
	EntityManager		*entityManager;
};

#endif