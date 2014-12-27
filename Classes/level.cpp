#include "level.h"

Level::Level( const std::string &filename )
{
	levelData = new LevelData();

	//add systems here
	systems.add<RenderSystem>();
	auto inputSystem = systems.add<InputSystem>();

	//the bulletsystem needs a reference to the entity manager to use it outside of its update method
	systems.add<BulletSystem>(entities);
	systems.add<MovementSystem>();
	systems.add<WeaponSystem>(entities);
	systems.add<EnergyBarSystem>();
	systems.add<EnemyMovementAISystem>();
	systems.add<EnemyGroupSystem>();
	systems.add<CollisionSystem>();
    systems.add<ShieldSystem>();
    systems.add<EnemyHealthSystem>(entities);
    systems.add<ParallaxScrollingSystem>();
    systems.add<UpgradeSystem>(entities);

	//calls configure on all systems
	systems.configure();
    
	auto player = this->createPlayer();
    
	this->createPlayerEnergyBar(player);
    this->createPlayerShieldBar(player);

	this->loadLevelFile(filename);
    this->loadEnemyDefinitions();
    
    //preload and play bgm music
    std::string bgmPath("bgm/" + levelData->mainBgm);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(bgmPath.c_str());
    //begin the music at some other point after the level has been initiated.
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(bgmPath.c_str());
}

Level::~Level()
{
	delete levelData;
}

void Level::update(float dt)
{
	if (!levelData->spawnData.empty())
	{
		auto &currentWave = levelData->spawnData.front();
		if (currentWave.delay <= 0)
		{
			//need to check for group delta and spawn the groups when it is < 0.
			//when all groups have been spawned, remove the wave from the spawnData
			std::list<GroupData>::iterator iter = currentWave.groupData.begin();
			std::list<GroupData>::iterator end = currentWave.groupData.end();
			while (iter != end)
			{
				if (iter->delay <= 0)
				{
					this->createGroupFromGroupData(*iter);
					//erase the groups data after it has been spawned
					currentWave.groupData.erase(iter++);
				}
				else
				{
					iter->delay -= dt;
					++iter;
				}
			}
		}
		else
		{
			currentWave.delay -= dt;
		}

		if (currentWave.groupData.empty())
		{
			levelData->spawnData.pop_front();
		}
	}

    systems.update<EnemyGroupSystem>(dt);
	systems.update<RenderSystem>(dt);
	systems.update<InputSystem>(dt);
	systems.update<CollisionSystem>(dt);
    systems.update<ShieldSystem>(dt);
    systems.update<EnemyHealthSystem>(dt);
	systems.update<WeaponSystem>(dt);
	systems.update<EnergyBarSystem>(dt);
	systems.update<BulletSystem>(dt);
	systems.update<MovementSystem>(dt);
	systems.update<EnemyMovementAISystem>(dt);
    systems.update<ParallaxScrollingSystem>(dt);
    systems.update<UpgradeSystem>(dt);
}

void Level::createParallaxBackground(std::string &background, std::string &foreground)
{
    auto entity = entities.create();
    entity.assign<ParallaxComponent>(background, foreground);
}

