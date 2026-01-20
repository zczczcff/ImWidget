#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>
#include <unordered_map>
#include "ImComboWidget/ImPageManager.h"
#include "EditorObjectBase.h"
namespace ImGuiWidget
{
    class ImWindow;
}
class UI_ProjectView : public ImGuiWidget::ImUserWidget,public EditorObjectBase
{
private:
    ImGuiWidget::ImPageManager* m_PageManager;
    void Init();
    void RebuildView();
public:

public:
    UI_ProjectView(const std::string& name, StatePath* stateSystem, const std::string& Path);
private:

};