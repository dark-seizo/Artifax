#include "collision_system.h"

void CollisionSystem::configure(EventManager &events)
{
    events.subscribe<ComponentAddedEvent<CollisionComponent>>(*this);
    
    auto scene = Director::getInstance()->getRunningScene();
    auto world = scene->getPhysicsWorld();
    world->setGravity(Vec2::ZERO);
    //only draw debug if we are building on debug and if a flag is set.
    world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    MyBodyParser::getInstance()->parseJsonFile("data/physics_data.json");
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(CollisionSystem::onContactBegin, this);
    
    auto director = Director::getInstance();
    director->getEventDispatcher()->addEventListenerWithFixedPriority(contactListener, 1);
    
    eventManager = &events;
}

bool CollisionSystem::onContactBegin(PhysicsContact &contact)
{
    auto *nodeA = (CollidibleSprite*)(contact.getShapeA()->getBody()->getNode());
    auto *nodeB = (CollidibleSprite*)(contact.getShapeB()->getBody()->getNode());
    //auto entityA = nodeA->getEntity();
    //auto entityB = nodeB->getEntity();
    
    if(nodeA && nodeB)
    {
        auto entityA = nodeA->getEntity();
        auto entityB = nodeB->getEntity();
        
        auto check = [](ComponentHandle<CollisionComponent> ccA, ComponentHandle<CollisionComponent> ccB, CollisionType type) -> bool { return (ccA->type == type) && (ccB->type != type); };
        
        if (entityA.has_component<CollisionComponent>() && entityB.has_component<CollisionComponent>())
        {
            auto ccA = entityA.component<CollisionComponent>();
            auto ccB = entityB.component<CollisionComponent>();
            
            if (ccA->type == CollisionType::PLAYER || ccB->type == CollisionType::PLAYER)
            {
                //first we check which entity is the player, then make a variable switch to simplify the code following
                
                if(check(ccB,ccA,CollisionType::PLAYER))
                {
                    //switch variables
                    auto entityC = entityA;
                    entityA = entityB;
                    entityB = entityC;
                    
                    auto ccC = ccA;
                    ccA = ccB;
                    ccB = ccC;
                    
                }
                /*
                if (check(ccB,ccA,CollisionType::PLAYER)) {
                    varSwitch();
                }
                */
                switch (ccB->type)
                {
                    case CollisionType::ENEMY:
                    {
                        //CCLOG("Collision between enemy and player has fired.");
                        
                        auto power = entityB.component<EnemyWeaponComponent>()->attackPower;
                        //when a player collides with an enemy, they take damage equal to three times its attack power
                        eventManager->emit<ShieldHitEvent>(power * 3, entityA);
                        //collidible.destroy();
                        entityB.destroy();
                        break;
                    }
                    case CollisionType::ENEMY_PROJECTILE:
                    {
                        //CCLOG("Collision between enemy projectile and player has fired.");
                        auto power = entityB.component<BulletComponent>()->power;
                        eventManager->emit<ShieldHitEvent>(power, entityA);
                        entityB.destroy();
                        break;
                    }
                    case CollisionType::UPGRADE:
                    {
                        //CCLOG("Collision between upgrade and player has fired.");
                        //auto drop = entityB.component<UpgradeComponent>()->drop;
                        eventManager->emit<PlayerUpgradeCollisionEvent>(entityB, entityA);
                        break;
                    }
                        
                    default: break;
                }
                
            }
            else if (ccA->type == CollisionType::ENEMY || ccB->type == CollisionType::ENEMY)
            {
                if(check(ccB,ccA,CollisionType::ENEMY))
                /*
                {
                    varSwitch();
                }
                */
                
                {
                    //switch variables
                    auto entityC = entityA;
                    entityA = entityB;
                    entityB = entityC;
                    
                    auto ccC = ccA;
                    ccA = ccB;
                    ccB = ccC;
                }
                
                switch (ccB->type)
                {
                    case CollisionType::PLAYER_PROJECTILE:
                    {
                        //CCLOG("Enemy has collided with player projectile.");
                        auto power = entityB.component<BulletComponent>()->power;
                        eventManager->emit<EnemyHitEvent>(power, entityA);
                        entityB.destroy();
                        //entityA.destroy();
                        //return false;
                        break;
                    }
                        
                    case CollisionType::PLAYER_TURRET:
                    {
                        
                    }
                        
                    default:
                        break;
                }
            }
        }
    }
    
    //static unsigned int counter = 0;
    //CCLOG("collision detection fired: %d", ++counter);
    return false;
}

