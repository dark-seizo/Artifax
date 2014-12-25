#ifndef __COMPONENT_S_
#define __COMPONENT_S_

#include <functional>
#include "entityx/entityx.h"
#include "cocos2d.h"
#include "constants.h"

USING_NS_CC;
using namespace entityx;


struct VelocityComponent : entityx::Component < VelocityComponent >
{
	VelocityComponent(float x, float y) : velocity(Vec2(x, y)) {}
	VelocityComponent(Vec2 new_velocity) : velocity(new_velocity) {}
	VelocityComponent() : velocity(Vec2(0, 0)) {}

	Vec2					velocity;
};

class CollidibleSprite : public cocos2d::Sprite
{
private:
    entityx::Entity         entity;
public:
    CollidibleSprite() {}
    
    static CollidibleSprite* createWithSpriteFrameName(const std::string &new_filename)
    {
        CollidibleSprite *sprite = new CollidibleSprite();
        if(sprite && sprite->initWithSpriteFrameName(new_filename))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return NULL;
    }
    
    inline void setEntity(const entityx::Entity new_entity) { entity = new_entity; }
    inline entityx::Entity getEntity() const { return entity; }
};

struct SpriteComponent : entityx::Component < SpriteComponent >
{
	SpriteComponent(const std::string &new_filename)
	{
		sprite = CollidibleSprite::createWithSpriteFrameName(new_filename);
        sprite->setName(removeExtension(new_filename));
	}

	SpriteComponent(std::string &new_filename)
	{
		sprite = CollidibleSprite::createWithSpriteFrameName(new_filename);
        sprite->setName(removeExtension(new_filename));
	}
    
	SpriteComponent() 
	{
		//need some form of error handling here
		sprite = (CollidibleSprite*)CollidibleSprite::create();
	};

	CollidibleSprite                  *sprite;
};

enum class CollisionType : int
{
	PLAYER_PROJECTILE       = 0x10000001,
	PLAYER_TURRET           = 0x10000010,
	ENEMY_PROJECTILE        = 0x00010000,
	UPGRADE                 = 0x01110000,
	PLAYER                  = 0x10000000,
	ENEMY                   = 0x11010000,
	NO_COLLISION            = 0x11111111
};

struct CollisionComponent : entityx::Component < CollisionComponent >
{
	CollisionComponent(CollisionType new_type) : type(new_type) {};
	CollisionComponent() : type(CollisionType::NO_COLLISION) {};

	CollisionType			type;
};

enum class PathType
{
	FUNCTION,
	POLAR
};

struct EnemyPath
{
	EnemyPath(PathType new_type, std::function<float(float)> new_function)
		: type(new_type), function(new_function) {}

	EnemyPath() {}

	PathType						type;
	std::function<float(float)>		function;
};


struct PathComponent : entityx::Component < PathComponent >
{
	PathComponent(const EnemyPath new_path) //by value, since they are small
		: path(new_path) {}
	//PathComponent() : path(nullptr) {}

	EnemyPath				path;
};

//tag component, used to denote the player's entity.
struct PlayerComponent : entityx::Component < PlayerComponent > 
{
	PlayerComponent() {};
};

struct ShieldComponent : entityx::Component < ShieldComponent > 
{
	ShieldComponent(float new_maxStrength, float new_rechargeRate, float new_rechargeDelay, float new_depletedDelay)
		: strength(new_maxStrength), maxStrength(new_maxStrength), rechargeRate(new_rechargeRate), rechargeDelay(new_rechargeDelay),
		depletedDelay(new_depletedDelay), rechargeTimeLeft(0.0f), active(true) {};

	ShieldComponent() : strength(0.f), maxStrength(1.f), rechargeRate(1.f), rechargeDelay(1.f), rechargeTimeLeft(0.f), depletedDelay(1.f), active(false) {};
	
	float					strength,
							maxStrength,
							rechargeRate,
                            rechargeTimeLeft,
							rechargeDelay,
							depletedDelay;

