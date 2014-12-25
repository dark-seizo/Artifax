#ifndef _ENEMY_GROUP_SYSTEM_H__
#define _ENEMY_GROUP_SYSTEM_H__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"

USING_NS_CC;
using namespace entityx;

class EnemyGroupSystem : public entityx::System < EnemyGroupSystem >
{
public:
	void configure(EventManager &events);
	
	void update(EntityManager &es, EventManager &events, double dt) override;
};

#endif