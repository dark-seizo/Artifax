
#ifndef _MOVEMENT_SYSTEM__
#define _MOVEMENT_SYSTEM__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"

USING_NS_CC;
using namespace entityx;

class MovementSystem : public entityx::System < MovementSystem >
{
public:
	//handles the updating for the system
	void update(EntityManager &es, EventManager &events, double dt) override;

	//setup event handlers here
	void configure(EventManager &events);

private:
};

#endif