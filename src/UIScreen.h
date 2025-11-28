#ifndef UISCREEN_H
#define UISCREEN_H

class UIScreen {
public:
    virtual ~UIScreen() {}

    virtual void enter() = 0;      // called once when screen becomes active
    virtual void exit() = 0;       // called when screen is left
    virtual void drawFull() = 0;   // full redraw
    virtual void update() = 0;     // called every loop
};

#endif
