#include "input_system.h"

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{
}

void InputSystem::configure(EventManager &events)
{
	events.subscribe<ComponentAddedEvent<InputComponent>>(*this);
	events.subscribe <ComponentRemovedEvent<InputComponent>>(*this);

	//preprocessor directives should be put here for different platform event setup

	//setup keyboard listener
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(InputSystem::keyPressed, this);
	listener->onKeyReleased = CC_CALLBACK_2(InputSystem::keyReleased, this);

	auto director = Director::getInstance();
	auto eventDispatcher = director->getEventDispatcher();
	eventDispatcher->addEventListenerWithFixedPriority(listener, 1);

	eventManager = &events;
}

void InputSystem::receive(const ComponentAddedEvent<InputComponent> &inputComponentAddedEvent)
{
	players.push_back(inputComponentAddedEvent.entity);
}

void InputSystem::receive(const ComponentRemovedEvent<InputComponent> &inputComponentRemovedEvent)
{
	players.remove(inputComponentRemovedEvent.entity);
}

void InputSystem::update(EntityManager &es, EventManager &events, double dt)
{
	//iterate over entities in players and fire their weapon if they are holding down their fire key

	for (entityx::Entity entity : players)
	{
		if (entity.has_component<WeaponComponent>() && entity.has_component<InputComponent>())
		{
			auto isWeaponKeyPressed = entity.component<InputComponent>()->keyPressedMap["fire"];
			if (isWeaponKeyPressed)
			{
				auto weaponComponent = entity.component<WeaponComponent>();

#pragma add turret checks here later
				if (weaponComponent->laser.isReady() || weaponComponent->missile.isReady()) //ADD TURRET CHECKS HERE LATER
				{
					events.emit<AttemptWeaponFireEvent>(entity);
				}
			}
		}
	}
}

void InputSystem::keyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	for (entityx::Entity entity : players)
	{
		if (entity.has_component<InputComponent>() && entity.has_component<VelocityComponent>())
		{
			auto ic = entity.component<InputComponent>();
			auto keyMap = ic->inputMap;
			Vec2 velocity = entity.component<VelocityComponent>()->velocity;

			if (keyCode == keyMap.at("up"))
			{
				velocity.y = 1;
				ic->keyPressedMap["up"] = true;
			}
			else if (keyCode == keyMap.at("down"))
			{
				velocity.y = -1;
				ic->keyPressedMap["down"] = true;
			}
			else if (keyCode == keyMap.at("left"))
			{
				velocity.x = -1;
				ic->keyPressedMap["left"] = true;
			}
			else if (keyCode == keyMap.at("right"))
			{
				velocity.x = 1;
				ic->keyPressedMap["right"] = true;
			}
			else if (keyCode == keyMap.at("fire"))
			{
				eventManager->emit<AttemptWeaponFireEvent>(entity);
				ic->keyPressedMap["fire"] = true;
			}
            
            velocity.normalize();
            velocity *= PLAYER_VELOCITY;
            entity.component<VelocityComponent>()->velocity = velocity;
		}
		else CCLOG("[FATAL] Player entity being iterated over in InputSystem with no InputComponent (or VelocityComponent..).");
	}
}

void InputSystem::keyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	for (entityx::Entity entity : players)
	{
		if (entity.has_component<InputComponent>() && entity.has_component<VelocityComponent>())
		{
			auto ic = entity.component<InputComponent>();
			//need to pass this by reference instead of by value
			auto keyMap = ic->inputMap;
			Vec2 velocity = entity.component<VelocityComponent>()->velocity;

			if (keyCode == keyMap.at("up"))
			{
				if (ic->keyPressedMap["down"])
				{
					velocity.y = -1;
				}
				else
				{
					velocity.y = 0;
				}
				ic->keyPressedMap["up"] = false;
			}
			else if (keyCode == keyMap.at("down"))
			{
				if (ic->keyPressedMap["up"])
				{
					velocity.y = 1;
				}
				else
				{
					velocity.y = 0;
				}
				ic->keyPressedMap["down"] = false;
			}
			else if (keyCode == keyMap.at("right"))
			{
				if (ic->keyPressedMap["left"])
				{
					velocity.x = -1;
				}
				else
				{
					velocity.x = 0;
				}
				ic->keyPressedMap["right"] = false;
			}
			else if (keyCode == keyMap.at("left"))
			{
				if (ic->keyPressedMap["right"])
				{
					velocity.x = 1;
				}
				else
				{
					velocity.x = 0;
				}
				ic->keyPressedMap["left"] = false;
			}
			else if (keyCode == keyMap.at("fire"))
			{
				ic->keyPressedMap["fire"] = false;
			}
            
            velocity.normalize();
            entity.component<VelocityComponent>()->velocity = velocity * PLAYER_VELOCITY;

			//for testing
			if (keyCode == EventKeyboard::KeyCode::KEY_U) entity.component<WeaponComponent>()->laser.increaseLevel();
		}
	}
}
