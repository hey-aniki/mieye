#include "MapScene.h"

USING_NS_CC;

MapScene::~MapScene(){}

bool MapScene::init()
{
    if(!Scene::init()){
        return false;
    }

    auto layer = GameLayer::create();

    this->addChild(layer);

    return true;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameLayer::menuCloseCallback, this));

	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    map_ = cocos2d::TMXTiledMap::create("newtiled.tmx");
    this->addChild(map_, 0);

    cocos2d::Texture2D *heroTexture = cocos2d::TextureCache::getInstance() -> addImage("hero.png");
    auto initFrame = cocos2d::SpriteFrame::createWithTexture(
        heroTexture,
        cocos2d::Rect(0, 0, 32, 32)
    );
    hero_ = cocos2d::Sprite::createWithSpriteFrame(initFrame);
    hero_->setAnchorPoint(Point(0, 0));
    hero_->setPosition(TileCoordToPosition(cocos2d::Point(1, 11)));
    map_->addChild(hero_);
    hero_is_walking_ = false;

    hero_move_path_ = std::deque<HeroDirection>();

    walk_animation_ = cocos2d::Vector<cocos2d::Animation*>(4);
    walk_animation_.insert(kDown, createAnimationByDirection(kDown));
    walk_animation_.insert(kLeft, createAnimationByDirection(kLeft));
    walk_animation_.insert(kRight, createAnimationByDirection(kRight));
    walk_animation_.insert(kUp, createAnimationByDirection(kUp));

    this->schedule(schedule_selector(GameLayer::update));

    auto listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(GameLayer::TouchAndMove, this);
    cocos2d::Director::getInstance()
        ->getEventDispatcher()
        ->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

GameLayer::~GameLayer()
{
    for(auto a : walk_animation_){
        a -> release();
    }
    walk_animation_.clear();
    this->unscheduleAllSelectors();
}


cocos2d::Animation *GameLayer::createAnimationByDirection(HeroDirection direction)
{
    cocos2d::Texture2D *heroTexture = cocos2d::TextureCache::sharedTextureCache() -> addImage("hero.png");
    cocos2d::Vector<cocos2d::SpriteFrame *> animFrames(4);
    for(int i = 0; i < 4; i++){
        cocos2d::SpriteFrame *frame = cocos2d::SpriteFrame::createWithTexture(
            heroTexture,
            cocos2d::Rect(32 * i, 32 * direction, 32, 32)
        );
        animFrames.pushBack(frame);
    }
    cocos2d::Animation *animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.2f);
    animation -> retain();
    return animation;
}


void GameLayer::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


float distance(cocos2d::Point a, cocos2d::Point b)
{
    float dx, dy;
    if(a.x < b.x)
        dx = b.x - a.x;
    else
        dx = a.x - b.x;
    if(a.y < b.y)
        dy = b.y - a.y;
    else
        dy = a.y - b.y;
    return dx + dy;
}


bool GameLayer::CheckNode(cocos2d::Point cur, cocos2d::Point next,
        PathNode& cur_map_node, PathNode& next_map_node,
        HeroDirection move_direction)
{
    if(next_map_node.visited)
        return false;
    if(this->map_->layerNamed("wall")->tileGIDAt(next))
        return false;
    if(!next_map_node.previous){
        next_map_node.previous = \
            ((int)cur.x << 4) + (int)cur.y + 1;
        next_map_node.pre_move = (unsigned char)move_direction;
        next_map_node.path_len = cur_map_node.path_len + 1;
    } else {
        return false;
    }
    return true;
}


