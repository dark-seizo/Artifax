
#ifndef _RENDER_SYSTEM__
#define _RENDER_SYSTEM__

#include "cocos2d.h"
#include "entityx/entityx.h"
#include "components.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;

class RenderSystem : public System < RenderSystem >, public Receiver<RenderSystem>
{
public:
	void update(EntityManager &es, EventManager &events, double dt) override;

	void receive(const ComponentAddedEvent<SpriteComponent> &spriteAddedEvent);
	void receive(const ComponentAddedEvent<ProgressBarComponent> &progressBarAddedEvent);
	void receive(const EntityDestroyedEvent &entityDestroyedEvent);
    void receive(const ComponentAddedEvent<ParallaxComponent> &parallaxAddedEvent);

	RenderSystem();
	~RenderSystem();

	//add/setup event handlers
	void configure(EventManager &events);
};

#endif