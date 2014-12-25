#include "render_system.h"

RenderSystem::RenderSystem()
{	
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::configure(EventManager &events)
{
	//receive callback when a sprite component is added to an entity
	events.subscribe<ComponentAddedEvent<SpriteComponent>>(*this);

	//receive callback when a sprite component is removed from an entity
	events.subscribe<EntityDestroyedEvent>(*this);

	//receive callback when progress bar component is added
	events.subscribe<ComponentAddedEvent<ProgressBarComponent>>(*this);
    
    //receive callback when a parallax component is added
    events.subscribe<ComponentAddedEvent<ParallaxComponent>>(*this);
}

void RenderSystem::receive(const ComponentAddedEvent<SpriteComponent> &spriteAddedEvent)
{
	auto sprite = spriteAddedEvent.component->sprite;

	auto director = Director::getInstance();
	auto currentScene = director->getRunningScene();
	auto gameLayer = currentScene->getChildByTag(GAME_LAYER);

	if (gameLayer)
	{
		auto batchNode = gameLayer->getChildByTag(MAIN_SPRITEBATCHNODE);
		if (batchNode)
		{
            sprite->setEntity(spriteAddedEvent.entity);
			batchNode->addChild(sprite);
		}
		else CCLOG("batchNode: NULL REFERENCE, RenderSystem, spriteAddedEvent");
	}
	else CCLOG("gameLayer: NULL REFERENCE, RenderSystem, spriteAddedEvent");
}

void RenderSystem::receive(const ComponentAddedEvent<ParallaxComponent> &parallaxComponentAddedEvent)
{
    auto winSize = Director::getInstance()->getWinSize();
    auto pc = parallaxComponentAddedEvent.component;
    auto director = Director::getInstance();
    auto currentScene = director->getRunningScene();
    auto gameLayer = currentScene->getChildByTag(GAME_LAYER);
    
    if(gameLayer)
    {
        //add the sprites for the parallax node
        if(pc->background_1 != nullptr && pc->background_2 != nullptr)
        {
            pc->background_1->setPosition(Vec2(winSize.width*0.5f, winSize.height*0.5f));
            pc->background_2->setPosition(Vec2(winSize.width*1.5f, winSize.height*0.5f));
            gameLayer->addChild(pc->background_1, Z_ORDER::PARALLAX_NODE_BACKGROUND);
            gameLayer->addChild(pc->background_2, Z_ORDER::PARALLAX_NODE_BACKGROUND);
            
            if (pc->foreground_1 != nullptr)
            {
                pc->foreground_1->setPosition(Vec2(winSize.width * 1.3f, winSize.height*0.6f));
                gameLayer->addChild(pc->foreground_1, Z_ORDER::PARALLAX_NODE_FOREGROUND);
            }
            
        }
    }
    else CCLOG("[!] gameLayer: NULL REFERENCE, RenderSystem, parallaxComponentAddedEvent:");
}

void RenderSystem::receive(const ComponentAddedEvent<ProgressBarComponent> &progressBarAddedEvent)
{
	auto progressBarComponent = progressBarAddedEvent.component;

	auto director = Director::getInstance();
	auto currentScene = director->getRunningScene();
	auto gameLayer = currentScene->getChildByTag(GAME_LAYER);

	if (gameLayer)
	{
		auto contentSize = progressBarComponent->background->getContentSize();
		progressBarComponent->progressBar->setPosition(Vec2(contentSize.width / 2, contentSize.height / 2));
		progressBarComponent->background->addChild(progressBarComponent->progressBar);
		gameLayer->addChild(progressBarComponent->background);
	}
	else CCLOG("gameLayer: NULL REFERENCE, RenderSystem, spriteAddedEvent");
}

void RenderSystem::receive(const EntityDestroyedEvent &entityDestroyedEvent)
{
	auto entity = entityDestroyedEvent.entity;

	if (entity.has_component<SpriteComponent>() || entity.has_component<ProgressBarComponent>() || entity.has_component<ParallaxComponent>())
	{
		auto director = Director::getInstance();
		auto currentScene = director->getRunningScene();
		auto gameLayer = currentScene->getChildByTag(GAME_LAYER);

		if (gameLayer)
		{
			if (entity.has_component<SpriteComponent>())
			{
				auto spriteComponent = entity.component<SpriteComponent>();
				auto sprite = spriteComponent->sprite;

				auto batchNode = gameLayer->getChildByTag(MAIN_SPRITEBATCHNODE);
				if (batchNode)
				{
					batchNode->removeChild(sprite);
				}
				else CCLOG("batchNode: NULL REFERENCE, RenderSystem, entityRemovedEvent");
			}

			if (entity.has_component<ProgressBarComponent>())
			{
				auto progressBarComponent = entity.component<ProgressBarComponent>();

				gameLayer->removeChild(progressBarComponent->background);
			}
            
            if (entity.has_component<ParallaxComponent>())
            {
                auto pn = entity.component<ParallaxComponent>();
                gameLayer->removeChild(pn->background_1);
                gameLayer->removeChild(pn->background_2);
                //remove foreground if present
                if(pn->foreground_1 != nullptr)
                {
                    gameLayer->removeChild(pn->foreground_1);
                }
            }

		}
		else CCLOG("gameLayer: NULL REFERENCE, RenderSystem, entityRemovedEvent");
	}
}

void RenderSystem::update(EntityManager &es, EventManager &events, double dt)
{
		
}
