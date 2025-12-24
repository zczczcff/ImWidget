#if defined(__linux__)
#include <GLFW/glfw3.h>
#include "ImPiApplication.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ImPiApplication::ImPiApplication() {}

ImPiApplication::~ImPiApplication() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool ImPiApplication::Initialize() {
        // 初始化GLFW
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        return false;
    }
    
    // 设置OpenGL ES 2.0上下文
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    
    // 创建窗口
    m_window = glfwCreateWindow(m_width, m_height, "ImGui Raspberry Pi App", NULL, NULL);
    if (m_window == NULL) {
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // 开启垂直同步
    
    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    // 初始化GLFW和OpenGL3后端
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 100"); // GLES2对应GLSL 100
    
    printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return true;
}


void ImPiApplication::Run() {
   while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        
        // 开始新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
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
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_window);
    }
    
    // 清理
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImPiApplication::GlfwErrorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


ImTextureID ImPiApplication::LoadTextureFromFile(const char* filename, int& width, int& height) {
    // 加载图像
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) return (ImTextureID)nullptr;

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