entityx::Entity Level::createEnemyFromEnemyData(const EnemyData &data, std::string path, float speed)
{
	auto entity = entities.create();
	entity.assign<UpgradeComponent>(data.drop);
	entity.assign<EnemyComponent>(data.type, speed, data.rotatable);
	entity.assign<PathComponent>(enemyPathMap.at(path));
	
	float healthPool = 1.f;
    float attackPower = 1.f;
    float attackSpeed = 1.f;
    float attackFrequency = 1.f;
    float frequencyDeltaMin = 1.f, frequencyDeltaMax = 1.f;
    EnemyWeaponType attackType;
    
    const std::string def = MapSearchByValue<std::map<std::string, EnemyType>, EnemyType>(enemyNameMap, data.type)->first;
    
    auto &enemyDefinition = enemyDefinitions[def.c_str()];
    
    if (enemyDefinition.IsObject())
    {
        healthPool = enemyDefinition["healthPool"].GetDouble();
        attackPower = enemyDefinition["attackPower"].GetDouble();
        attackSpeed = enemyDefinition["attackSpeed"].GetDouble();
        attackFrequency = enemyDefinition["attackFrequency"].GetDouble();
        frequencyDeltaMin = enemyDefinition["frequencyDeltaMin"].GetDouble();
        frequencyDeltaMax = enemyDefinition["frequencyDeltaMax"].GetDouble();
        attackType = (EnemyWeaponType)enemyDefinition["attackType"].GetInt();
    }
    else
    {
        //ERROR HANDLING
        CCLOG("createEnemyFromData: Unable to find enemy definition");
    }
    
    entity.assign<EnemyWeaponComponent>(attackPower, attackFrequency, frequencyDeltaMin, frequencyDeltaMax, attackSpeed, attackType);
	entity.assign<EnemyHealthComponent>(healthPool);

	//entity.assign<VelocityComponent>();
	entity.assign<SpriteComponent>(enemyToString(data.type));
    entity.assign<CollisionComponent>(CollisionType::ENEMY);
	//enemies position is set in the group creation methods
	return entity;
}

void Level::loadEnemyDefinitions()
{
    auto content = FileUtils::getInstance()->getDataFromFile("data/enemies.json");
    std::string contentString((const char*)content.getBytes(), content.getSize());
    
    enemyDefinitions.Parse<rapidjson::kParseDefaultFlags>(contentString.c_str());
    
    if(enemyDefinitions.HasParseError())
    {
        //throw exception, enemy definitions was unreadable
        CCLOG("error reading enemy definitions file, %s", enemyDefinitions.GetParseError());
    }
}

void Level::createGroupFromGroupData(const GroupData &data)
{
	auto groupEntity = entities.create();
	groupEntity.assign<EnemyGroupComponent>();
	groupEntity.assign<UpgradeComponent>(data.upgradeDrop);

	auto winSize = Director::getInstance()->getWinSize();

	//wauto groupEnemies = &groupEntity.component<EnemyGroupComponent>()->groupEnemies;
	auto enemies = data.enemyData;

	if (!enemies.empty())
	{
		//take the first enemy and determine where it should be placed for its first position
 		auto entity = this->createEnemyFromEnemyData(enemies.front(), data.path, data.enemySpeed);
		auto sprite = entity.component<SpriteComponent>()->sprite;
		Vec2 initialPosition = this->setInitialPosition(entity);
		groupEntity.component<EnemyGroupComponent>()->groupEnemies.push_back(entity);

		//since we are always going to have a previous enemy, we can work with its contents size for padding the distance between each enemy
		float previousEnemyWidth = sprite->getContentSize().width;

		std::list<EnemyData>::iterator iter = ++enemies.begin();
		std::list<EnemyData>::iterator end = enemies.end();
		while (iter != end)
		{
			auto entity = this->createEnemyFromEnemyData(*iter, data.path, data.enemySpeed);
			auto sprite = entity.component<SpriteComponent>()->sprite;
			float new_x = initialPosition.x + previousEnemyWidth / 2 + sprite->getContentSize().width / 2 + data.padding;
			float new_y = entity.component<PathComponent>()->path.function(new_x);

			//now set the previous content size to the new enemy, and set the position
			sprite->setPosition(Vec2(new_x, new_y));
			groupEntity.component<EnemyGroupComponent>()->groupEnemies.push_back(entity);

			previousEnemyWidth = sprite->getContentSize().width;
			initialPosition = Vec2(new_x, new_y);
			++iter;
		}
	}
}

