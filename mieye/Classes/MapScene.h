#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

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

class PathFindingHelperNode
{
public:
    unsigned char path_len;
    unsigned char previous;
    unsigned char visited;
    unsigned char pre_move;
};

typedef PathFindingHelperNode PathNode;

class MapScene: public cocos2d::Scene {
public:
    virtual bool init();
    ~MapScene(void);
    CREATE_FUNC(MapScene);
};

class GameLayer : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void MoveCallback(cocos2d::Ref* pSender);

    CollisionType CheckCollision(cocos2d::Point sprite_position);

    bool TouchAndMove(cocos2d::Touch* touch, cocos2d::Event* event);

    void MoveOneStep(HeroDirection direction);

    void MoveHeroAlong();

    void OnWalkDone(Node *target, void *data);

    void SetFacingDirection(HeroDirection direction);

    cocos2d::Point TileCoordToPosition(cocos2d::Point tilecoord);

    cocos2d::Point PositionToTileCoord(cocos2d::Point position);

    void ScenceScroll(cocos2d::Point reference_position);

    void update(float dt);

    void UpdateHeroPath(cocos2d::Point target);

    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);

    cocos2d::Animation *createAnimationByDirection(HeroDirection direction);

    cocos2d::Vector<cocos2d::Animation*> walk_animation_;

    cocos2d::Sprite *hero_;

    cocos2d::TMXTiledMap *map_;

    std::deque<HeroDirection> hero_move_path_;

    cocos2d::Point hero_next_position_;

    bool hero_is_walking_;

    ~GameLayer();

private:
    bool CheckNode(cocos2d::Point cur, cocos2d::Point next,
         PathNode& cur_map_node, PathNode& next_map_node,
         HeroDirection move_direction);
};

#endif // __MAP_SCENE_H__
