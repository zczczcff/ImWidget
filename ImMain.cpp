#include "Application/ImApplication.h"

extern int ImInit();
extern void ImTick();

namespace ImGuiWidget
{
    ImApplication* GlobalApp;
}

#if defined(_WIN32)
#include "Application/ImWin64Application.h"

void ImWin64Application::Render()
{
    ImTick();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    ImWin64Application app(hInstance, nCmdShow);
    ImGuiWidget::GlobalApp = &app;
    if (!app.Initialize())
        return 1;
    ImInit();
    app.Run();
    return 0;
}

#elif defined(__linux__)
#include "Application/ImPiApplication.h"

void ImPiApplication::Render()
{
    ImTick();
}

int main(int argc, char** argv) {
    ImPiApplication app;
    ImGuiWidget::GlobalApp = &app;

    if (!app.Initialize()) return 1;
    ImInit();
    app.Run();
    return 0;
}


#endif