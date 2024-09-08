#include <iostream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

void getSDLVersion();

int main(int argc, char* argv[])
{
    getSDLVersion();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Error: SDL_Init(): " << SDL_GetError() << std::endl; 
    }

    SDL_Window *window;
    window = SDL_CreateWindow("Graphics-Demo", 1920, 1080, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl; 
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    bool running = true;
    SDL_Event e;

    while (running)
    {
        while (SDL_PollEvent(&e))     
        {
            if (e.type == SDL_EVENT_QUIT || (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE))
                running = false;
        }

        glClearColor(50 / 255.0f, 50 / 255.0f, 50 / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void getSDLVersion()
{
    const int compiled = SDL_VERSION;
    const int linked = SDL_GetVersion();

    SDL_Log("========== Compiled against SDL version %d.%d.%d ==========\n",
            SDL_VERSIONNUM_MAJOR(compiled),
            SDL_VERSIONNUM_MINOR(compiled),
            SDL_VERSIONNUM_MICRO(compiled));

    SDL_Log("========== Linking  against SDL version %d.%d.%d =========\n\n",
            SDL_VERSIONNUM_MAJOR(linked),
            SDL_VERSIONNUM_MINOR(linked),
            SDL_VERSIONNUM_MICRO(linked));
}