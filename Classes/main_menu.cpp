#include "main_menu.h"


Scene* MainMenu::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MainMenu::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool MainMenu::init()
{
	if (!Layer::init()) return false;

	//Size visibleSize = Director::getInstance()->getVisibleSize();
    auto winSize = Director::getInstance()->getWinSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto playGame = MenuItemFont::create("Play Game", CC_CALLBACK_1(MainMenu::playGameClicked, this));
    auto quitGame = MenuItemFont::create("Quit", CC_CALLBACK_1(MainMenu::quitGameClicked, this));

	//testLevel->setPosition(Vec2(origin.x + visibleSize.width - testLevel->getContentSize().width / 2,
		//origin.y + testLevel->getContentSize().height / 2));

	auto menu = Menu::create(playGame,quitGame, NULL);
    menu->alignItemsVertically();
    //auto menuContentSize = menu->getContentSize();
	menu->setPosition(winSize.width/2, winSize.height/2);
	this->addChild(menu);

	return true;
}

void MainMenu::playGameClicked(cocos2d::Ref* pSender)
{
	Director::getInstance()->pushScene(Game::createScene());
}

void MainMenu::quitGameClicked(cocos2d::Ref* pSender)
{
    Director::getInstance()->end();
}