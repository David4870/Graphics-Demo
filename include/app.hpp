#pragma once

#include <SDL3/SDL.h>

class App
{
public:
    App();
    ~App();
    static void processEvents();
    void run();

private:
    void initialize();
    void quit();
};