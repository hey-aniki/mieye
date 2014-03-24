#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

typedef enum {
    kDown = 0,
    kLeft = 1,
    kRight = 2,
    kUp = 3
} HeroDirection;

typedef enum {
    kNone = 1,
    kWall,
    kEnemy
} CollisionType;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void MoveCallback(cocos2d::Ref* pSender);

    void OnWalkDone(Node *target, void *data);

    void SetFacingDirection(HeroDirection direction);

    cocos2d::Point TileCoordToPosition(cocos2d::Point tilecoord);

    cocos2d::Point PositionToTileCoord(cocos2d::Point position);

    void ScenceScroll(cocos2d::Point reference_position);

    void update(float dt);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    cocos2d::Animation *createAnimationByDirection(HeroDirection direction);

    cocos2d::Vector<cocos2d::Animation*> walk_animation_;

    cocos2d::Sprite *hero_;

    cocos2d::TMXTiledMap *map_;

    bool hero_is_walking_;

    ~HelloWorld();
};

#endif // __HELLOWORLD_SCENE_H__
