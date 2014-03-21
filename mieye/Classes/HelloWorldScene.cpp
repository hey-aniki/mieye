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

    auto map = cocos2d::TMXTiledMap::create("newtiled.tmx");
    this->addChild(map, 0);

    cocos2d::Texture2D *heroTexture = cocos2d::TextureCache::sharedTextureCache() -> addImage("hero.png");
    auto initFrame = cocos2d::SpriteFrame::createWithTexture(
        heroTexture,
        cocos2d::Rect(0, 0, 32, 32)
    );
    auto hero = cocos2d::Sprite::createWithSpriteFrame(initFrame);
    hero->setAnchorPoint(Point(0, 0));
    hero->setPosition(cocos2d::Point(32, 32));
    map->addChild(hero);

    mWalkAnimation = cocos2d::Vector<cocos2d::Animation*>(4);
    mWalkAnimation.insert(kDown, createAnimationByDirection(kDown));
    mWalkAnimation.insert(kLeft, createAnimationByDirection(kLeft));
    mWalkAnimation.insert(kRight, createAnimationByDirection(kRight));
    mWalkAnimation.insert(kUp, createAnimationByDirection(kUp));
    cocos2d::Animate *animate = cocos2d::Animate::create(mWalkAnimation.at(kDown));
    hero->runAction(cocos2d::RepeatForever::create(animate));

    map->setPosition(Point(
        (visibleSize.width - map->getMapSize().width * map->getTileSize().width) / 2,
        (visibleSize.height - map->getMapSize().height * map->getTileSize().height) / 2));

    return true;
}

HelloWorld::~HelloWorld()
{
    for(auto a : mWalkAnimation){
        a -> release();
    }
    mWalkAnimation.clear();
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
