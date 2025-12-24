
#if defined(_WIN32)
#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <functional>

#include "ImApplication.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImWin64Application :public ImApplication
{
public:
    ImWin64Application(HINSTANCE hInstance, int nCmdShow);
    virtual ~ImWin64Application();

    bool Initialize();
    void Run();
    ; 
        // 新增纹理加载函数
    ImTextureID LoadTextureFromFile(const char* filename,int& width,int& height)override;
    void ReleaseTexture(ImTextureID TextureID)override; // 新增释放函数
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

#endif