//
//  standard_enemy_system.cpp
//  Artifax
//
//  Created by jarod on 12/25/14.
//
//

#include "standard_enemy_system.h"


void StandardEnemySystem::configure(EventManager &events)
{
    events.subscribe<EnemyHitEvent>(*this);
    eventManager = &events;
}

void StandardEnemySystem::receive(const EnemyHitEvent &enemyHitEvent)
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

void StandardEnemySystem::update(EntityManager &es, EventManager &events, double dt)
{
    //enemy groups
    EnemyGroupComponent::Handle enemyGroupComponent;
    UpgradeComponent::Handle upgradeComponent;
    for (auto entity : es.entities_with_components(enemyGroupComponent, upgradeComponent))
    {
        if (enemyGroupComponent->groupEnemies.empty())
        {
            entity.destroy();
        }
        else
        {
            //iterate over groupEnemies and remove the invalid ones
            std::list<entityx::Entity>::iterator iter = enemyGroupComponent->groupEnemies.begin();
            std::list<entityx::Entity>::iterator end = enemyGroupComponent->groupEnemies.end();
            while (iter != end)
            {
                if (!iter->valid())
                {
                    //remove from list and increment iterator to keep from having issues with the removed element
                    enemyGroupComponent->groupEnemies.erase(iter++);
                }
                else
                    ++iter;
            }
            
            if (enemyGroupComponent->groupEnemies.size() == 1)
            {
                //set that enemies upgrade drop to the last remaining enemy, then destroy the group entity
                if (enemyGroupComponent->groupEnemies.front().has_component<UpgradeComponent>())
                {
                    if (upgradeComponent->drop != UpgradeDrop::NO_UPGRADE)
                    {
                        auto drop = upgradeComponent->drop;
                        enemyGroupComponent->groupEnemies.front().component<UpgradeComponent>()->drop = drop;
                    }
                    //entity.destroy();
                }
                //destroy the group
                entity.destroy();
            }
        }
    }
    
    //enemy movement
    //VelocityComponent::Handle velocityComponent;
    EnemyComponent::Handle enemyComponent;
    PathComponent::Handle pathComponent;
    SpriteComponent::Handle spriteComponent;
    for (Entity entity : es.entities_with_components(pathComponent, enemyComponent, spriteComponent))
    {
        float speed = enemyComponent->speed;
        auto sprite = spriteComponent->sprite;
        auto pos = sprite->getPosition();
        auto pathType = pathComponent->path.type;
        
        if (pathType == PathType::FUNCTION)
        {
            auto func = pathComponent->path.function;
            float newX = pos.x - speed * dt;
            float newY = func(newX);
            Vec2 newPos = Vec2(newX, newY);
            sprite->setPosition(newPos);
            
            //set rotation
            if(enemyComponent->rotatable)
            {
                auto rotation = atanf((newY - pos.y) / (newX - pos.x));
                sprite->setRotation(-radianToDegree(rotation));
            }
            
        }
        else if (pathType == PathType::POLAR)
        {
            //handle functions that deal with polar cordinates.
        }
        
        //remove enemies that are completely past the left side of the screen
        if (sprite->getPositionX() + sprite->getContentSize().width / 2 < 0)
        {
            entity.destroy();
        }
    }
}
