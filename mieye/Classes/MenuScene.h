#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"

class MenuLayer: public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    ~MenuLayer(void);
    CREATE_FUNC(MenuLayer);

private:
    void startGameCallback(cocos2d::Ref *sender);
};

#endif // __MENU_SCENE_H__
