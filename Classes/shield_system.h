#ifndef _SHIELD_SYSTEM__H
#define _SHIELD_SYSTEM__H

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"

USING_NS_CC;
using namespace entityx;

struct ShieldHitEvent : public entityx::Event < ShieldHitEvent >
{
    ShieldHitEvent(const float new_power, entityx::Entity playerHit) : power(new_power), player(playerHit) {}
    
    float           power;
    entityx::Entity player;
};


class ShieldSystem : public entityx::System < ShieldSystem >, public entityx::Receiver < ShieldSystem >
{
public:
    void configure(EventManager &events);

    void receive(const ShieldHitEvent &shieldHitEvent);
    
    void update(EntityManager &es, EventManager &events, double dt) override;
};

#endif