	bool					active;

	entityx::Entity			displayEntity;
};

struct EnergyBarComponent : entityx::Component < EnergyBarComponent >
{
	EnergyBarComponent(float new_maxEnergyLevel, float new_rechargeRate, float new_rechargeDelay)
		: energyLevel(new_maxEnergyLevel), maxEnergyLevel(new_maxEnergyLevel), rechargeRate(new_rechargeRate),
		rechargeDelay(new_rechargeDelay), rechargeTimeLeft(0.0f), depleted(false) {};

	EnergyBarComponent()
	{
		energyLevel = maxEnergyLevel = rechargeRate = rechargeDelay = 0.f;
		depleted = false;
	}

	float					energyLevel,
							maxEnergyLevel,
							rechargeRate,
							rechargeTimeLeft,
							rechargeDelay;

	bool					depleted;

	entityx::Entity			displayEntity;
};

typedef std::map<const std::string, cocos2d::EventKeyboard::KeyCode> InputMap;
typedef std::pair<const std::string, cocos2d::EventKeyboard::KeyCode> InputPair;
typedef std::map<const std::string, bool> InputKeyPressedMap;

struct InputComponent : entityx::Component < InputComponent > 
{
	InputComponent(InputMap keyMap) : InputComponent()
	{
		for (auto e : keyMap)
		{
			inputMap.insert(e);
		}
	}

	InputComponent() 
	{
		keyPressedMap.insert({ "up", false });
		keyPressedMap.insert({ "down", false });
		keyPressedMap.insert({ "left", false });
		keyPressedMap.insert({ "right", false });
		keyPressedMap.insert({ "fire", false });
	}

	InputMap			inputMap;
	InputKeyPressedMap	keyPressedMap;
};

struct LaserWeapon
{
	enum LaserWeaponLevel
	{
		LASER_INACTIVE			= -1,
		LASER_LEVEL_ONE			= 0,
		LASER_LEVEL_TWO			= 1,
		LASER_LEVEL_THREE		= 2,
		LASER_LEVEL_FOUR		= 3,

		LASER_MAX_LEVEL			= LASER_LEVEL_FOUR
	} level;


	LaserWeapon(LaserWeaponLevel new_level, float new_power) : level(new_level), power(new_power), delay(0.0f) {}
	LaserWeapon() : level(LASER_INACTIVE), power(1.f), delay(0.0f) {}

	inline void increaseLevel() { if (level != LASER_MAX_LEVEL) level = LaserWeaponLevel(level + 1); }
	inline const bool isReady() { return delay <= 0; }

    float		delay,
                power;
};

struct MissileWeapon
{
	enum MissileWeaponLevel
	{
		MISSILE_INACTIVE		= -1,
		MISSILE_LEVEL_ONE		= 0,
		MISSILE_LEVEL_TWO		= 1,
		MISSILE_LEVEL_THREE		= 2,

		MISSILE_MAX_LEVEL		= MISSILE_LEVEL_THREE
	} level;

	MissileWeapon(MissileWeaponLevel new_level, float new_power) : level(new_level), power(new_power), delay(0.0f) {}
	MissileWeapon() : level(MissileWeapon::MISSILE_INACTIVE), power(1.f), delay(0.0f) {}

	inline void increaseLevel() { if (level != MISSILE_MAX_LEVEL) level = MissileWeaponLevel(level + 1); }
	inline bool isReady() const { return delay <= 0; }

    float		delay,
                power;
};

struct TurretWeapon
{
	enum TurretWeaponLevel
	{
		TURRET_INACTIVE = -1,
		TURRET_LEVEL_ONE = 0,
		TURRET_LEVEL_TWO = 1,
		TURRET_LEVEL_THREE = 2,

		TURRET_MAX_LEVEL = TURRET_LEVEL_THREE
	} turretWeaponLevel;


};