void CollisionSystem::receive(const ComponentAddedEvent<CollisionComponent> &collisionComponentAddedEvent)
{
    auto entity = collisionComponentAddedEvent.entity;
    //auto collisionComponent = collisionComponentAddedEvent.component;
    
    if (!entity.has_component<SpriteComponent>())
    {
        //ERROR HANDLING
        CCLOG("CollisionSystem: cannot add a collision component to an entity without a sprite component first.");
    }
    else
    {
        Sprite *sprite = entity.component<SpriteComponent>()->sprite;
        auto *body = MyBodyParser::getInstance()->bodyFromJson(sprite, sprite->getName());
        if (body != nullptr)
        {
            sprite->setPhysicsBody(body);
            sprite->getPhysicsBody()->setCategoryBitmask(0x01111111);
            sprite->getPhysicsBody()->setContactTestBitmask(0x041111);
            //sprite->getPhysicsBody()->setCategoryBitmask((int)collisionComponent->type);
            //sprite->getPhysicsBody()->setCollisionBitmask((int)collisionComponent->type);
        }
        else
        {
            CCLOG("Error: could not initialise physics body");
        }
    }
}

void CollisionSystem::update(EntityManager &es, EventManager &events, double dt)
{
    /*
	CollisionComponent::Handle collisionComponent;
	SpriteComponent::Handle spriteComponent;

	for (auto entity : es.entities_with_components(collisionComponent, spriteComponent))
	{
        if(collisionComponent->type == CollisionType::PLAYER)
        {
            //check this entity against everything a player can collide with
            CollisionComponent::Handle cc;
            SpriteComponent::Handle sc;
            for(auto collidible : es.entities_with_components(cc,sc))
            {
                if(cc->type == CollisionType::ENEMY_PROJECTILE      ||
                   cc->type == CollisionType::ENEMY                 ||
                   cc->type == CollisionType::UPGRADE)
                {
                    auto mainSprite = spriteComponent->sprite;
                    auto mainContentSize = mainSprite->getBoundingBox();
                    auto checkSprite = sc->sprite;
                    auto checkContentSize = checkSprite->getBoundingBox();
                    if (mainContentSize.intersectsRect(checkContentSize))
                    {
                        if (this->pixelPerfectCollisionTest(mainSprite, checkSprite))
                        {
                            //need to switch over the type of collison that has happend and act accordingly
                            switch (cc->type)
                            {
                                case CollisionType::ENEMY_PROJECTILE:
                                {
                                    CCLOG("Collision between enemy projectile and player has fired.");
                                    auto power = collidible.component<BulletComponent>()->power;
                                    events.emit<ShieldHitEvent>(power, entity);
                                    collidible.destroy();
                                    break;
                                }
                                case CollisionType::ENEMY:
                                {
                                    CCLOG("Collision between enemy and player has fired.");
                                    //events.emit<ShieldHitEvent>(200, entity);
                                    auto power = collidible.component<EnemyWeaponComponent>()->attackPower;
                                    //when a player collides with an enemy, they take damage equal to three times its attack power
                                    events.emit<ShieldHitEvent>(power * 3, entity);
                                    collidible.destroy();
                                    break;
                                }
                                    
                                    
                                case CollisionType::UPGRADE:
                                    CCLOG("Collision between upgrade and player has fired.");
                                    break;
                                    
                                default: break;
                            }
                        }

                    }
                }
            }
        
        }
        else if(collisionComponent->type == CollisionType::ENEMY)
        {
            CollisionComponent::Handle cc;
            SpriteComponent::Handle sc;
            for(auto collidible : es.entities_with_components(cc,sc))
            {
                if (cc->type == CollisionType::PLAYER_PROJECTILE     ||
                    cc->type == CollisionType::PLAYER_TURRET)
                {
                    auto mainSprite = spriteComponent->sprite;
                    auto mainContentSize = mainSprite->getBoundingBox();
                    auto checkSprite = sc->sprite;
                    auto checkContentSize = checkSprite->getBoundingBox();
                    if(mainContentSize.intersectsRect(checkContentSize))
                    {
                        if (this->pixelPerfectCollisionTest(mainSprite, checkSprite))
                        {
                            switch (cc->type)
                            {
                                case CollisionType::PLAYER_PROJECTILE:
                                    CCLOG("Enemy has collided with player projectile.");
                                    collidible.destroy();
                                    entity.destroy();
                                    //this is here for testing purposes, remove when events are added
                                    return;
                                    break;
                                    
                                case CollisionType::PLAYER_TURRET:
                                    CCLOG("Enemy has collided with a player's turret.");
                                    collidible.destroy();
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                    }
                }
            }
        }
	}
     */
}

