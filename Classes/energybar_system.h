
#ifndef _ENERGYBAR_SYSTEM_
#define _ENERGYBAR_SYSTEM_

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "weapon_system.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;

struct AttemptWeaponFireEvent : public entityx::Event < AttemptWeaponFireEvent >
{
	AttemptWeaponFireEvent(entityx::Entity &new_entity) : entity(new_entity) {}

	entityx::Entity entity;
};

class EnergyBarSystem : public entityx::System < EnergyBarSystem >, public entityx::Receiver < EnergyBarSystem >
{
public:
	void configure(EventManager &events);

	void receive(const AttemptWeaponFireEvent &attemptWeaponFireEvent);

	void update(EntityManager &es, EventManager &events, double dt) override;

private:

	EventManager			*eventManager;
};

#endif