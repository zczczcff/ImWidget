// Model_ImUserWidgetClassEditor.h
#pragma once

#include "EditorEventObject.h"
#include "ImWidget/ImUserWidgetClass.h"
#include "Model/Command/ImDesignerCommandManager.h"
#include "EditorAction.h"
#include "EditorEvents.h"
#include <memory>
#include <string>


class Model_ImUserWidgetClassEditor : public EditorEventObject
{
private:
    // 编辑的文件完整路径
    std::string m_EditedFileFullPath;

    // 目标类
    ImGuiWidget::ImUserWidgetClass* m_TargetClass;

    // 命令管理器
    std::unique_ptr<ImDesingnerCommandManager> m_CommandManager;

    // 当前选择的变量信息
    std::string m_CurrentSelectedVariableName;
    ImGuiWidget::WidgetClassVariableType m_CurrentSelectedVariableType;
    ImGuiWidget::ImWidget* m_CurrentSelectedWidget;

    // 编辑状态
    bool m_IsModified;

    // 添加：Action和Event容器
    std::vector<EditorActionID> m_FileActions;
    std::vector<EditorEventID> m_FileEvents;
public:
    Model_ImUserWidgetClassEditor(const std::string& editedFileFullPath, ImGuiWidget::ImUserWidgetClass* Target)
        : m_EditedFileFullPath(editedFileFullPath)
        , m_TargetClass(Target)
        , m_IsModified(false)
        , m_CurrentSelectedWidget(nullptr)
        , m_CurrentSelectedVariableType(ImGuiWidget::WidgetClassVariableType::Widget)
    {
        m_CommandManager = std::make_unique<ImDesingnerCommandManager>();
        InitActions();
    }

    virtual ~Model_ImUserWidgetClassEditor()
    {
        // 父类析构函数会自动清理订阅
        if (m_TargetClass)
        {
            delete m_TargetClass;
        }
    }

    // 获取目标类
    ImGuiWidget::ImUserWidgetClass* GetTargetClass() const { return m_TargetClass; }

    // 获取文件路径
    const std::string& GetEditedFileFullPath() const { return m_EditedFileFullPath; }

    // 检查是否已修改
    bool IsModified() const { return m_IsModified; }

    // 保存修改
    bool SaveChanges()
    {
        if (!m_TargetClass) return false;

        // 保存到文件
        bool success = m_TargetClass->ExportToJsonFile(m_EditedFileFullPath);
        if (success)
        {
            m_IsModified = false;
            // 发布保存成功事件
            Publish(m_EditedFileFullPath + Events::MainUI::SET_UNDOREDO_STATE, false, false);
        }
        return success;
    }

    // 撤销
    bool Undo()
    {
        if (!m_CommandManager->CanUndo()) return false;

        bool success = m_CommandManager->Undo();
        if (success)
        {
            m_IsModified = true;
            UpdateUndoRedoState();
        }
        return success;
    }

    // 重做
    bool Redo()
    {
        if (!m_CommandManager->CanRedo()) return false;

        bool success = m_CommandManager->Redo();
        if (success)
        {
            m_IsModified = true;
            UpdateUndoRedoState();
        }
        return success;
    }

    // 设置当前选择
    void SetCurrentSelection(const std::string& variableName,
        ImGuiWidget::WidgetClassVariableType variableType,
        ImGuiWidget::ImWidget* widget = nullptr)
    {
        m_CurrentSelectedVariableName = variableName;
        m_CurrentSelectedVariableType = variableType;
        m_CurrentSelectedWidget = widget;
    }

    // 获取当前选择的变量名
    const std::string& GetCurrentSelectedVariableName() const { return m_CurrentSelectedVariableName; }

    // 获取当前选择的变量类型
    ImGuiWidget::WidgetClassVariableType GetCurrentSelectedVariableType() const { return m_CurrentSelectedVariableType; }

