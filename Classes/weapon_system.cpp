#include "weapon_system.h"

WeaponSystem::WeaponSystem(EntityManager &em)
{
    entityManager = &em;
}

void WeaponSystem::configure(EventManager &events)
{
	eventManager = &events;

	events.subscribe<PlayerWeaponFiredEvent>(*this);
    events.subscribe<EnemyWeaponFiredEvent>(*this);
}

void WeaponSystem::receive(const EnemyWeaponFiredEvent &enemyWeaponFiredEvent)
{
    auto entity = enemyWeaponFiredEvent.entity;
    
    if (entity.has_component<EnemyWeaponComponent>() && entity.has_component<SpriteComponent>() && entity.has_component<EnemyComponent>())
    {
        auto ec = entity.component<EnemyComponent>();
        auto wc = entity.component<EnemyWeaponComponent>();
        Vec2 pos = entity.component<SpriteComponent>()->sprite->getPosition();
        
        //find out what kind of bullet type to use
        BulletType type = BulletType::NO_BULLET_TYPE;
        switch (ec->type)
        {
            case EnemyType::SMALL_ENEMY:
                type = BulletType::SMALL_SHOT;
                break;
            case EnemyType::MEDIUM_ENEMY:
                type = BulletType::MEDIUM_SHOT;
                break;
            case EnemyType::LARGE_ENEMY:
                type = BulletType::LARGE_SHOT;
                break;
        }
        
        switch (wc->type) {
            case EnemyWeaponType::UPGRADE:
            {
                eventManager->emit<BulletFiredEvent>(pos, Vec2(wc->speed, 180+20), type, wc->attackPower);
                eventManager->emit<BulletFiredEvent>(pos, Vec2(wc->speed, 180-20), type, wc->attackPower);
            }
                
            case EnemyWeaponType::STANDARD:
            {
                eventManager->emit<BulletFiredEvent>(pos, Vec2(wc->speed, 180), type, wc->attackPower);
                break;
            }
            case EnemyWeaponType::TARGETED:
            {
                auto targetEntity = BulletSystem::SelectTarget(*entityManager, CollisionType::PLAYER, pos);
                eventManager->emit<TargetedBulletFiredEvent>(pos, wc->speed, type, wc->attackPower, targetEntity);
                break;
            }
                
            default: break;
        }
    }
}

void WeaponSystem::receive(const PlayerWeaponFiredEvent &weaponFiredEvent)
{
	auto entity = weaponFiredEvent.entity;

	if (entity.has_component<WeaponComponent>() && entity.has_component<SpriteComponent>())
	{
		auto weaponComponent = entity.component<WeaponComponent>();
		auto sprite = entity.component<SpriteComponent>()->sprite;
		auto pos = sprite->getPosition();

		if (weaponComponent->laser.isReady())
		{
			switch (weaponComponent->laser.level)
			{
			case(LaserWeapon::LASER_LEVEL_ONE) :
				eventManager->emit<BulletFiredEvent>(pos, bullet_velocity_map.at("player_straight_shot"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
				break;
			case(LaserWeapon::LASER_LEVEL_FOUR) :
				eventManager->emit<BulletFiredEvent>(pos, bullet_velocity_map.at("player_20_up"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
				eventManager->emit<BulletFiredEvent>(pos, bullet_velocity_map.at("player_20_down"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
			case(LaserWeapon::LASER_LEVEL_THREE) :
				eventManager->emit<BulletFiredEvent>(Vec2(pos.x + 5.0f, pos.y), bullet_velocity_map.at("player_straight_shot"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
			case(LaserWeapon::LASER_LEVEL_TWO) :
				eventManager->emit<BulletFiredEvent>(Vec2(pos.x, pos.y + 10.f), bullet_velocity_map.at("player_straight_shot"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
				eventManager->emit<BulletFiredEvent>(Vec2(pos.x, pos.y - 10.f), bullet_velocity_map.at("player_straight_shot"), BulletType::PLAYER_LASER, weaponComponent->laser.power);
			default : break;
			}

			//need to preload this
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("flaunch.wav");

			weaponComponent->laser.delay += PLAYER_LASER_DELAY;
			//CCLOG("laser gun delay: %f", laserGun.delay);
		}

		if (weaponComponent->missile.isReady())
		{
			//implement missile weapon mechanics
			switch (weaponComponent->missile.level)
			{
                    
			default: break;
			}
			weaponComponent->missile.delay += PLAYER_MISSILE_DELAY;
		}

		if (weaponComponent->turretDelay <= 0)
		{
			//implement turret weapon mechanics
			weaponComponent->turretDelay += PLAYER_TURRET_DELAY;
		}
	}
	else CCLOG("[!] Entity without weapon component attempting to fire a player's weapon.");
}

void WeaponSystem::update(EntityManager &es, EventManager &events, double dt)
{
	WeaponComponent::Handle weaponComponent;
	for (Entity e : es.entities_with_components(weaponComponent))
	{
		if (weaponComponent->laser.delay > 0)
		{
			weaponComponent->laser.delay -= dt;
		}

		if (weaponComponent->missile.delay > 0)
		{
			weaponComponent->missile.delay -= dt;
		}

		if (weaponComponent->turretDelay > 0)
		{
			weaponComponent->turretDelay -= dt;
		}
	}
    
    EnemyWeaponComponent::Handle enemyWeaponComponent;
    SpriteComponent::Handle spriteComponent;
    for (entityx::Entity e : es.entities_with_components(enemyWeaponComponent, spriteComponent))
    {
        auto boundingRect = spriteComponent->sprite->getBoundingBox();
        auto winSize = Director::getInstance()->getWinSize();
        cocos2d::Rect winRect(0,0,winSize.width,winSize.height);
        
        if (boundingRect.intersectsRect(winRect))
        {
            if (enemyWeaponComponent->attackDelayRemaining < 0)
            {
                eventManager->emit<EnemyWeaponFiredEvent>(e);
                //adding a small random amount here to stagger the attack sequence of enemies to make it look more realistic
                float randFloat = generateRandomFloat(enemyWeaponComponent->frequencyDeltaMin, enemyWeaponComponent->frequencyDeltaMax);
                //CCLOG("random float: %f", randFloat);
                enemyWeaponComponent->attackDelayRemaining = enemyWeaponComponent->frequencyOfAttack + randFloat;
            }
            else
                enemyWeaponComponent->attackDelayRemaining -= dt;
        }
        

    }
    
    
}