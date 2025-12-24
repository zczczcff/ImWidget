#include "Application/ImApplication.h"

extern ImGuiWidget::ImWidget* ImInit();
extern void ImTick();

namespace ImGuiWidget
{
    ImApplication* GlobalApp;
}

void ImApplication::Render()
{
    ImGui::PushFont(GetFont(DefalutFontSize, DefaultFont));
    m_EventSys->ProcessEvents();
    ImTick();
    ImGui::PopFont();
}

void AbstractMainFun(ImApplication* app)
{
    ImGuiWidget::GlobalApp = app;
    if (!app->Initialize())
        return ;
    app->SetRootWidget(ImInit());
    app->Run();
}


//根据平台区分入口函数
#if defined(_WIN32)
#include "Application/ImWin64Application.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
){
    ImWin64Application app(hInstance, nShowCmd);
    AbstractMainFun(&app);
    return 0;
}

#elif defined(__linux__)
#include "Application/ImPiApplication.h"


int main(int argc, char** argv) {
    ImPiApplication app;
    AbstractMainFun(&app);
    return 0;
}


#endif