struct WeaponComponent : entityx::Component < WeaponComponent >
{
	WeaponComponent() : laser(LaserWeapon::LASER_LEVEL_ONE, PLAYER_LASER_POWER), missile(MissileWeapon::MISSILE_INACTIVE, PLAYER_MISSILE_POWER), turret(0), turretDelay(0)  {}

    unsigned short			turret;
    float					turretDelay;
	LaserWeapon				laser;
	MissileWeapon			missile;
};

struct BoundaryComponent : entityx::Component < BoundaryComponent >
{
    BoundaryComponent(cocos2d::Rect new_rect) : boundary(new_rect) {}
    BoundaryComponent() : boundary(cocos2d::Rect(0,0,1,1)) {}
    
    cocos2d::Rect           boundary;
};

enum class BulletType
{
	SMALL_SHOT,
	MEDIUM_SHOT,
	LARGE_SHOT,
	PLAYER_MISSILE,
	PLAYER_LASER,
	NO_BULLET_TYPE
};

inline const std::string bulletToString(BulletType type)
{
	switch (type)
	{
	case BulletType::SMALL_SHOT:
		return "smallshot1.png";
	case BulletType::MEDIUM_SHOT:
		return "mediumshot1.png";
	case BulletType::LARGE_SHOT:
		return "largeshot1.png";
	case BulletType::PLAYER_MISSILE:
		return "playermissile.png";
	case BulletType::PLAYER_LASER:
		return "playerlaser1.png";
	default:
		//need better error handling here
		CCLOG("[!] attempted to query string for undefined bullet type.");
		return "err";
		break;
	}
}

struct BulletComponent : entityx::Component < BulletComponent >
{
	BulletComponent(BulletType new_type, float new_power) : type(new_type), power(new_power) {}
	BulletComponent() : type(BulletType::NO_BULLET_TYPE), power(1.f) {};

	BulletType					type;
    float                       power;
};


//used for targeted actions against another entity
struct TargetedComponent : entityx::Component < TargetedComponent >
{
	TargetedComponent(entityx::Entity new_target) : target(new_target) {};

	entityx::Entity				target;
};

struct ProgressBarComponent : entityx::Component < ProgressBarComponent >
{
	ProgressBarComponent(const std::string &fileName)
	{
		//split the filename string and add in background for initializing the background
		background = CCSprite::createWithSpriteFrameName("progressbar_background.png");
		progressBar = ProgressTimer::create(CCSprite::createWithSpriteFrameName(fileName));
	}

	cocos2d::Sprite             *background;
	cocos2d::ProgressTimer		*progressBar;
};

struct ParallaxComponent : entityx::Component < ParallaxComponent >
{
    ParallaxComponent(const std::string &backgroundFileName, const std::string &foregroundFileName = "")
    {
        background_1 = Sprite::create(backgroundFileName);
        background_2 = Sprite::create(backgroundFileName);
        if(foregroundFileName != "")
        {
            foreground_1 = Sprite::create(foregroundFileName);
        }
    }
    
    Sprite                      *background_1,
                                *background_2,
                                *foreground_1;
};

enum class EnemyType
{
	SMALL_ENEMY,
	MEDIUM_ENEMY,
	LARGE_ENEMY
};
    
    const static std::map<std::string, EnemyType> enemyNameMap =
    {
        { "smallenemy", EnemyType::SMALL_ENEMY },
        { "mediumenemy", EnemyType::MEDIUM_ENEMY },
        { "largeenemy", EnemyType::LARGE_ENEMY }
    };


//figure out a better solution here
inline const std::string enemyToString(EnemyType type)
{
	switch (type)
	{
	case EnemyType::SMALL_ENEMY:
		return "smallenemy1.png";
	case EnemyType::MEDIUM_ENEMY:
		return "mediumenemy1.png";
	case EnemyType::LARGE_ENEMY:
		return "largeenemy1.png";
	default:
		//need better error handling
		CCLOG("[!] unrecognized enemy type in enemyToString(EnemyType)");
		break;
	}
}
/*
//need better solution here too...
inline const EnemyType stringToEnemyType(const char* string)
{
	if (strcmp(string, "smallenemy") == 0)			return EnemyType::SMALL_ENEMY;
	else if (strcmp(string, "mediumenemy") == 0)	return EnemyType::MEDIUM_ENEMY;
	else if (strcmp(string, "largeenemy") == 0)		return EnemyType::LARGE_ENEMY;
	else											return EnemyType::NO_TYPE;
	//error handling!
};
*/

