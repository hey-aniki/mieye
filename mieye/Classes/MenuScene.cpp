#include "MenuScene.h"
#include "MapScene.h"

USING_NS_CC;

Scene* MenuLayer::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuLayer::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


bool MenuLayer::init()
{
    if(!Layer::init()){
        return false;
    }
    auto start_game_button = MenuItemFont::create("Start Game", MenuLayer::startGameCallback);
    auto menu = Menu::create(start_game_button, NULL);
    menu->alignItemsVerticallyWithPadding(20);
    this->addChild(menu);
}


void MenuLayer::startGameCallback(cocos2d::Ref *sender)
{
    auto scene = MapScene::create();
    TransitionScene *transition = TransitionFade::create(1, scene);
    Director::getInstance()->replaceScene(transition);
}


MenuLayer::~MenuLayer(){}
