//
//  upgrade_system.h
//  space
//
//  Created by jarod on 10/16/14.
//
//

#ifndef space_upgrade_system_h
#define space_upgrade_system_h

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;

struct UpgradeDropEvent : public entityx::Event < UpgradeDropEvent >
{
    UpgradeDropEvent(Vec2 pos, UpgradeDrop drop) : startingPos(pos), dropType(drop) {}
    
    Vec2            startingPos;
    UpgradeDrop     dropType;
};

struct PlayerUpgradeCollisionEvent : public entityx::Event < PlayerUpgradeCollisionEvent >
{
    PlayerUpgradeCollisionEvent(entityx::Entity type, entityx::Entity new_player) : drop(type), player(new_player) {}
    
    entityx::Entity     drop;
    entityx::Entity     player;
};

class UpgradeSystem : public entityx::System < UpgradeSystem >, public entityx::Receiver < UpgradeSystem >
{
public:
    UpgradeSystem(EntityManager &em);
    
    void configure(EventManager &events);
    
    void update(EntityManager &es, EventManager &events, double dt) override;
    
    void receive(const UpgradeDropEvent &upgradeDropEvent);
    void receive(const PlayerUpgradeCollisionEvent &playerUpgradeCollisionEvent);
    
private:
    EntityManager               *entityManager;
    
    std::list<entityx::Entity>  upgrades;
};


#endif
