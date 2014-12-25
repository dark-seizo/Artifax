#include "enemy_movementAI_system.h"

void EnemyMovementAISystem::configure(EventManager &events)
{

}

//this updates the enemies velocity components.  the enemies position is updated by the movement system
void EnemyMovementAISystem::update(EntityManager &es, EventManager &events, double dt)
{
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