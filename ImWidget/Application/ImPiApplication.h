#if defined(__linux__)
#pragma once


#include <imgui.h>
#include <vector>

#include "ImApplication.h"

class ImPiApplication:public ImApplication
{
public:
    ImPiApplication();
    virtual ~ImPiApplication();

    bool Initialize();
    void Run();

    ImTextureID LoadTextureFromFile(const char* filename, int& width, int& height);
    void ReleaseTexture(ImTextureID textureID);

protected:
    struct GLFWwindow* m_window = nullptr;
    int m_width = 1280;
    int m_height = 800;

    static void GlfwErrorCallback(int error, const char* description);
};

#endif