//
//  upgrade_system.cpp
//  space
//
//  Created by jarod on 10/16/14.
//
//

#include "upgrade_system.h"

UpgradeSystem::UpgradeSystem(EntityManager &em)
{
    entityManager = &em;
}

void UpgradeSystem::configure(EventManager &events)
{
    events.subscribe<UpgradeDropEvent>(*this);
    events.subscribe<PlayerUpgradeCollisionEvent>(*this);
}

void UpgradeSystem::receive(const UpgradeDropEvent &upgradeDropEvent)
{
    auto entity = entityManager->create();
    std::string upgradeString = MapSearchByValue<std::map<std::string, UpgradeDrop>, UpgradeDrop>(upgradeMap, upgradeDropEvent.dropType)->first + ".png";
    auto spriteComponent = entity.assign<SpriteComponent>(upgradeString);
    auto sprite = spriteComponent->sprite;
    sprite->setName("upgrade"); //physics body name
    sprite->setPosition(upgradeDropEvent.startingPos);
    entity.assign<VelocityComponent>(-50.f, 0);
    entity.assign<CollisionComponent>(CollisionType::UPGRADE);
    entity.assign<UpgradeComponent>(upgradeDropEvent.dropType);
    upgrades.push_back(entity);
}

void UpgradeSystem::receive(const PlayerUpgradeCollisionEvent &playerUpgradeCollisionEvent)
{
    entityx::Entity player = playerUpgradeCollisionEvent.player;
    entityx::Entity drop = playerUpgradeCollisionEvent.drop;
    
    if(player.has_component<WeaponComponent>())
    {
        auto weaponComponent = player.component<WeaponComponent>();
        if (drop.has_component<UpgradeComponent>())
        {
            switch (drop.component<UpgradeComponent>()->drop)
            {
                case UpgradeDrop::LASER:
                {
                    weaponComponent->laser.increaseLevel();
                    break;
                }
                case UpgradeDrop::MISSILE:
                {
                    weaponComponent->missile.increaseLevel();
                    break;
                }
                case UpgradeDrop::TURRET:
                {
                    //add turret upgrades here
                    break;
                }
                case UpgradeDrop::OVERSHIELD:
                {
                    //add overshield upgrades here
                    break;
                }
            }
        }

    }
    //throw exception, player entity must have its weapon component

    drop.destroy();
    //upgrades.erase(drop);
}

void UpgradeSystem::update(EntityManager &es, EventManager &events, double dt)
{
    auto director = Director::getInstance();
    auto screenSize = director->getWinSize();
    
    std::list<entityx::Entity>::iterator iter = upgrades.begin();
    std::list<entityx::Entity>::iterator end = upgrades.end();
    
    while (iter != end)
    {
        auto e = *iter;
        if(!e.valid() || !e.has_component<UpgradeComponent>())
        {
            upgrades.erase(iter++);
            break;
        }
        
        auto sprite = e.component<SpriteComponent>()->sprite;
        
        //check if upgrade object is off screen, if so, destroy it
        auto contentSize = sprite->getContentSize();
        auto pos = sprite->getPosition();
        
        float max_x = screenSize.width + contentSize.width / 2;
        float max_y = screenSize.height + contentSize.height / 2;
        
        if (pos.x < -contentSize.width / 2  ||
            pos.x > max_x					||
            pos.y < -contentSize.height / 2 ||
            pos.y > max_y)
        {
            e.destroy();
            upgrades.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}