Vec2 Level::setInitialPosition(entityx::Entity entity)
{
	if (entity.has_component<SpriteComponent>() && entity.has_component<PathComponent>() && entity.has_component<EnemyComponent>())
	{
        float x_delta = 10.0f;
        
        auto determineRotation = [&x_delta] (entityx::Entity entity)
        {
            if (entity.component<EnemyComponent>()->rotatable) {
                cocos2d::Sprite *sprite = entity.component<SpriteComponent>()->sprite;
                float initial_x = sprite->getPositionX();
                float initial_y = sprite->getPositionY();
                
                float rotation = 0.f;
                switch (entity.component<PathComponent>()->path.type)
                {
                    case PathType::FUNCTION:
                    {
                        float newX = initial_x - x_delta;
                        float newY = entity.component<PathComponent>()->path.function(newX);
                        rotation = atanf((newY - initial_y) / (newX - initial_x));
                        break;
                    }
                        
                    case PathType::POLAR:
                    {
                        rotation = initial_x;
                        break;
                    }
                }
                return -(radianToDegree(rotation));
            }
            else
            {
                return 0.0f;
            }
        };

		auto winSize = Director::getInstance()->getWinSize();
		cocos2d::Rect winRect(0, 0, winSize.width, winSize.height);

		auto sprite = entity.component<SpriteComponent>()->sprite;
		auto contentSize = sprite->getContentSize();
		float initial_x = winSize.width + contentSize.width / 2 + 1.f;
		float initial_y = entity.component<PathComponent>()->path.function(initial_x);
		sprite->setPosition(Vec2(initial_x, initial_y));
        sprite->setRotation(determineRotation(entity));
        
		auto pos = sprite->getPosition();
        
        cocos2d::Rect spriteRect = sprite->getBoundingBox();

		float x_multiple = 0.f;
		bool multiplied = false;
		while (!winRect.intersectsRect(spriteRect))
		{
			multiplied = true;
			++x_multiple;
			float new_x = initial_x - (x_delta * x_multiple);
			float new_y = entity.component<PathComponent>()->path.function(new_x);
			sprite->setPosition(Vec2(new_x, new_y));
            sprite->setRotation(determineRotation(entity));
			
            //reset bounding box, may not need to do this.
            spriteRect = sprite->getBoundingBox();
		}
		auto final_pos = sprite->getPosition();

		//the position in which the sprite is right on the screen has been found, return the final position
		//float final_x = multiplied ? final_pos.x : final_pos.x - (x_delta * (x_multiple - 1.f));
        float final_x = final_pos.x;
		float final_y = entity.component<PathComponent>()->path.function(final_x);
		sprite->setPosition(Vec2(final_x, final_y));
		return Vec2(final_x, final_y);
	}
	else CCLOG("[FATAL] Entity without sprite and path component and enemy component passed into Level::setInitialPosition");
}

entityx::Entity Level::createPlayer()
{
	auto director = Director::getInstance();
    Size visibleSize = director->getWinSize();
	Vec2 origin = director->getVisibleOrigin();

	auto entity = entities.create();

	auto spriteComponent = entity.assign<SpriteComponent>("player.png");
	auto sprite = spriteComponent->sprite;
	sprite->setZOrder(Z_ORDER::PLAYER);
	auto contentSize = sprite->getContentSize();

	float width_padding = 15.f;
	float player_x = origin.x + contentSize.width / 2 + width_padding;
	float player_y = origin.y + visibleSize.height / 2;

	spriteComponent->sprite->setPosition(Vec2(player_x, player_y));

	entity.assign<PlayerComponent>();

	InputMap keyMap =
	{
		{ "up", EventKeyboard::KeyCode::KEY_W },
		{ "down", EventKeyboard::KeyCode::KEY_S },
		{ "left", EventKeyboard::KeyCode::KEY_A },
		{ "right", EventKeyboard::KeyCode::KEY_D },
		{ "fire", EventKeyboard::KeyCode::KEY_SPACE }
	};

	entity.assign<InputComponent>(keyMap);
	entity.assign<ShieldComponent>(PLAYER_SHIELD_MAX_STRENGTH, PLAYER_SHIELD_RECHARGE_RATE, PLAYER_SHIELD_RECHARGE_DELAY, PLAYER_SHIELD_DEPLETED_DELAY);
	entity.assign<EnergyBarComponent>(PlAYER_ENERGYBAR_MAX_STRENGTH, PLAYER_ENERGYBAR_RECHARGE_RATE, PLAYER_ENERGYBAR_RECHARGE_DELAY);
	entity.assign<CollisionComponent>(CollisionType::PLAYER);
	entity.assign<WeaponComponent>();
	entity.assign<VelocityComponent>();
    entity.assign<BoundaryComponent>(cocos2d::Rect(0,0,visibleSize.width,visibleSize.height));
	
	return entity;
}

