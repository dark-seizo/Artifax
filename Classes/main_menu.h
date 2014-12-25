#ifndef _MAIN_MENU__
#define _MAIN_MENU__

#include "cocos2d.h"
#include "game.h"
#include "level.h"

USING_NS_CC;

class MainMenu : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void playGameClicked(cocos2d::Ref* pSender);
    void quitGameClicked(cocos2d::Ref* pSender);

	CREATE_FUNC(MainMenu);
};

#endif