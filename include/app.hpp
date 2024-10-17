#pragma once

class App
{
public:
    App();
    ~App();
    static void processEvents();
    void run();

private:
    static void processAppEvents();
    void initialize();
    void terminate();
};