void Level::createPlayerEnergyBar(entityx::Entity player)
{
	auto director = Director::getInstance();
	Size visibleSize = director->getWinSize();

	auto entity = entities.create();
	
	auto progressBarComponent = entity.assign<ProgressBarComponent>("energybar.png");
	auto backgroundSprite = progressBarComponent->background;
	auto progressBar = progressBarComponent->progressBar;
	auto contentSizeBackground = backgroundSprite->getContentSize();

	backgroundSprite->setPosition(Vec2(contentSizeBackground.width / 2 + 10.f, visibleSize.height - contentSizeBackground.height / 2 - 5.0f));
	backgroundSprite->setZOrder(Z_ORDER::UI_ELEMENT);
	
	//progressBar->setPosition(Vec2(contentSizeBar.width / 2 + 15.f, ));
	progressBar->setType(ProgressTimerType::BAR);
	progressBar->setBarChangeRate(Vec2(1, 0));
	progressBar->setMidpoint(Vec2(0, 0.5f));
	progressBar->setPercentage(100.f);

	if (player.has_component<EnergyBarComponent>())
	{
		player.component<EnergyBarComponent>()->displayEntity = entity;
	}
	else CCLOG("player did not have energy component, did not set display entity");
}

void Level::createPlayerShieldBar(entityx::Entity player)
{
    auto director = Director::getInstance();
    Size winSize = director->getWinSize();
    
    auto entity = entities.create();
    
    auto shieldBarComponent = entity.assign<ProgressBarComponent>("shieldbar.png");
    auto backgroundSprite = shieldBarComponent->background;
    auto bar = shieldBarComponent->progressBar;
    auto contentSizeBackground = backgroundSprite->getContentSize();
    
    backgroundSprite->setPosition((contentSizeBackground.width * 1.5f + 20.f),winSize.height - contentSizeBackground.height / 2 - 5.f);
    backgroundSprite->setZOrder(Z_ORDER::UI_ELEMENT);
    
    bar->setType(ProgressTimerType::BAR);
    bar->setBarChangeRate(Vec2(1, 0));
	bar->setMidpoint(Vec2(0, 0.5f));
	bar->setPercentage(100.f);
    
    if(player.has_component<ShieldComponent>())
    {
        player.component<ShieldComponent>()->displayEntity = entity;
    }
    else CCLOG("player did not have shield component, did not set display entity");
}

