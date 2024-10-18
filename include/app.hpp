#pragma once

class App
{
public:
    App();
    ~App();
    static void processEvents();
    static void startImGuiFrame();
    static void endImGuiFrame();
    void run();

private:
    static void processAppEvents();
    void setWindowSize();
    void initialize();
    void terminate();
};
