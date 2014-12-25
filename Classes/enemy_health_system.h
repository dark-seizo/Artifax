//
//  enemy_health_system.h
//  space
//
//  Created by jarod on 9/4/14.
//
//

#ifndef _ENEMY_HEALTH_SYSTEM_H_
#define _ENEMY_HEALTH_SYSTEM_H_

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "upgrade_system.h"
#include "components.h"

USING_NS_CC;
using namespace entityx;

struct EnemyHitEvent : public entityx::Event < EnemyHitEvent >
{
    EnemyHitEvent(float new_power, entityx::Entity new_enemyHit) :
    power(new_power), enemyHit(new_enemyHit) {}
    
    entityx::Entity         enemyHit;
    float                   power;
};

class EnemyHealthSystem : public entityx::System < EnemyHealthSystem >, public entityx::Receiver < EnemyHealthSystem >
{
public:
    
    explicit EnemyHealthSystem(EntityManager &entities) : entityManager(&entities) {}

    void configure(EventManager &events);
    
    void receive(const EnemyHitEvent &enemyHitEvent);
    
    void update(EntityManager &es, EventManager &events, double dt) override;
private:
    EventManager        *eventManager = nullptr;
    EntityManager       *entityManager = nullptr;
};

#endif