void Level::loadLevelFile(const std::string &filename)
{
	std::string filePath = filename;
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filePath.c_str()) == XML_NO_ERROR)
	{
		XMLElement *mainNode = doc.FirstChildElement("level");
		if (mainNode)
		{
			//check if there is a bgm track, add it to the level data if there is
			if (mainNode->Attribute("bgm"))
			{
				levelData->mainBgm = mainNode->Attribute("bgm");
			}
            
            std::string bg = "";
            if (mainNode->Attribute("bg"))
            {
                bg = mainNode->Attribute("bg");
            }
            std::string fg = "";
            if (mainNode->Attribute("fg"))
            {
                fg = mainNode->Attribute("fg");
            }
            
            this->createParallaxBackground(bg, fg);
            
			mainNode = mainNode->FirstChildElement("wave");
			if (mainNode)
			{
				bool nextWaveNode = false;
				do
				{
					if (nextWaveNode)
					{
						mainNode = mainNode->NextSiblingElement("wave");
					}
					XMLElement *waveNode = mainNode->FirstChildElement("group");
					if (waveNode)
					{
						WaveData waveData(mainNode->FloatAttribute("delta"));
						do
						{
							XMLElement *groupNode = waveNode;
							std::string path = groupNode->Attribute("path");
							
							float delay = 0.0f;
							//check if there is a delay
							groupNode->QueryFloatAttribute("delta", &delay);

							//check if there is an upgrade
							bool upgrade = false;
							std::string upgradeString;
							if (groupNode->Attribute("upgrade"))
							{
								upgrade = true;
								upgradeString = groupNode->Attribute("upgrade");
							}
                            
                            //UpgradeDrop drop = upgradeMap.find(upgradeString)->second;
                            UpgradeDrop drop = stringToUpgradeDrop(upgradeString.c_str());

							//find the speed of the group
							float speed = 0.f;
							groupNode->QueryFloatAttribute("speed", &speed);

							//find the padding of the group, default value: 5.f
							float padding = 5.f;
							groupNode->QueryFloatAttribute("padding", &padding);
                            
                            //find if this group is rotatable
                            bool rotatable = true;
                            groupNode->QueryBoolAttribute("rotatable", &rotatable);

							GroupData groupData(path, delay, speed, padding, drop, rotatable);

							//test to see what type of group definition this is
							unsigned int numberOfEnemies;
							auto checkNumber = groupNode->QueryUnsignedAttribute("number", &numberOfEnemies);

							if ((checkNumber != XML_WRONG_ATTRIBUTE_TYPE) && (checkNumber != XML_NO_ATTRIBUTE))
							{
								//EnemyType type = stringToEnemyType(groupNode->Attribute("type"));
                                EnemyType type = enemyNameMap.find(groupNode->Attribute("type"))->second;
								for (size_t i = 0; i < numberOfEnemies; ++i)
								{
									EnemyData enemyData(type);
                                    enemyData.rotatable = rotatable;
									groupData.enemyData.push_back(enemyData);
								}
							}
							else //the group definition defines the enemies seperately
							{
								XMLElement *enemyNode = groupNode->FirstChildElement("enemy");
								if (enemyNode)
								{
									do
									{
										EnemyData enemyData;
                                        
                                        //set if the enemy group is rotatable
                                        enemyData.rotatable = rotatable;
                                        
										//check if we need to look for an upgrade
										if (!upgrade && enemyNode->Attribute("upgrade"))
										{
                                            //std::string upgradeString = enemyNode->Attribute("upgrade");
                                            //enemyData.drop = upgradeMap.find(upgradeString)->second;
											enemyData.drop = stringToUpgradeDrop(enemyNode->Attribute("upgrade"));

										}
										//check if there is an enemy type here
										if (enemyNode->Attribute("type"))
										{
											std::string typeString = enemyNode->Attribute("type");
											//enemyData.type = stringToEnemyType(typeString.c_str());
                                            enemyData.type = enemyNameMap.find(typeString)->second;
										}
										else
										{
#warning throw exception, enemy must have valid type
										}
										groupData.enemyData.push_back(enemyData);
									} while ((enemyNode = enemyNode->NextSiblingElement()));
								}
							}
							//add the configured groupData to the wave
							waveData.groupData.push_back(groupData);
						} while ((waveNode = waveNode->NextSiblingElement("group")));
						//if there was a wave, add it to the level here
						levelData->spawnData.push_back(waveData);
					}
					nextWaveNode = true;
				} while (mainNode->NextSiblingElement("wave"));
			}
			mainNode = mainNode->NextSiblingElement("bosswave"); //bosswave
			if (mainNode)
			{
				//check for a boss bgm
				if (mainNode->Attribute("bgm"))
				{
					levelData->bossBgm = mainNode->Attribute("bgm");
				}

				XMLElement *bossNode = mainNode->FirstChildElement("boss");
				if (bossNode)
				{
					do
					{
						//make sure the bossNode has the boss's name element, handle the error if it doesnt
						if (bossNode->Attribute("name"))
						{
							levelData->bossData.push_back(bossNode->Attribute("name"));
						}
						else CCLOG("[!] Boss node parsed without an identifier(name).");

					} while ((bossNode = bossNode->NextSiblingElement("boss")));
				}
			}
		}
	}
}