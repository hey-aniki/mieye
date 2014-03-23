#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
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
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    auto up = cocos2d::MenuItemFont::create(
        "up", CC_CALLBACK_1(HelloWorld::MoveCallback, this));
    up->setTag(kUp);
    auto right = cocos2d::MenuItemFont::create(
        "right", CC_CALLBACK_1(HelloWorld::MoveCallback, this));
    right->setTag(kRight);
    auto action_menu = Menu::create(up, right, NULL);
    action_menu->alignItemsHorizontallyWithPadding(50);
    this->addChild(action_menu, 1);

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

    walk_animation_ = cocos2d::Vector<cocos2d::Animation*>(4);
    walk_animation_.insert(kDown, createAnimationByDirection(kDown));
    walk_animation_.insert(kLeft, createAnimationByDirection(kLeft));
    walk_animation_.insert(kRight, createAnimationByDirection(kRight));
    walk_animation_.insert(kUp, createAnimationByDirection(kUp));

    this->schedule(schedule_selector(HelloWorld::update));

    return true;
}

HelloWorld::~HelloWorld()
{
    for(auto a : walk_animation_){
        a -> release();
    }
    walk_animation_.clear();
    this->unscheduleAllSelectors();
}


cocos2d::Animation *HelloWorld::createAnimationByDirection(HeroDirection direction)
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


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void HelloWorld::MoveCallback(Ref* pSender)
{
    cocos2d::log("hero position: %f, %f", hero_->getPosition().x, hero_->getPosition().y);
    if(hero_is_walking_)
        return;
    hero_is_walking_ = true;
    int target_direction = ((cocos2d::Node *) pSender) -> getTag();
    cocos2d::Point move_by_position;
    switch(target_direction){
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
    auto action = cocos2d::Sequence::create(
        cocos2d::Spawn::create(
            cocos2d::Animate::create(walk_animation_.at(target_direction)),
            cocos2d::MoveBy::create(0.8f, move_by_position),
            NULL
        ),
        cocos2d::CallFuncN::create(
            CC_CALLBACK_1(
                HelloWorld::OnWalkDone, this, (void *)target_direction
            )),
        NULL
    );
    hero_->runAction(action);
}


void HelloWorld::OnWalkDone(Node *target, void *data)
{
    int direction = int(data);
    SetFacingDirection((HeroDirection)direction);
    hero_is_walking_ = false;
    ScenceScroll(hero_->getPosition());
}


void HelloWorld::SetFacingDirection(HeroDirection direction)
{
    hero_->setTextureRect(Rect(0, 32 * direction, 32, 32));
}


cocos2d::Point HelloWorld::TileCoordToPosition(cocos2d::Point tilecoord)
{
    cocos2d::Point pos = cocos2d::Point(
        tilecoord.x * map_->getTileSize().width,
        (map_->getMapSize().height - 1 - tilecoord.y) * map_->getTileSize().height
    );
    return pos;
}


void HelloWorld::ScenceScroll(Point reference_position)
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
    cocos2d::log("%f, %f", scroll_by.x, scroll_by.y);
}


void HelloWorld::update(float dt)
{
    if(hero_is_walking_){
        ScenceScroll(hero_->getPosition());
    }
}


cocos2d::Point HelloWorld::PositionToTileCoord(cocos2d::Point position)
{
    int x = position.x / map_->getTileSize().width;
    int y = (((map_->getMapSize().height - 1) * map_->getTileSize().height)
            - position.y) / map_->getTileSize().height;
    return cocos2d::Point(x, y);
}
