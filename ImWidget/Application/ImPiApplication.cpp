#if defined(__linux__)

#include "ImPiApplication.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "imgui_impl_x11.h"
#include "imgui_impl_opengl3.h"

ImPiApplication::ImPiApplication() {}

ImPiApplication::~ImPiApplication() {
    CleanupEGL();
    if (m_display) {
        XCloseDisplay(m_display);
    }
}

bool ImPiApplication::Initialize() {
    if (!CreateX11Window()) return false;
    if (!InitEGL()) return false;
    
    // 设置窗口标题
    XStoreName(m_display, m_window, "ImGui Raspberry Pi App");
    
    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    // 初始化后端
    ImGui_ImplX11_Init(m_display, m_window);
    ImGui_ImplOpenGL3_Init("#version 100"); // GLES2对应GLSL 100
    
    return true;
}

bool ImPiApplication::CreateX11Window() {
    m_display = XOpenDisplay(nullptr);
    if (!m_display) return false;
    
    m_screen = DefaultScreen(m_display);
    Window root = RootWindow(m_display, m_screen);
    
    // 设置窗口属性
    XSetWindowAttributes windowAttribs;
    windowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                               ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    
    m_window = XCreateWindow(
        m_display, root,
        0, 0, m_width, m_height, 0,
        CopyFromParent, InputOutput,
        CopyFromParent, CWEventMask,
        &windowAttribs
    );
    
    // 允许窗口关闭
    Atom wmDelete = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(m_display, m_window, &wmDelete, 1);
    
    XMapWindow(m_display, m_window);
    XFlush(m_display);
    return true;
}

bool ImPiApplication::InitEGL() {
    m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)m_display);
    if (m_eglDisplay == EGL_NO_DISPLAY) return false;
    
    if (!eglInitialize(m_eglDisplay, nullptr, nullptr)) return false;
    
    // 配置EGL
    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(m_eglDisplay, configAttribs, &config, 1, &numConfigs) || numConfigs == 0) {
        return false;
    }
    
    // 创建EGL表面
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, config, m_window, nullptr);
    if (m_eglSurface == EGL_NO_SURFACE) return false;
    
    // 创建OpenGL ES 2.0上下文
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    m_eglContext = eglCreateContext(m_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (m_eglContext == EGL_NO_CONTEXT) return false;
    
    if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext)) {
        return false;
    }
    
    return true;
}

void ImPiApplication::Run() {
    XEvent event;
    bool running = true;
    
    while (running) {
        while (XPending(m_display) {
            XNextEvent(m_display, &event);
            HandleEvent(event);
            
            // 检查窗口关闭事件
            if (event.type == ClientMessage) {
                Atom wmDelete = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
                if (event.xclient.data.l[0] == wmDelete) {
                    running = false;
                }
            }
        }
        
        // 开始ImGui帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplX11_NewFrame();
        ImGui::NewFrame();
        
        // 设置全屏窗口
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGuiWindowFlags flags = 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoSavedSettings;
        
        ImGui::Begin("MainWindow", nullptr, flags);
        Render(); // 调用派生类的渲染
        ImGui::End();
        
        // 渲染
        ImGui::Render();
        glViewport(0, 0, m_width, m_height);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        eglSwapBuffers(m_eglDisplay, m_eglSurface);
    }
    
    // 清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplX11_Shutdown();
    ImGui::DestroyContext();
}

void ImPiApplication::HandleEvent(XEvent& event) {
    // 传递事件给ImGui处理
    ImGui_ImplX11_EventHandler(&event);
}

void ImPiApplication::CleanupEGL() {
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
        }
        if (m_eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
        }
        eglTerminate(m_eglDisplay);
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglContext = EGL_NO_CONTEXT;
    m_eglSurface = EGL_NO_SURFACE;
}

ImTextureID ImPiApplication::LoadTextureFromFile(const char* filename, int& width, int& height) {
    // 加载图像
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) return nullptr;

    // 创建OpenGL纹理
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    return (ImTextureID)(intptr_t)texture;
}

void ImPiApplication::ReleaseTexture(ImTextureID textureID) {
    if (textureID) {
        GLuint texture = (GLuint)(intptr_t)textureID;
        glDeleteTextures(1, &texture);
    }
}

#endif