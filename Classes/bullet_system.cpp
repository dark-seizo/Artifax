#include "bullet_system.h"

BulletSystem::BulletSystem(EntityManager &es)
{
	entityManager = &es;
}

const entityx::Entity BulletSystem::SelectTarget(EntityManager &entityManager, CollisionType type, Vec2 startingPos)
{
    std::pair<entityx::Entity, float> target;
    target.second = 10000000.f; //some large number for now, needs to be some multiple of screen size
    
    CollisionComponent::Handle collisionComponent;
    SpriteComponent::Handle spriteComponent;
    for(entityx::Entity e : entityManager.entities_with_components(collisionComponent, spriteComponent))
    {
        if(collisionComponent->type == type)
        {
            auto sprite = spriteComponent->sprite;
            float distance = sprite->getPosition().distance(startingPos);
            if(distance <= target.second)
            {
                target.first = e;
                target.second = distance;
            }
        }
    }
    return target.first;
}

void BulletSystem::configure(EventManager &events)
{
	events.subscribe<BulletFiredEvent>(*this);
	events.subscribe<TargetedBulletFiredEvent>(*this);
}

void BulletSystem::receive(const BulletFiredEvent &bulletEvent)
{
	//CCLOG("bullet fired event received");
	this->configureBullet(bulletEvent.startPosition, bulletEvent.velocity, bulletEvent.bulletType, bulletEvent.power);
}

void BulletSystem::receive(const TargetedBulletFiredEvent &targetedBulletEvent)
{
    auto newBullet = this->configureBullet(targetedBulletEvent.startPosition, Vec2::ZERO, targetedBulletEvent.bulletType, targetedBulletEvent.power);
	
    //assign TargetedComponent
	auto targetEntity = newBullet.assign<TargetedComponent>(targetedBulletEvent.target)->target;
    auto sprite = newBullet.component<SpriteComponent>()->sprite;
    auto targetSprite = targetEntity.component<SpriteComponent>()->sprite;
    Vec2 bulletPosition = sprite->getPosition();
    Vec2 targetPosition = targetSprite->getPosition();
    auto targetVelocityComponent = targetEntity.component<VelocityComponent>();
    float dy = targetSprite->getPositionY() - sprite->getPositionY();
    float dx = targetSprite->getPositionX() - sprite->getPositionX();
    
    float angle = radianToDegree(atanf(dy/dx));
    if(targetSprite->getPositionY() > sprite->getPositionY()) angle += 180;
    if(targetPosition.y > bulletPosition.y && targetPosition.x > bulletPosition.x) angle+=180;
    if(targetPosition.y < bulletPosition.y && targetPosition.x < bulletPosition.x)
        angle+=180;
    
    sprite->setRotation(-angle);
    //sprite->getPhysicsBody()->setRotationOffset(degreeToRadian(angle));
    
    Vec2 targetVelocity = targetVelocityComponent->velocity;
    
    //to find the time it takes for the bullet to hit the player, we use law of cosines and quadratic formula
    Vec2 D_vec = targetPosition - bulletPosition;
    float D = D_vec.length();
    float targetSpeed = targetVelocity.length();
    float bulletSpeed = targetedBulletEvent.bulletSpeed;
    
    Vec2 norm = bulletPosition - targetPosition;
    norm.normalize();
    Vec2 targetVelocityNorm = targetVelocity;
    targetVelocityNorm.normalize();
    
    float cos = Vec2::dot(norm, targetVelocityNorm);
    
    auto bulletFiredAtPlayer = [&D_vec, &newBullet, &bulletSpeed] ()
    {
        Vec2 vel = D_vec;
        vel.normalize();
        newBullet.component<VelocityComponent>()->velocity = vel * bulletSpeed;
    };
    
    //gives a 30% chance that the enemy will shoot at the player directly,
    //so the player can't just dodge back and forth to play on the enemies
    //shooting algo
    if(generateRandomFloat(0.0f, 1.0f) <= 0.3f)
    {
        bulletFiredAtPlayer();
        return;
    }
    else
    {
        float a = bulletSpeed * bulletSpeed - targetSpeed * targetSpeed;
        float b = 2 * D * targetSpeed * cos;
        float c = -(D * D);
        
        if(((b*b) - 4*a*c) < 0)
        {
            //just fire the bullet at the player in this case
            bulletFiredAtPlayer();
            return;
        }
        
        float t_1 = (-b + sqrtf((b*b) - 4 * a * c )) / (2 * a);
        float t_2 = (-b - sqrtf((b*b) - 4 * a * c )) / (2 * a);
        
        float t = MIN(t_1, t_2) < 0 ? MAX(t_1, t_2) : MIN(t_1, t_2);
        //CCLOG("t=%f", t);
        
        //if t is greater than 3, then just shoot directly at the player
        if (t > 3.f)
        {
            bulletFiredAtPlayer();
        }
        else
        {
            
            Vec2 desiredDirection = targetVelocity + ((targetPosition - bulletPosition) / t);
            desiredDirection.normalize();
            
            newBullet.component<VelocityComponent>()->velocity = desiredDirection * bulletSpeed;
        }
    }
}

void BulletSystem::update(EntityManager &es, EventManager &events, double dt)
{
	auto director = Director::getInstance();
	auto screenSize = director->getWinSize();

	SpriteComponent::Handle spriteComponent;
	BulletComponent::Handle bulletComponent;
	VelocityComponent::Handle velocityComponent;
	//invoke action on entities sprite using velocity (how do we find the specific velocity for each bullet?)
	for (Entity e : es.entities_with_components(spriteComponent, bulletComponent, velocityComponent))
    {

		auto sprite = spriteComponent->sprite;
        
		//check if bullet is off screen, if so, destroy it
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
		}
	}
}

entityx::Entity BulletSystem::configureBullet(cocos2d::Vec2 startPosition, cocos2d::Vec2 projection, BulletType type, float power)
{
	//CCLOG("bullet fired event received");
	auto newBullet = entityManager->create();

	newBullet.assign<BulletComponent>(type, power);
    newBullet.assign<VelocityComponent>(Vec2(projection.x*cosf(degreeToRadian(projection.y)), projection.x*sinf(degreeToRadian(projection.y))));
	auto spriteComponent = newBullet.assign<SpriteComponent>(bulletToString(type));
	auto sprite = spriteComponent->sprite;

	sprite->setPosition(startPosition);
	sprite->setRotation(-projection.y);

	CollisionType collisionType;
	switch (type)
	{
	case BulletType::LARGE_SHOT:
	case BulletType::MEDIUM_SHOT:
	case BulletType::SMALL_SHOT:
		collisionType = CollisionType::ENEMY_PROJECTILE;
		sprite->setZOrder(Z_ORDER::ENEMY_PROJECTILE);
		break;
	case BulletType::PLAYER_LASER:
	case BulletType::PLAYER_MISSILE:
		collisionType = CollisionType::PLAYER_PROJECTILE;
		sprite->setZOrder(Z_ORDER::PLAYER_PROJECTILE);
		break;

	case BulletType::NO_BULLET_TYPE:
	default:
		//need better error handling here
		CCLOG("[!] Attempting to create a bullet with undefined bullet type. (BulletSystem)");
		collisionType = CollisionType::NO_COLLISION;
		break;
	}

	newBullet.assign<CollisionComponent>(collisionType);
    //sprite->getPhysicsBody()->setRotationOffset(degreeToRadian(-projection.y + 180));
    

	return newBullet;
}