void GameLayer::UpdateHeroPath(Point target)
{
    PathNode map_node[(int)map_->getMapSize().width][(int)map_->getMapSize().height];
    memset(map_node, 0,
        sizeof(PathNode) * (int)map_->getMapSize().width *
        (int)map_->getMapSize().height);

    std::deque<cocos2d::Point> visit_node;

    cocos2d::Point start_point;
    if(hero_is_walking_)
        start_point = PositionToTileCoord(hero_next_position_);
    else
        start_point = PositionToTileCoord(hero_->getPosition());
    visit_node.push_back(start_point);

    cocos2d::Point next;
    bool not_checked;

    cocos2d::Point closest_target = start_point;

    while(!visit_node.empty()){
        cocos2d::Point cur = visit_node.front();
        visit_node.pop_front();
        if(cur == target)
            break;
        if(map_node[(int)cur.x][(int)cur.y].visited == 1)
            continue;
        map_node[(int)cur.x][(int)cur.y].visited = 1;
        // 4 directions
        for(int i = 0; i < 4; i++){
            switch(i){
                case kUp:
                    if(cur.y <= 0)
                        break;
                    next = cocos2d::Point(cur.x, cur.y - 1);
                    not_checked = CheckNode(
                        cur,
                        next,
                        map_node[(int)cur.x][(int)cur.y],
                        map_node[(int)next.x][(int)next.y],
                        kUp
                    );
                    if(not_checked){
                        visit_node.push_back(next);
                        if(distance(next, target)
                            < distance(closest_target, target)){
                            closest_target = next;
                        }
                    }
                    break;
                case kDown:
                    if(cur.y >= map_->getMapSize().height - 1)
                        break;
                    next = cocos2d::Point(cur.x, cur.y + 1);
                    not_checked = CheckNode(
                        cur,
                        next,
                        map_node[(int)cur.x][(int)cur.y],
                        map_node[(int)next.x][(int)next.y],
                        kDown
                    );
                    if(not_checked){
                        visit_node.push_back(next);
                        if(distance(next, target)
                            < distance(closest_target, target)){
                            closest_target = next;
                        }
                    }
                    break;
                case kLeft:
                    if(cur.x <= 0)
                        break;
                    next = cocos2d::Point(cur.x - 1, cur.y);
                    not_checked = CheckNode(
                        cur,
                        next,
                        map_node[(int)cur.x][(int)cur.y],
                        map_node[(int)next.x][(int)next.y],
                        kLeft
                    );
                    if(not_checked){
                        visit_node.push_back(next);
                        if(distance(next, target)
                            < distance(closest_target, target)){
                            closest_target = next;
                        }
                    }
                    break;
                case kRight:
                    if(cur.x >= map_->getMapSize().width - 1)
                        break;
                    next = cocos2d::Point(cur.x + 1, cur.y);
                    not_checked = CheckNode(
                        cur,
                        next,
                        map_node[(int)cur.x][(int)cur.y],
                        map_node[(int)next.x][(int)next.y],
                        kRight
                    );
                    if(not_checked){
                        visit_node.push_back(next);
                        if(distance(next, target)
                            < distance(closest_target, target)){
                            closest_target = next;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    hero_move_path_.clear();
    HeroDirection reverse_direction[4];
    reverse_direction[(int)kUp] = kDown;
    reverse_direction[(int)kDown] = kUp;
    reverse_direction[(int)kLeft] = kRight;
    reverse_direction[(int)kRight] = kLeft;
    while(closest_target != start_point){
        hero_move_path_.push_front(/*reverse_direction[*/(HeroDirection)
            map_node[(int)closest_target.x][(int)closest_target.y].pre_move);
        unsigned char prenode =
            map_node[(int)closest_target.x][(int)closest_target.y].previous;
        closest_target.x = (float)(((int)prenode - 1) >> 4);
        closest_target.y = (float)(((int)prenode - 1) & ((1 << 4) - 1));
    }
}


bool GameLayer::TouchAndMove(Touch* touch, Event* event)
{
    Point p = this->convertTouchToNodeSpace(touch);
    cocos2d::log("on touch event get on %f, %f", p.x, p.y);
    UpdateHeroPath(cocos2d::Point(1,5));
    MoveHeroAlong();
    return true;
}


void GameLayer::MoveOneStep(HeroDirection direction)
{
    cocos2d::log("hero position: %f, %f", hero_->getPosition().x, hero_->getPosition().y);
    cocos2d::Point move_by_position;
    switch(direction){
        case kDown:
            move_by_position = cocos2d::Point(0, -32);
            break;
        case kLeft:
            move_by_position = cocos2d::Point(-32, 0);
            break;
        case kRight:
            move_by_position = cocos2d::Point(32, 0);
            break;
        case kUp:
            move_by_position = cocos2d::Point(0, 32);
            break;
    }
    if(CheckCollision(hero_->getPosition() + move_by_position) == kWall){
        SetFacingDirection((HeroDirection)direction);
        hero_move_path_.clear();
        return;
    }

    hero_next_position_ = hero_->getPosition() + move_by_position;
    hero_is_walking_ = true;

    auto action = cocos2d::Sequence::create(
        cocos2d::Spawn::create(
            cocos2d::Animate::create(walk_animation_.at(direction)),
            cocos2d::MoveBy::create(0.8f, move_by_position),
            NULL
        ),
        cocos2d::CallFuncN::create(
            CC_CALLBACK_1(
                GameLayer::OnWalkDone, this, (void *)direction
            )),
        NULL
    );
    hero_->runAction(action);
}


void GameLayer::MoveHeroAlong()
{
    if(hero_is_walking_)
        return;
    if(hero_move_path_.empty())
        return;
    HeroDirection _direction = hero_move_path_.front();
    hero_move_path_.pop_front();
    MoveOneStep(_direction);
}


void GameLayer::OnWalkDone(Node *target, void *data)
{
    int direction = int(data);
    SetFacingDirection((HeroDirection)direction);
    hero_is_walking_ = false;
    ScenceScroll(hero_->getPosition());
    MoveHeroAlong();
}


void GameLayer::SetFacingDirection(HeroDirection direction)
{
    hero_->setTextureRect(Rect(0, 32 * direction, 32, 32));
}


cocos2d::Point GameLayer::TileCoordToPosition(cocos2d::Point tilecoord)
{
    cocos2d::Point pos = cocos2d::Point(
        tilecoord.x * map_->getTileSize().width,
        (map_->getMapSize().height - 1 - tilecoord.y) * map_->getTileSize().height
    );
    return pos;
}


void GameLayer::ScenceScroll(Point reference_position)
{
    cocos2d::Size screen_size = cocos2d::Director::getInstance()->getWinSize();
    cocos2d::Size map_size_pixel = cocos2d::Size(
        map_->getMapSize().width * map_->getTileSize().width,
        map_->getMapSize().height * map_->getTileSize().height
    );
    cocos2d::Point scroll_by = Point(0.0f, 0.0f);
    if(reference_position.x > screen_size.width / 2.0f){
        scroll_by.x = screen_size.width / 2.0f - reference_position.x;
    }
    if(reference_position.y > screen_size.height / 2.0f){
        scroll_by.y = screen_size.height / 2.0f - reference_position.y;
    }
    if(map_size_pixel.width > screen_size.width)
        scroll_by.x = MAX(scroll_by.x, screen_size.width - map_size_pixel.width);
    else
        scroll_by.x = 0.0f;
    if(map_size_pixel.height > screen_size.height)
        scroll_by.y = MAX(scroll_by.y, screen_size.height - map_size_pixel.height);
    else
        scroll_by.y = 0.0f;
    this->setPosition(scroll_by);
}


void GameLayer::update(float dt)
{
    if(hero_is_walking_){
        ScenceScroll(hero_->getPosition());
    }
}


cocos2d::Point GameLayer::PositionToTileCoord(cocos2d::Point position)
{
    int x = position.x / map_->getTileSize().width;
    int y = (((map_->getMapSize().height - 1) * map_->getTileSize().height)
            - position.y) / map_->getTileSize().height;
    return cocos2d::Point(x, y);
}


CollisionType GameLayer::CheckCollision(cocos2d::Point sprite_position)
{
    cocos2d::Point tile_coord = PositionToTileCoord(sprite_position);
    if(
        sprite_position.x < 0
        || tile_coord.x > map_->getMapSize().width - 1
        || sprite_position.y < 0
        || tile_coord.y > map_->getMapSize().height - 1)
        return kWall;

    int tile_gid = map_->layerNamed("wall")->tileGIDAt(tile_coord);
    if(tile_gid){
        return kWall;
    }
    return kNone;
}
