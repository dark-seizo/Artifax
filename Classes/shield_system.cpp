#include "shield_system.h"

void ShieldSystem::configure(EventManager &events)
{
    events.subscribe<ShieldHitEvent>(*this);
}

void ShieldSystem::receive(const ShieldHitEvent &shieldHitEvent)
{
    if (shieldHitEvent.player.has_component<ShieldComponent>())
    {
        auto entity = shieldHitEvent.player;
        auto shieldComponent = entity.component<ShieldComponent>();
        
        if (shieldComponent->active)
        {
            shieldComponent->strength -= shieldHitEvent.power;
            shieldComponent->rechargeTimeLeft = shieldComponent->rechargeDelay;
            
            if(shieldComponent->strength <= 0)
            {
                shieldComponent->rechargeTimeLeft = shieldComponent->depletedDelay;
                shieldComponent->strength = 0;
                shieldComponent->active = false;
                //maybe run some sort of animation on the shieldComponents display entity to show that it is depleted
            }
        }
        else
        {
            CCLOG("player has been hit with a fatal shot, need to raise player death event here");
        }
    }
}

void ShieldSystem::update(EntityManager &es, EventManager &events, double dt)
{
    ShieldComponent::Handle shieldComponent;
    for(auto entity : es.entities_with_components(shieldComponent))
    {
        if(shieldComponent->rechargeTimeLeft > 0)
        {
            shieldComponent->rechargeTimeLeft -= dt;
        }
        else if(shieldComponent->rechargeTimeLeft <= 0 && shieldComponent->strength < shieldComponent->maxStrength)
        {
            shieldComponent->strength += shieldComponent->rechargeRate * dt;
        }
        
        if (shieldComponent->strength > 0 && !shieldComponent->active)
        {
            shieldComponent->active = true;
        }
        
        auto shieldBar = shieldComponent->displayEntity;
        if (shieldBar.has_component<ProgressBarComponent>())
        {
            float percentage = (shieldComponent->strength / shieldComponent->maxStrength)*100.f;
            shieldBar.component<ProgressBarComponent>()->progressBar->setPercentage(percentage);
        }
        else
        {
            //need error handling here
            CCLOG("entity with shield component and no display component to be updated");
        }
    }
}
