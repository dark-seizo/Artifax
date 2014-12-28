#ifndef _CONSTANTS___	
#define _CONSTANTS___

#include <map>
#include <math.h>
#include "cocos2d.h"

const static unsigned int GAME_LAYER						= 9129;
const static unsigned int MAIN_SPRITEBATCHNODE				= 103410;
const static unsigned int PLAYER_TAG                        =191;

//namespace is used because the enum names are used elsewhere in the global namespace
namespace Z_ORDER
{
	enum Z_ORDER
	{
		UPGRADE                         = 1,
		ENEMY_PROJECTILE                = 2,
		PLAYER_PROJECTILE               = 3,
		PLAYER                          = 4,
		ENEMY                           = 5,
		UI_ELEMENT                      = 10,
        PARALLAX_NODE_BACKGROUND        = -10,
        PARALLAX_NODE_FOREGROUND        = -9
	};
}

inline float degreeToRadian(float degree) { return degree * (M_PI / 180); }
inline float radianToDegree(float radian) { return radian * (180 / M_PI); }

inline std::string removeExtension(const std::string &filename)
{
    size_t lastdot = filename.find_last_of(".");
    if(lastdot == std::string::npos)
        return filename;
    return filename.substr(0, lastdot);
}

inline float generateRandomFloat(float small, float large)
{
    float diff = large - small;
    return ((float)rand() / RAND_MAX) * diff + small;
}

template <class Map, class Val>
typename Map::const_iterator MapSearchByValue(const Map & SearchMap, const Val & SearchVal)
{
    typename Map::const_iterator iRet = SearchMap.end();
    for (typename Map::const_iterator iTer = SearchMap.begin(); iTer != SearchMap.end(); iTer ++)
    {
        if (iTer->second == SearchVal)
        {
            iRet = iTer;
            break;
        }
    }
    return iRet;
}

const static float BASE_LASER_SPEED							= 1000.f;

const static std::map<std::string, cocos2d::Vec2> bullet_velocity_map =
{
    { "player_straight_shot",		cocos2d::Vec2(BASE_LASER_SPEED, 0)	},
    { "player_20_up",				cocos2d::Vec2(BASE_LASER_SPEED, 20)	},
    { "player_20_down",				cocos2d::Vec2(BASE_LASER_SPEED, -20) },
    { "player_25_up",				cocos2d::Vec2(BASE_LASER_SPEED, 25)	},
    { "player_25_down",				cocos2d::Vec2(BASE_LASER_SPEED, -25) },
    { "enemy_straight_shot",        cocos2d::Vec2(-1, 180) }
};

#endif