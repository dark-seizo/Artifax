//
//  enemy_health_system.cpp
//  space
//
//  Created by jarod on 9/4/14.
//
//

#include "enemy_health_system.h"

void EnemyHealthSystem::configure(EventManager &events)
{
    events.subscribe<EnemyHitEvent>(*this);
    eventManager = &events;
}

void EnemyHealthSystem::receive(const EnemyHitEvent &enemyHitEvent)
{
    auto entity = enemyHitEvent.enemyHit;
    if (entity.has_component<EnemyHealthComponent>())
    {
        auto healthComponent = entity.component<EnemyHealthComponent>();
        healthComponent->health -= enemyHitEvent.power;
        
        if (healthComponent->health <= 0)
        {
            //stop enemy from moving
            entity.remove<PathComponent>();
            //stop enemy from shooting
            entity.remove<EnemyWeaponComponent>();
            
            if (entity.has_component<UpgradeComponent>() && entity.has_component<SpriteComponent>())
            {
                if (entity.component<UpgradeComponent>()->drop != UpgradeDrop::NO_UPGRADE)
                {
                    //raise event to place an upgrade at the position of the enemy when they die.
                    Vec2 pos = entity.component<SpriteComponent>()->sprite->getPosition();
                    auto drop = entity.component<UpgradeComponent>()->drop;
                    eventManager->emit<UpgradeDropEvent>(pos, drop);
                }
            }
            //need to immediately destroy the enemy, then create another entity for the explosion, then destroy it when the animation is done.
            auto explosion = entityManager->create();
            
            if (entity.has_component<SpriteComponent>())
            {
                auto spriteComponent = entity.component<SpriteComponent>();
                auto sprite = spriteComponent->sprite;
                auto explosionSprite = explosion.assign<SpriteComponent>("explosion_1.png")->sprite;
                explosionSprite->setPosition(sprite->getPosition());
                explosionSprite->setZOrder(sprite->getZOrder());
                
                //destroy the enemy
                entity.destroy();
                
                auto animationCache = AnimationCache::getInstance();
                auto animation = animationCache->getAnimation("explosion");
                auto callback = [explosion] () -> void
                {
                    //to get around the argument being const
                    auto entity2 = explosion;
                    //entity2.invalidate();
                    entity2.destroy();
                };
                
                //auto sprite = entity.component<SpriteComponent>()->sprite;
                
                auto animate = Animate::create(animation);
                auto destroy = CallFunc::create(callback);
                
                //stop the entity from receiving any more collisions
                //sprite->setPhysicsBody(nullptr);
                
                //entity.component<SpriteComponent>()->sprite->runAction(Sequence::create(animate, enemyDeath, NULL));
                explosionSprite->runAction(Sequence::create(animate, destroy, NULL));
            }
        }
    }
}

void EnemyHealthSystem::update(EntityManager &es, EventManager &events, double dt)
{
    
}

