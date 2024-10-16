#pragma once

class App
{
public:
    App();
    ~App();
    static void processEvents();
    void run();

private:
    void initialize();
    void terminate();
};
