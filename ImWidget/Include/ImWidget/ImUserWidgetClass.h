// ImUserWidgetClass.h（移除ImBasicVariable后）
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <variant>
#include "ImWidget.h"
#include "ImObject.h"
#include "ImObjectFactory.h"
#include "ImWidgetFactory.h"
#include "ImUserWidget.h"
#include "ImPanelWidget.h"
#include "ImUserWidgetClassVariable.h"  
#include "nlohmann/json.hpp"

namespace ImGuiWidget
{
    // 注意：这里已移除ImBasicVariable类定义

    // 用户控件类管理器
    class ImUserWidgetClass
    {
    private:
        std::string m_ClassName;
        std::string m_Namespace = "ImGuiWidget";
        std::string m_BaseClass = "ImUserWidget";

        // 统一变量存储
        std::unordered_map<std::string, std::unique_ptr<ImWidgetClassVariable>> m_Variables;

        std::string m_DefaultRootVariableName;

        // 名称检查辅助函数
        bool IsNameUsed(const std::string& name) const
        {
            return m_Variables.find(name) != m_Variables.end() ||
                CheckAllChildWidgetNames(name);
        }

        // 递归检查所有控件树子项名称
        bool CheckAllChildWidgetNames(const std::string& name) const
        {
            for (const auto& pair : m_Variables)
            {
                if (pair.second->GetType() == WidgetClassVariableType::Widget)
                {
                    auto widgetVar = pair.second->As<ImWidgetClassVariable_Widget>();
                    if (widgetVar && widgetVar->GetWidget())
                    {
                        if (CheckWidgetTreeForName(widgetVar->GetWidget(), name))
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        // 递归检查单个控件树中的名称
        bool CheckWidgetTreeForName(ImWidget* widget, const std::string& name) const
        {
            if (!widget) return false;

            // 检查当前控件
            if (widget->GetWidgetName() == name)
                return true;

            // 如果是容器控件，递归检查子项
            if (widget->GetChildNum() > 0)
            {
                for (int i = 0; i < widget->GetChildNum(); i++)
                {
                    ImWidget* child = widget->GetChildAt(i);
                    if (CheckWidgetTreeForName(child, name))
                        return true;
                }
            }

            return false;
        }

        // 生成唯一名称
        std::string GenerateUniqueName(const std::string& baseName) const
        {
            if (!IsNameUsed(baseName)) return baseName;
            std::string name = baseName;
            int counter = 1;

            while (IsNameUsed(name))
            {
                name = baseName + std::to_string(counter);
                counter++;
            }

            return name;
        }

        // 辅助方法：根据路径查找控件
        ImWidget* FindWidgetByPath(ImWidget* root, const std::string& path) const
        {
            if (!root) return nullptr;
            return root->FindChildByPath(path);
        }

        // 获取指定类型的变量（模板辅助函数）
        template<typename T>
        T* GetVariableAs(const std::string& name) const
        {
            auto it = m_Variables.find(name);
            if (it != m_Variables.end())
            {
                return it->second->As<T>();
            }
            return nullptr;
        }

    public:
        ImUserWidgetClass(const std::string& className)
            : m_ClassName(className)
        {
        }

        bool InitFormJson(const nlohmann::json& FromJson);
        bool InitFromFile(const std::string& FilePath);

        ~ImUserWidgetClass()
        {
            // 智能指针自动清理
            m_Variables.clear();
        }

        // 设置/获取命名空间
        void SetNamespace(const std::string& ns) { m_Namespace = ns; }
        std::string GetNamespace() const { return m_Namespace; }

        // 设置/获取基类
        void SetBaseClass(const std::string& baseClass) { m_BaseClass = baseClass; }
        std::string GetBaseClass() const { return m_BaseClass; }

        // 获取类名
        std::string GetClassName() const { return m_ClassName; }
        void SetClassName(const std::string& name) { m_ClassName = name; }

        // 设置/获取默认根控件
        void SetDefaultRootVariable(const std::string& varName)
        {
            if (m_Variables.find(varName) != m_Variables.end())
            {
                // 验证是否是控件类型
                auto widgetVar = GetVariableAs<ImWidgetClassVariable_Widget>(varName);
                if (widgetVar)
                {
                    m_DefaultRootVariableName = varName;
                }
            }
        }

        std::string GetDefaultRootVariableName() const { return m_DefaultRootVariableName; }

        ImWidget* GetDefaultRootWidget() const
        {
            if (m_DefaultRootVariableName.empty())
                return nullptr;

            auto widgetVar = GetVariableAs<ImWidgetClassVariable_Widget>(m_DefaultRootVariableName);
            return widgetVar ? widgetVar->GetWidget() : nullptr;
        }

        // ================ 新的统一变量操作接口 ================

        // 添加变量（统一接口）
        bool AddVariable(WidgetClassVariableType type, const std::string& specificType,
            std::string& outVarName)
        {
            switch (type)
            {
            case WidgetClassVariableType::Widget:
                return AddWidgetVariable(specificType, outVarName);

            case WidgetClassVariableType::Object:
                return AddObjectVariable(specificType, outVarName);

            case WidgetClassVariableType::Basic:
            {
                PropertyType propType = StringToPropertyType(specificType);
                return AddBasicVariable(propType, outVarName);
            }

            default:
                return false;
            }
        }

        // 添加控件树变量
        bool AddWidgetVariable(const std::string& typeName, std::string& outVarName)
        {
            ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(typeName, "");
            if (!widget) return false;

            std::string baseName = typeName;
            size_t pos = baseName.find_last_of("::");
            if (pos != std::string::npos)
                baseName = baseName.substr(pos + 1);

            outVarName = GenerateUniqueName(baseName);
            widget->SetWidgetName(outVarName);

            auto var = std::make_unique<ImWidgetClassVariable_Widget>(outVarName, widget);
            m_Variables[outVarName] = std::move(var);
            return true;
        }

        // 添加ImObject变量
        bool AddObjectVariable(const std::string& typeName, std::string& outVarName)
        {
            ImObject* obj = ImObjectFactory::GetInstance().CreateObject(typeName);
            if (!obj) return false;

            std::string baseName = typeName;
            outVarName = GenerateUniqueName(baseName);

            auto var = std::make_unique<ImWidgetClassVariable_Object>(outVarName, obj);
            m_Variables[outVarName] = std::move(var);
            return true;
        }

        // 添加基本变量（使用新的ImWidgetClassVariable_Basic）
        bool AddBasicVariable(PropertyType type, std::string& outVarName)
        {
            std::string baseName;
            switch (type)
            {
            case PropertyType::Int: baseName = "Int"; break;
            case PropertyType::Float: baseName = "Float"; break;
            case PropertyType::Bool: baseName = "Bool"; break;
            case PropertyType::String: baseName = "String"; break;
            case PropertyType::Color: baseName = "Color"; break;
            case PropertyType::Vec2: baseName = "Vec2"; break;
            default:
                baseName = "Unknown";
                break;
            }

            outVarName = GenerateUniqueName(baseName);
            auto var = std::make_unique<ImWidgetClassVariable_Basic>(outVarName, type);
            m_Variables[outVarName] = std::move(var);
            return true;
        }

        // 删除变量
        bool RemoveVariable(const std::string& varName)
        {
            auto it = m_Variables.find(varName);
            if (it != m_Variables.end())
            {
                // 如果删除的是默认根控件，清空默认根
                if (varName == m_DefaultRootVariableName)
                {
                    m_DefaultRootVariableName.clear();
                }

                m_Variables.erase(it);
                return true;
            }
            return false;
        }

        // 获取变量（通用接口）
        ImWidgetClassVariable* GetVariable(const std::string& varName) const
        {
            auto it = m_Variables.find(varName);
            return it != m_Variables.end() ? it->second.get() : nullptr;
        }

        // 获取具体类型的变量（类型安全）
        ImWidget* GetWidgetVariable(const std::string& varName) const
        {
            auto widgetVar = GetVariableAs<ImWidgetClassVariable_Widget>(varName);
            return widgetVar ? widgetVar->GetWidget() : nullptr;
        }

        ImObject* GetObjectVariable(const std::string& varName) const
        {
            auto objectVar = GetVariableAs<ImWidgetClassVariable_Object>(varName);
            return objectVar ? objectVar->GetObject() : nullptr;
        }

        // 获取基本变量（返回ImWidgetClassVariable_Basic类型）
        ImWidgetClassVariable_Basic* GetBasicVariable(const std::string& varName) const
        {
            return GetVariableAs<ImWidgetClassVariable_Basic>(varName);
        }

        // 获取变量类型
        WidgetClassVariableType GetVariableType(const std::string& varName) const
        {
            auto var = GetVariable(varName);
            return var ? var->GetType() : WidgetClassVariableType::Widget;
        }

        // 获取所有变量名称
        std::vector<std::string> GetAllVariableNames() const
        {
            std::vector<std::string> names;
            for (const auto& pair : m_Variables)
            {
                names.push_back(pair.first);
            }
            return names;
        }

        // 按类型获取变量名称
        std::vector<std::string> GetVariableNamesByType(WidgetClassVariableType type) const
        {
            std::vector<std::string> names;
            for (const auto& pair : m_Variables)
            {
                if (pair.second->GetType() == type)
                {
                    names.push_back(pair.first);
                }
            }
            return names;
        }

        // 获取所有控件树变量名称（兼容旧接口）
        std::vector<std::string> GetWidgetVariableNames() const
        {
            return GetVariableNamesByType(WidgetClassVariableType::Widget);
        }

        // 获取所有对象变量名称（兼容旧接口）
        std::vector<std::string> GetObjectVariableNames() const
        {
            return GetVariableNamesByType(WidgetClassVariableType::Object);
        }

        // 获取所有基本变量名称（兼容旧接口）
        std::vector<std::string> GetBasicVariableNames() const
        {
            return GetVariableNamesByType(WidgetClassVariableType::Basic);
        }

        // 重命名变量（统一接口）
        bool RenameVariable(const std::string& oldName, const std::string& newName)
        {
            // 检查新名称是否已使用
            if (IsNameUsed(newName) && newName != oldName)
                return false;

            auto it = m_Variables.find(oldName);
            if (it == m_Variables.end())
                return false;

            // 对于控件树变量，需要同时更新控件名称
            if (it->second->GetType() == WidgetClassVariableType::Widget)
            {
                auto widgetVar = it->second->As<ImWidgetClassVariable_Widget>();
                if (widgetVar && widgetVar->GetWidget())
                {
                    widgetVar->GetWidget()->SetWidgetName(newName);
                }
            }

            // 对于基本变量，也需要更新内部名称
            if (it->second->GetType() == WidgetClassVariableType::Basic)
            {
                it->second->SetName(newName);
            }

            // 更新存储位置
            auto node = m_Variables.extract(oldName);
            node.key() = newName;
            m_Variables.insert(std::move(node));

            // 如果重命名的是默认根控件，更新默认根名称
            if (oldName == m_DefaultRootVariableName)
            {
                m_DefaultRootVariableName = newName;
            }

            return true;
        }

        // 原有方法（保持不变，但使用新的变量获取方式）
        // 7.1 在指定控件树中指定父节点中插入新建子项
        ImWidget* InsertChildWidget(const std::string& WidgetTreeVarName, ImWidget* parent,
            const std::string& InsertWidgetRegisterName, int index)
        {
            if (!parent) return nullptr;

            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return nullptr;
            if (!parent->IsInTree(WidgetTreeRoot)) return nullptr;

            // 创建子控件
            ImWidget* child = ImWidgetFactory::GetInstance().CreateWidget(InsertWidgetRegisterName, "");
            if (!child) return nullptr;

            // 生成唯一名称
            std::string baseName = InsertWidgetRegisterName;
            std::string NewWidgetName = GenerateUniqueName(baseName);
            child->SetWidgetName(NewWidgetName);

            if (parent->InsertChildAt(index, child))
            {
                return child;
            }
            else
            {
                delete child;
                return nullptr;
            }
        }

        // 7.2 在指定路径控件节点插入新建子项
        ImWidget* InsertChildWidgetByPath(const std::string& WidgetTreeVarName,
            const std::string& parentWidgetPath,
            const std::string& InsertWidgetRegisterName,
            int index)
        {
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return nullptr;

            // 找到父控件
            ImWidget* parent = FindWidgetByPath(WidgetTreeRoot, parentWidgetPath);
            if (!parent) return nullptr;

            // 创建新控件
            ImWidget* child = ImWidgetFactory::GetInstance().CreateWidget(InsertWidgetRegisterName, "");
            if (!child) return nullptr;

            // 生成唯一名称
            std::string baseName = InsertWidgetRegisterName;
            std::string NewWidgetName = GenerateUniqueName(baseName);
            child->SetWidgetName(NewWidgetName);

            if (parent->InsertChildAt(index, child))
            {
                return child;
            }
            else
            {
                delete child;
                return nullptr;
            }
        }

        // 7.3 在指定控件树中指定父节点插入给定子项
        bool InsertChildWidget(const std::string& WidgetTreeVarName, ImWidget* parent,
            ImWidget* child, int index)
        {
            if (!parent || !child) return false;
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return false;
            if (!parent->IsInTree(WidgetTreeRoot)) return false;

            if (!parent->InsertChildAt(index, child))
            {
                return false;
            }

            // 更新所有子控件的名称
            ImWidget* current = child;
            std::queue<ImWidget*> children;
            children.push(current);
            while (!children.empty())
            {
                current = children.front();

                std::string NewName = GenerateUniqueName(current->GetWidgetName());
                current->SetWidgetName(NewName);

                for (int i = 0; i < current->GetChildNum(); i++)
                {
                    children.push(current->GetChildAt(i));
                }
                children.pop();
            }

            return true;
        }

        // 7.4 在指定控件树中指定父节点插入给定子项(通过JSON对象)
        ImWidget* InsertChildWidget(const std::string& widgetVarName,
            const std::string& parentWidgetPath,
            const nlohmann::json& WidgetJson,
            int index);

        // 8.1 移除控件树子项(但不删除)
        bool RemoveChildWidget(const std::string& parentVarName, ImWidget* childWidget, bool bDelete = false)
        {
            if (!childWidget) return false;
            ImWidget* WidgetTreeRoot = GetWidgetVariable(parentVarName);
            if (!WidgetTreeRoot) return false;
            ImWidget* parent = childWidget->GetParents();
            if (!parent) return false;
            if (!parent->IsInTree(WidgetTreeRoot))return false;
            if (WidgetTreeRoot == childWidget) return false;

            return parent->RemoveChild(childWidget, bDelete);
        }

        // 8.2 纯路径字符串版本的移除子控件
        bool RemoveChildWidgetByPath(const std::string& WidgetTreeVarName,
            const std::string& WidgetPath)
        {
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return false;

            // 找到控件
            ImWidget* widgetToRemove = FindWidgetByPath(WidgetTreeRoot, WidgetPath);
            if (!widgetToRemove) return false;

            return RemoveChildWidget(WidgetTreeVarName, widgetToRemove, true);
        }

        // 10.1 重命名控件树子项
        bool RenameChildWidget(const std::string& parentVarName, ImWidget* childWidget,
            const std::string& newName)
        {
            if (!childWidget) return false;

            // 检查新名称是否已使用
            if (IsNameUsed(newName))
                return false;

            // 获取父控件
            ImWidget* WidgetTreeRoot = GetWidgetVariable(parentVarName);
            if (!WidgetTreeRoot) return false;

            if (!childWidget->IsInTree(WidgetTreeRoot))return false;

            if (childWidget == WidgetTreeRoot)
            {
                return RenameVariable(parentVarName, newName);
            }

            // 重命名子控件
            childWidget->SetWidgetName(newName);
            return true;
        }

        // 10.2 纯路径字符串版本的重命名控件
        bool RenameWidgetByPath(const std::string& WidgetTreeVarName,
            const std::string& widgetPath,
            const std::string& newName)
        {
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return false;

            // 找到目标控件
            ImWidget* targetWidget = FindWidgetByPath(WidgetTreeRoot, widgetPath);
            if (!targetWidget) return false;

            // 检查新名称是否可用
            if (IsNameUsed(newName))
                return false;

            // 重命名控件
            targetWidget->SetWidgetName(newName);
            return true;
        }

        // 15. ImObject变量路径属性编辑接口
        template<typename T>
        bool SetObjectPropertyByPath(const std::string& objectVarName,
            const std::string& propertyPath,
            const T& value)
        {
            ImObject* obj = GetObjectVariable(objectVarName);
            if (!obj) return false;

            return obj->SetPathProperty<T>(propertyPath, value);
        }

        // 16. 控件变量控件树节点路径&属性路径编辑接口
        template<typename T>
        bool SetWidgetPropertyByPath(const std::string& widgetVarName,
            const std::string& widgetPath,
            const std::string& propertyPath,
            const T& value)
        {
            ImWidget* rootWidget = GetWidgetVariable(widgetVarName);
            if (!rootWidget) return false;

            // 如果widgetPath为空，表示编辑根控件
            if (widgetPath.empty())
            {
                return rootWidget->SetPathProperty<T>(propertyPath, value);
            }

            // 分割控件路径
            std::vector<std::string> pathParts;
            std::stringstream ss(widgetPath);
            std::string part;
            while (std::getline(ss, part, '/'))
            {
                if (!part.empty())
                {
                    pathParts.push_back(part);
                }
            }

            // 在控件树中导航到目标控件
            ImWidget* currentWidget = rootWidget;
            for (const auto& widgetName : pathParts)
            {
                bool found = false;
                // 在子控件中查找
                for (int i = 0; i < currentWidget->GetChildNum(); ++i)
                {
                    ImWidget* child = currentWidget->GetChildAt(i);
                    if (child && child->GetWidgetName() == widgetName)
                    {
                        currentWidget = child;
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }

            // 编辑目标控件的属性
            return currentWidget->SetPathProperty<T>(propertyPath, value);
        }

        // 17. 控件变量控件树节点slot属性路径编辑接口
        template<typename T>
        bool SetWidgetSlotPropertyByPath(const std::string& widgetVarName,
            const std::string& widgetPath,
            const std::string& propertyPath,
            const T& value)
        {
            ImWidget* rootWidget = GetWidgetVariable(widgetVarName);
            if (!rootWidget) return false;

            // 导航到目标控件
            ImWidget* targetWidget = rootWidget;
            if (!widgetPath.empty())
            {
                std::vector<std::string> pathParts;
                std::stringstream ss(widgetPath);
                std::string part;
                while (std::getline(ss, part, '/'))
                {
                    if (!part.empty())
                    {
                        pathParts.push_back(part);
                    }
                }

                for (const auto& widgetName : pathParts)
                {
                    bool found = false;
                    for (int i = 0; i < targetWidget->GetChildNum(); ++i)
                    {
                        ImWidget* child = targetWidget->GetChildAt(i);
                        if (child && child->GetWidgetName() == widgetName)
                        {
                            targetWidget = child;
                            found = true;
                            break;
                        }
                    }
                    if (!found) return false;
                }
            }

            // 获取控件的Slot
            ImSlot* slot = targetWidget->GetSlotAt();
            if (!slot) return false;

            // 编辑Slot属性
            return slot->SetPathProperty<T>(propertyPath, value);
        }

        // 直接设置已创建的变量（仅供序列化使用）
        bool SetWidgetVariableDirect(const std::string& name, ImWidget* widget)
        {
            if (IsNameUsed(name) && name != widget->GetWidgetName())
                return false;

            widget->SetWidgetName(name);
            auto var = std::make_unique<ImWidgetClassVariable_Widget>(name, widget);
            m_Variables[name] = std::move(var);
            return true;
        }

        bool SetObjectVariableDirect(const std::string& name, ImObject* obj)
        {
            if (IsNameUsed(name))
                return false;

            auto var = std::make_unique<ImWidgetClassVariable_Object>(name, obj);
            m_Variables[name] = std::move(var);
            return true;
        }

        bool SetBasicVariableDirect(const std::string& name, ImWidgetClassVariable_Basic* var)
        {
            if (IsNameUsed(name))
                return false;

            var->SetName(name);
            m_Variables[name] = std::unique_ptr<ImWidgetClassVariable_Basic>(var);
            return true;
        }

        // 清空所有变量
        void ClearAllVariables()
        {
            m_Variables.clear();
            m_DefaultRootVariableName.clear();
        }

        // 导出功能
        bool ExportToCppFiles(const std::string& className,
            const std::string& headerOutputPath,
            const std::string& sourceOutputPath) const;

        bool ExportToJsonFile(const std::string& jsonFileOutputPath);

        nlohmann::json ToJson();
    };
}