enum class UpgradeDrop
{
	LASER,
	MISSILE,
	TURRET,
	OVERSHIELD,

	NO_UPGRADE,
};

//better solutions are better
inline const UpgradeDrop stringToUpgradeDrop(const char * string)
{
	if (strcmp(string, "laserupgrade") == 0)				return UpgradeDrop::LASER;
	else if (strcmp(string, "missileupgrade") == 0)		return UpgradeDrop::MISSILE;
	else if (strcmp(string, "turretupgrade") == 0)			return UpgradeDrop::TURRET;
	else if (strcmp(string, "shieldupgrade") == 0)		return UpgradeDrop::OVERSHIELD;
	else											return UpgradeDrop::NO_UPGRADE;
}

    const static std::map<std::string, UpgradeDrop> upgradeMap =
    {
        { "laserupgrade", UpgradeDrop::LASER },
        { "missileupgrade", UpgradeDrop::MISSILE },
        { "turretupgrade", UpgradeDrop::TURRET },
        { "shieldupgrade", UpgradeDrop::OVERSHIELD },
        { "noupgrade", UpgradeDrop::NO_UPGRADE }
    };

struct UpgradeComponent : public entityx::Component < UpgradeComponent >
{
	UpgradeComponent(const UpgradeDrop new_drop = UpgradeDrop::NO_UPGRADE) : drop(new_drop) {}

	UpgradeDrop					drop;
};

struct EnemyComponent : public entityx::Component < EnemyComponent >
{
	EnemyComponent(EnemyType new_type, float new_speed, bool new_rotatable = true) : type(new_type), speed(new_speed), rotatable(new_rotatable) {}
	EnemyComponent(EnemyType new_type) : type(new_type), speed(100.f), rotatable(false) {}
	//EnemyComponent() : type(EnemyType::NO_TYPE) {};

	EnemyType					type;
	float						speed;
    bool                        rotatable;
};

struct EnemyHealthComponent : public entityx::Component < EnemyHealthComponent >
{
	EnemyHealthComponent(float new_health) : health(new_health), maxHealth(new_health) {}
	EnemyHealthComponent() : health(10.f), maxHealth(10.f) {}

	float						health,
								maxHealth;
    
    //used for bosses or any enemy that wants a way to display their health
    entityx::Entity             displayEntity;
};


enum class EnemyWeaponType : int
{
    STANDARD        = 0, //a straight shot across the screen
    UPGRADE         = 1, //a standard shot with two other shots +-25 degrees from horizontal
    TARGETED        = 2  //seeks the nearest player and shoots at them directly, wherever they are
};

struct EnemyWeaponComponent : public entityx::Component < EnemyWeaponComponent >
{
    EnemyWeaponComponent(float new_attackPower, float new_frequency, float new_frequencyMin, float new_frequencyMax, float new_speed, EnemyWeaponType new_type) : attackPower(new_attackPower), frequencyOfAttack(new_frequency), frequencyDeltaMin(new_frequencyMin), frequencyDeltaMax(new_frequencyMax), speed(new_speed), attackDelayRemaining(new_frequency), type(new_type) {}
    
    float                       attackPower,
                                frequencyOfAttack,
                                frequencyDeltaMin,
                                frequencyDeltaMax,
                                speed,
                                attackDelayRemaining;
    EnemyWeaponType             type;
};

struct EnemyGroupComponent : public entityx::Component < EnemyGroupComponent >
{
	EnemyGroupComponent() {}

	std::list<entityx::Entity>	groupEnemies;
};

#endif