bool CollisionSystem::pixelPerfectCollisionTest(cocos2d::Sprite *spr1, cocos2d::Sprite *spr2) const
{
    /*
    Rect r1 = spr1->getBoundingBox();
    Rect r2 = spr2->getBoundingBox();
    
    float tempX;
    float tempY;
    float tempWidth;
    float tempHeight;
    
    if (r1.getMaxX() > r2.getMinX())
    {
        tempX = r2.getMinX();
        tempWidth = r1.getMaxX() - r2.getMinX();
    }
    else
    {
        tempX = r1.getMinX();
        tempWidth = r2.getMaxX() - r1.getMinX();
    }
    
    if (r1.getMinY() < r2.getMaxY())
    {
        tempY = r1.getMinY();
        tempHeight = r2.getMaxY() - r1.getMinY();
    }
    else
    {
        tempY = r2.getMinY();
        tempHeight = r1.getMaxY() - r2.getMinY();
    }
    
    //create the intersection rect
    
    Rect intersection(tempX * CC_CONTENT_SCALE_FACTOR(), tempY * CC_CONTENT_SCALE_FACTOR(), tempWidth * CC_CONTENT_SCALE_FACTOR(), tempHeight * CC_CONTENT_SCALE_FACTOR());
    
    unsigned int x = intersection.origin.x;
    unsigned int y = intersection.origin.y;
    unsigned int w = intersection.size.width;
    unsigned int h = intersection.size.height;
    
    unsigned int numPixels = w * h;
    
    if (numPixels <= 0) return false;
    
    //draw the pixels onto a RenderTexture
    Size winSize = Director::getInstance()->getWinSize();
    RenderTexture *rt = RenderTexture::create(w, h, Texture2D::PixelFormat::RGBA8888);
    
    rt->beginWithClear(0, 0, 0, 0);
    
    //glReadBuffer(GL_BACK);
    
    //render both sprites: first one in RED and second one in GREEN
    glColorMask(1, 0, 0, 1);
    spr1->visit();
    glColorMask(0, 1, 0, 1);
    spr2->visit();
    //set color mask back to normal
    glColorMask(1, 1, 1, 1);
    
    Color4B *buffer = (Color4B *)malloc(sizeof(Color4B) * numPixels);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    
    rt->end();
    
    //Director::getInstance()->getRenderer()->render();
    
    bool isColliding = false;
    unsigned int step = 1;
    for (unsigned int i = 0; i < numPixels; i+=step)
    {
        Color4B color = buffer[i];
        GLubyte red = buffer[i].r;
        GLubyte green = buffer[i].g;
        
        if (color.r > 0 && color.g > 0)
        {
            isColliding = true;
            break;
        }
    }
    free(buffer);
    
    return isColliding;
     */
    return true;
}
