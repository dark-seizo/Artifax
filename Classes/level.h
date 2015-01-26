#ifndef _LEVEL_H__
#define _LEVEL_H__

#include <list>
#include <map>
#include "entityx/entityx.h"
#include "SimpleAudioEngine.h"
#include "tinyxml2.h"
#include "render_system.h"
#include "input_system.h"
#include "bullet_system.h"
#include "movement_system.h"
#include "weapon_system.h"
#include "energybar_system.h"
#include "enemy_movementAI_system.h"
#include "enemy_group_system.h"
#include "collision_system.h"
#include "shield_system.h"
#include "enemy_health_system.h"
#include "parallax_system.h"
#include "upgrade_system.h"
#include "components.h"
#include "constants.h"

#include "json/document.h"

USING_NS_CC;
using namespace entityx;
using namespace tinyxml2;
using namespace rapidjson;


struct EnemyData
{
	EnemyData(EnemyType new_type, UpgradeDrop new_drop = UpgradeDrop::NO_UPGRADE, bool new_rotatable = true)
		: type(new_type), drop(new_drop), rotatable(new_rotatable) {}

	EnemyData() : drop(UpgradeDrop::NO_UPGRADE), rotatable(true) {}

	UpgradeDrop					drop;
	EnemyType					type;
    bool                        rotatable;
};

struct GroupData
{
	GroupData(std::string &new_path, float new_delay, float new_enemySpeed, float new_padding, UpgradeDrop new_upgradeDrop, bool new_rotatable)
		: path(new_path), delay(new_delay), enemySpeed(new_enemySpeed), padding(new_padding), upgradeDrop(new_upgradeDrop), rotatable(new_rotatable) {}
	
	float						delay;
	float						padding;
	float						enemySpeed;
    bool                        rotatable;
	std::string					path;
	std::list<EnemyData>		enemyData;
	UpgradeDrop					upgradeDrop;
};

struct WaveData
{
	WaveData(float new_delay = 0.0f) : delay(new_delay) {}

	float						delay;
	std::list<GroupData>		groupData;
};

struct LevelData
{
	std::list<WaveData>			spawnData;
	std::vector<std::string>	bossData;
	std::string					mainBgm;
	std::string					bossBgm;
};



class Level : EntityX
{
public:
	explicit Level(const std::string &filename);
	~Level();

	//main update method for levels
	void update(float dt);

private:
	//Loads the level xml file
	void loadLevelFile(const std::string  &filename);

	//creates the player's entity
	entityx::Entity createPlayer();

	//create an enemy entity from an EnemyData
	entityx::Entity createEnemyFromEnemyData(const EnemyData &data, std::string path, float speed);

	//create a group of enemies from a GroupData
	void createGroupFromGroupData(const GroupData &data);

	//create a players energy bar entity
	void createPlayerEnergyBar(entityx::Entity player);
    
    //create a players shield bar
    void createPlayerShieldBar(entityx::Entity player);

	//finds the correct position to set the new enemy
	Vec2 setInitialPosition(entityx::Entity entity);
    
    //creates the background entity for the current level
    void createParallaxBackground(std::string &background, std::string &foreground);
    
    //loads the enemy definitions from enemies.json
    void loadEnemyDefinitions();
    
    //retrieves player's ships constant's, storing them in a map.
    std::map<const std::string, float> loadPlayerShipValues(void);
    
	//holds the data parsed from the level file
	LevelData					*levelData;
    
    //holds the information for each individual enemy
    rapidjson::Document         enemyDefinitions;
};

#endif
