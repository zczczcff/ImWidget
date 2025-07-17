#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <imgui.h>
#include <vector>

class PiApplication
{
public:
    PiApplication();
    virtual ~PiApplication();

    bool Initialize();
    void Run();
    virtual void Render() = 0;

    ImTextureID LoadTextureFromFile(const char* filename, int& width, int& height);
    void ReleaseTexture(ImTextureID textureID);

protected:
    Display*    m_display = nullptr;
    Window      m_window = 0;
    int         m_screen = 0;
    EGLDisplay  m_eglDisplay = EGL_NO_DISPLAY;
    EGLContext  m_eglContext = EGL_NO_CONTEXT;
    EGLSurface  m_eglSurface = EGL_NO_SURFACE;
    
    int m_width = 1280;
    int m_height = 800;
    
    bool CreateX11Window();
    bool InitEGL();
    void HandleEvent(XEvent& event);
    void CleanupEGL();
};