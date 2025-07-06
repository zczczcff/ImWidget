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

Application::Application(HINSTANCE hInstance, int nCmdShow)
    : m_hInstance(hInstance), m_nCmdShow(nCmdShow), m_hWnd(nullptr),
    m_pd3dDevice(nullptr), m_pd3dDeviceContext(nullptr),
    m_pSwapChain(nullptr), m_mainRenderTargetView(nullptr)
{
}

Application::~Application()
{
    CleanupDeviceD3D();
}

bool Application::Initialize()
{
    // 注册窗口类
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
        m_hInstance, nullptr, nullptr, nullptr, nullptr,
        L"ImGuiAppClass", nullptr
    };
    ::RegisterClassEx(&wc);

    // 创建窗口
    m_hWnd = ::CreateWindow(
        wc.lpszClassName, L"ImGui DirectX11 Application",
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800,
        nullptr, nullptr, wc.hInstance, this);

    if (!CreateDeviceD3D())
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return false;
    }

    ::ShowWindow(m_hWnd, m_nCmdShow);
    ::UpdateWindow(m_hWnd);

    // 初始化ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_pd3dDevice, m_pd3dDeviceContext);

    return true;
}

void Application::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // 开始新帧
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 在 ImGui 渲染循环中
        ImGui::SetNextWindowPos(ImVec2(0, 0));            // 左上角开始
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize); // 覆盖整个窗口

        // 设置窗口标志（关键步骤）
        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoTitleBar |                // 无标题栏
            ImGuiWindowFlags_NoResize |                  // 不可调整大小
            ImGuiWindowFlags_NoMove |                    // 不可移动
            ImGuiWindowFlags_NoScrollbar |               // 无滚动条
            ImGuiWindowFlags_NoCollapse |                // 不可折叠
            ImGuiWindowFlags_NoBackground |              // 透明背景
            ImGuiWindowFlags_NoBringToFrontOnFocus |     // 禁止前置
            ImGuiWindowFlags_NoSavedSettings;            // 不保存设置
        // 调用渲染函数（由派生类实现）

        ImGui::Begin("MainWindow", nullptr, window_flags);
        Render();
        ImGui::End();
        // 渲染
        ImGui::Render();
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
        m_pd3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        m_pSwapChain->Present(1, 0);
    }

    // 清理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool Application::CreateDeviceD3D()
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    if (D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION,
        &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel,
        &m_pd3dDeviceContext) != S_OK)
    {
        return false;
    }

    CreateRenderTarget();
    return true;
}

void Application::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
    pBackBuffer->Release();
}

void Application::CleanupRenderTarget()
{
    if (m_mainRenderTargetView) {
        m_mainRenderTargetView->Release();
        m_mainRenderTargetView = nullptr;
    }
}

void Application::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = nullptr; }
    if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = nullptr; }
}

LRESULT WINAPI Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (Application* app = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)))
        {
            if (app->m_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
            {
                app->CleanupRenderTarget();
                app->m_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                app->CreateRenderTarget();
            }
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // 禁用ALT键菜单
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_CREATE:
    {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
    }
    return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}