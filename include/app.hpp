#pragma once

#include <SDL3/SDL.h>

class App
{
public:
    App();
    ~App();
    void run();

private:
    void initialize();
    void processEvents();
    void quit();
};