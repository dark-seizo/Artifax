#ifndef _GAME__
#define _GAME__

#include "cocos2d.h"
#include "level.h"
#include "constants.h"

USING_NS_CC;

class Game : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void update(float dt);

	CREATE_FUNC(Game);

private:
	void setupLevel(float);

	Level				*currentLevel = nullptr;
};

#endif