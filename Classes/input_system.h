
#ifndef _INPUT_SYSTEM__
#define _INPUT_SYSTEM__

#include <list>
#include "cocos2d.h"
#include "entityx/entityx.h"
#include "energybar_system.h"
#include "components.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;

class InputSystem : public entityx::System < InputSystem >, public entityx::Receiver < InputSystem >
{

public:
	InputSystem();
	~InputSystem();

	void configure(EventManager &events);

	void receive(const ComponentAddedEvent<InputComponent> &inputComponentAddedEvent);
	void receive(const ComponentRemovedEvent<InputComponent> &inputComponentRemovedEvent);

	void update(EntityManager &es, EventManager &events, double dt) override;

private:
	//used for processing keyboard events
	void keyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void keyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	EventManager					*eventManager = nullptr;
	std::list <entityx::Entity>		players;
};

#endif