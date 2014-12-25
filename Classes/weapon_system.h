#ifndef _WEAPON_SYSTEM__H
#define _WEAPON_SYSTEM__H

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "bullet_system.h"
#include "components.h"
#include "constants.h"
#include "level.h"

USING_NS_CC;
using namespace entityx;

struct PlayerWeaponFiredEvent : public entityx::Event < PlayerWeaponFiredEvent >
{
	PlayerWeaponFiredEvent(entityx::Entity &new_entity) : entity(new_entity) {}

	entityx::Entity			entity;
};

struct EnemyWeaponFiredEvent : public entityx::Event < EnemyWeaponFiredEvent >
{
    EnemyWeaponFiredEvent(entityx::Entity new_entity) : entity(new_entity) {}
    
    entityx::Entity         entity;
};

class WeaponSystem : public entityx::System < WeaponSystem >, public Receiver< WeaponSystem >
{
public:
    WeaponSystem(EntityManager &em);
    
	void configure(EventManager &events);

	void update(EntityManager &es, EventManager &events, double dt) override;

	void receive(const PlayerWeaponFiredEvent &weaponFiredEvent);
    void receive(const EnemyWeaponFiredEvent &enemyWeaponFiredEvent);
    
    //finds the closest target of the collision type passed in.
    //entityx::Entity findTarget(CollisionType type);

private:
	//reference to the event manager so we can emit events inside of the receive functions
	EventManager			*eventManager = nullptr;
    EntityManager           *entityManager = nullptr;
};

#endif