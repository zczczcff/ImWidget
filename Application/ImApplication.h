#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Application
{
public:
    Application(HINSTANCE hInstance, int nCmdShow);
    virtual ~Application();

    bool Initialize();
    void Run();
    virtual void Render() = 0; // 纯虚函数，派生类实现具体渲染
        // 新增纹理加载函数
    ImTextureID LoadTextureFromFile(const char* filename,int& width,int& height);
    void ReleaseTexture(ImTextureID TextureID); // 新增释放函数
protected:
    HINSTANCE       m_hInstance;
    HWND            m_hWnd;
    int             m_nCmdShow;
    ID3D11Device* m_pd3dDevice;
    ID3D11DeviceContext* m_pd3dDeviceContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_mainRenderTargetView;

    bool CreateDeviceD3D();
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

