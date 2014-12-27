#include "game.h"

Scene* Game::createScene()
{
	// 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();

	// 'layer' is an autorelease object
	auto layer = Game::create();
	layer->setTag(GAME_LAYER);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Game::init()
{

	if (!Layer::init()) return false;

	auto mainBatchNode = SpriteBatchNode::create("data/mainsheet.png");
	mainBatchNode->setTag(MAIN_SPRITEBATCHNODE);
	this->addChild(mainBatchNode);

	//load the main sprite sheet	
	auto cache = SpriteFrameCache::getInstance();
	cache->addSpriteFramesWithFile("data/mainsheet.plist");
    
    auto animationCache = AnimationCache::getInstance();
    animationCache->addAnimationsWithFile("data/animations.plist");

	this->scheduleOnce(schedule_selector(Game::setupLevel), 0);

	return true;
}

void Game::setupLevel(float)
{
	currentLevel = new Level("levels/test.xml");
	this->scheduleUpdate();
}

void Game::update(float dt)
{
	currentLevel->update(dt);
}
