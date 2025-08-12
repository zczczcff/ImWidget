#include "Application/ImApplication.h"

extern int ImInit();
extern void ImTick();

namespace ImGuiWidget
{
    ImApplication* GlobalApp;
}

void ImApplication::Render()
{
    ImGui::PushFont(GetFont(DefalutFontSize, DefaultFont));
    ImTick();
    ImGui::PopFont();
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
    ImGuiWidget::GlobalApp = &app;
    if (!app.Initialize())
        return 1;
    ImInit();
    app.Run();
    return 0;
}

#elif defined(__linux__)
#include "Application/ImPiApplication.h"


int main(int argc, char** argv) {
    ImPiApplication app;
    ImGuiWidget::GlobalApp = &app;

    if (!app.Initialize()) return 1;
    ImInit();
    app.Run();
    return 0;
}


#endif