    // 获取当前选择的控件
    ImGuiWidget::ImWidget* GetCurrentSelectedWidget() const { return m_CurrentSelectedWidget; }

private:
    void InitActions()
    {
        ResetAction();
        // 监听文件重命名事件
        AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
            {
                if (m_EditedFileFullPath == OldFullPath)
                {
                    m_EditedFileFullPath = NewFullPath;
                    ResetAction();
                }
            });
    }
    // 设置动作订阅
    void ResetAction()
    {
        for (auto& id : m_FileActions)
        {
            RemoveProcessor(id);
        }

        m_FileActions.clear();
        // 订阅OutlineView的动作
        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::CREATE_BASIC_VARIABLE,
            [this](ImGuiWidget::PropertyType type)
            {
                OnCreateBasicVariable(type);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::CREATE_OBJECT_VARIABLE,
            [this](const std::string& objectRegisterName)
            {
                OnCreateObjectVariable(objectRegisterName);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::CREATE_WIDGET_VARIABLE,
            [this](const std::string& widgetRegisterName)
            {
                OnCreateWidgetVariable(widgetRegisterName);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::DELETE_VARIABLE,
            [this](const std::string& variableName)
            {
                OnDeleteVariable(variableName);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::INSERT_WIDGET,
            [this](const std::string& widgetTreeVarName, const std::string& parentPath,
                const std::string& insertWidgetRegisterName, int insertIndex)
            {
                OnInsertWidget(widgetTreeVarName, parentPath, insertWidgetRegisterName, insertIndex);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::DELETE_WIDGET,
            [this](const std::string& widgetTreeVarName, const std::string& widgetPath)
            {
                OnDeleteWidget(widgetTreeVarName, widgetPath);
            }));

        m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::OutlineView::COPY_VARIABLE,
            [this](const std::string& variableName)
            {
                OnCopyVariable(variableName);
            }));

        // 订阅全局撤销/重做请求
        m_FileActions.push_back(AddValidator(m_EditedFileFullPath + Action::_REQUEST_UNDO,
            [this]()
            {
                return Undo();
            }));

        m_FileActions.push_back(AddValidator(m_EditedFileFullPath + Action::_REQUEST_REDO,
            [this]()
            {
                return Redo();
            }));


    }


    void OnCreateBasicVariable(ImGuiWidget::PropertyType type);

    void OnCreateObjectVariable(const std::string& objectRegisterName);

    void OnCreateWidgetVariable(const std::string& widgetRegisterName);

    void OnDeleteVariable(const std::string& variableName);

    void OnCopyVariable(const std::string& variableName);

    // 更新Undo/Redo状态
    void UpdateUndoRedoState()
    {
        bool canUndo = m_CommandManager->CanUndo();
        bool canRedo = m_CommandManager->CanRedo();
        Publish(m_EditedFileFullPath + Events::MainUI::SET_UNDOREDO_STATE, canUndo, canRedo);
    }

    // 重命名变量（从事件处理）
    void OnVariableRenamed(const std::string& oldName, const std::string& newName);

    // 重命名控件（通过路径）
    void OnRenameWidgetByPath(const std::string& widgetTreeVarName,
        const std::string& widgetPath,
        const std::string& newName);

    void OnInsertWidget(const std::string& widgetTreeVarName, const std::string& parentPath,
        const std::string& insertWidgetRegisterName, int insertIndex);

    void OnDeleteWidget(const std::string& widgetTreeVarName, const std::string& widgetPath);

    // ==================== 变量粘贴操作 ====================
    void OnPasteVariable(const nlohmann::json& serializedData, bool keepOriginalName = false);
    void OnPasteObjectVariable(const nlohmann::json& objectJson, const std::string& suggestedName = "", bool keepSuggestedName = false);
    void OnPasteWidgetVariable(const nlohmann::json& widgetJson, const std::string& suggestedName = "", bool keepSuggestedName = false);

    // ==================== 通过JSON插入子控件 ====================
    void OnInsertWidgetByJson(const std::string& widgetTreeVarName, const std::string& parentPath,
        const nlohmann::json& widgetJson, int insertIndex);

    // ==================== 类字符串属性编辑 ====================
    void OnEditClassName(const std::string& newName);
    void OnEditNamespace(const std::string& newName);
    void OnEditBaseClass(const std::string& newName);
    void OnEditDefaultRoot(const std::string& newName);

    // ==================== 模板属性编辑函数 ====================


private:
    // 辅助函数：从路径中提取控件名
    std::string ExtractWidgetNameFromPath(const std::string& path)
    {
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos)
        {
            return path.substr(lastSlash + 1);
        }
        return path;
    }
};