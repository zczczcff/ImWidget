#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include "ImWidget.h"
#include "ImObject.h"
#include "ImObjectFactory.h"
#include "ImWidgetFactory.h"
#include "ImUserWidget.h"
#include "ImPanelWidget.h"
#include "nlohmann/json.hpp"

namespace ImGuiWidget
{
    // 基本属性变量类，用于包装基本类型的属性
    class ImBasicVariable : public ImObject
    {
    public:
        enum class BasicType
        {
            Int,
            Float,
            Bool,
            String,
            Color
        };

    private:
        BasicType m_Type;
        std::string m_Name;
        std::string m_Category;

        union
        {
            int m_IntValue;
            float m_FloatValue;
            bool m_BoolValue;
        };

        std::string m_StringValue;
        ImU32 m_ColorValue;

    public:
        ImBasicVariable(const std::string& name, BasicType type, const std::string& category = "Default")
            : m_Name(name), m_Type(type), m_Category(category)
        {
            // 初始化默认值
            switch (type)
            {
            case BasicType::Int: m_IntValue = 0; break;
            case BasicType::Float: m_FloatValue = 0.0f; break;
            case BasicType::Bool: m_BoolValue = false; break;
            case BasicType::String: m_StringValue = ""; break;
            case BasicType::Color: m_ColorValue = IM_COL32(255, 255, 255, 255); break;
            }
        }

        BasicType GetBasicType() const { return m_Type; }
        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }

        void* GetValuePtr()
        {
            switch (m_Type)
            {
            case BasicType::Int: return &m_IntValue;
            case BasicType::Float: return &m_FloatValue;
            case BasicType::Bool: return &m_BoolValue;
            case BasicType::String: return &m_StringValue;
            case BasicType::Color: return &m_ColorValue;
            default: return nullptr;
            }
        }

        std::string GetRegisterTypeName() override
        {
            return "BasicVariable_" + std::to_string(static_cast<int>(m_Type));
        }
    };

    // 用户控件类管理器
    class ImUserWidgetClass
    {
    private:
        std::string m_ClassName;
        std::string m_Namespace = "ImGuiWidget";          // 自定义命名空间
        std::string m_BaseClass = "ImUserWidget";        // 自定义基类

        // 变量存储
        std::unordered_map<std::string, ImWidget*> m_WidgetVariables;      // 控件树变量
        std::unordered_map<std::string, ImObject*> m_ObjectVariables;      // ImObject变量
        std::unordered_map<std::string, ImBasicVariable*> m_BasicVariables; // 基本属性变量

        std::string m_DefaultRootVariableName; // 默认根控件变量名

        // 名称检查辅助函数
        bool IsNameUsed(const std::string& name) const
        {
            return m_WidgetVariables.find(name) != m_WidgetVariables.end() ||
                m_ObjectVariables.find(name) != m_ObjectVariables.end() ||
                m_BasicVariables.find(name) != m_BasicVariables.end() ||
                CheckAllChildWidgetNames(name);
        }

        // 递归检查所有控件树子项名称
        bool CheckAllChildWidgetNames(const std::string& name) const
        {
            for (const auto& pair : m_WidgetVariables)
            {
                if (CheckWidgetTreeForName(pair.second, name))
                {
                    return true;
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
            if (widget->GetChildNum()>0)
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
            if (!IsNameUsed(baseName))return baseName;
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

            // 如果路径为空或为"."，返回根控件
            if (path.empty() || path == ".")
                return root;

            // 分割路径
            std::vector<std::string> pathParts;
            std::stringstream ss(path);
            std::string part;
            while (std::getline(ss, part, '/'))
            {
                if (!part.empty() && part != ".")
                {
                    pathParts.push_back(part);
                }
            }

            // 遍历路径查找控件
            ImWidget* current = root;
            for (const auto& partName : pathParts)
            {
                bool found = false;

                // 如果是特殊路径".."，表示父控件
                if (partName == "..")
                {
                    current = current->GetParents();
                    if (!current) return nullptr;
                    found = true;
                }
                else
                {
                    // 在当前控件的子控件中查找
                    for (int i = 0; i < current->GetChildNum(); i++)
                    {
                        ImWidget* child = current->GetChildAt(i);
                        if (child && child->GetWidgetName() == partName)
                        {
                            current = child;
                            found = true;
                            break;
                        }
                    }
                }

                if (!found) return nullptr;
            }

            return current;
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
            // 清理所有变量
            for (auto& pair : m_WidgetVariables) delete pair.second;
            for (auto& pair : m_ObjectVariables) delete pair.second;
            for (auto& pair : m_BasicVariables) delete pair.second;
        }

        // 设置/获取命名空间
        void SetNamespace(const std::string& ns) { m_Namespace = ns; }
        std::string GetNamespace() const { return m_Namespace; }

        // 设置/获取基类
        void SetBaseClass(const std::string& baseClass) { m_BaseClass = baseClass; }
        std::string GetBaseClass() const { return m_BaseClass; }

        // 1. 获取类名
        std::string GetClassName() const { return m_ClassName; }
        void SetClassName(const std::string& name) { m_ClassName = name; }

        // 2. 设置/获取默认根控件
        void SetDefaultRootVariable(const std::string& varName)
        {
            if (m_WidgetVariables.find(varName) != m_WidgetVariables.end())
            {
                m_DefaultRootVariableName = varName;
            }
        }

        std::string GetDefaultRootVariableName() const { return m_DefaultRootVariableName; }

        ImWidget* GetDefaultRootWidget() const
        {
            auto it = m_WidgetVariables.find(m_DefaultRootVariableName);
            return it != m_WidgetVariables.end() ? it->second : nullptr;
        }

        // 3. 添加变量接口
        bool AddWidgetVariable(const std::string& typeName, std::string& outVarName)
        {
            // 创建控件
            ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(typeName, "");
            if (!widget) return false;

            // 生成唯一名称
            std::string baseName = typeName;
            // 移除命名空间前缀
            size_t pos = baseName.find_last_of("::");
            if (pos != std::string::npos)
                baseName = baseName.substr(pos + 1);

            outVarName = GenerateUniqueName(baseName);
            widget->SetWidgetName(outVarName);

            m_WidgetVariables[outVarName] = widget;
            return true;
        }

        bool AddObjectVariable(const std::string& typeName, std::string& outVarName)
        {
            // 创建对象
            ImObject* obj = ImObjectFactory::GetInstance().CreateObject(typeName);
            if (!obj) return false;

            // 生成唯一名称
            std::string baseName = typeName;
            outVarName = GenerateUniqueName(baseName);

            m_ObjectVariables[outVarName] = obj;
            return true;
        }

        bool AddBasicVariable(ImBasicVariable::BasicType type, const std::string& category, std::string& outVarName)
        {
            // 生成唯一名称
            std::string baseName;
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: baseName = "Int"; break;
            case ImBasicVariable::BasicType::Float: baseName = "Float"; break;
            case ImBasicVariable::BasicType::Bool: baseName = "Bool"; break;
            case ImBasicVariable::BasicType::String: baseName = "String"; break;
            case ImBasicVariable::BasicType::Color: baseName = "Color"; break;
            }

            outVarName = GenerateUniqueName(baseName);

            // 创建基本变量
            ImBasicVariable* var = new ImBasicVariable(outVarName, type, category);
            m_BasicVariables[outVarName] = var;
            return true;
        }

        // 4. 删除变量接口
        bool RemoveVariable(const std::string& varName)
        {
            // 检查控件树变量
            auto widgetIt = m_WidgetVariables.find(varName);
            if (widgetIt != m_WidgetVariables.end())
            {
                delete widgetIt->second;
                m_WidgetVariables.erase(widgetIt);

                // 如果删除的是默认根控件，清空默认根
                if (varName == m_DefaultRootVariableName)
                {
                    m_DefaultRootVariableName.clear();
                }
                return true;
            }

            // 检查对象变量
            auto objectIt = m_ObjectVariables.find(varName);
            if (objectIt != m_ObjectVariables.end())
            {
                delete objectIt->second;
                m_ObjectVariables.erase(objectIt);
                return true;
            }

            // 检查基本变量
            auto basicIt = m_BasicVariables.find(varName);
            if (basicIt != m_BasicVariables.end())
            {
                delete basicIt->second;
                m_BasicVariables.erase(basicIt);
                return true;
            }

            return false;
        }

        // 5. 修改变量接口
        ImWidget* GetWidgetVariable(const std::string& varName) const
        {
            auto it = m_WidgetVariables.find(varName);
            return it != m_WidgetVariables.end() ? it->second : nullptr;
        }

        ImObject* GetObjectVariable(const std::string& varName) const
        {
            auto it = m_ObjectVariables.find(varName);
            return it != m_ObjectVariables.end() ? it->second : nullptr;
        }

        ImBasicVariable* GetBasicVariable(const std::string& varName) const
        {
            auto it = m_BasicVariables.find(varName);
            return it != m_BasicVariables.end() ? it->second : nullptr;
        }

        // 6. 获取所有变量名称
        std::vector<std::string> GetAllVariableNames() const
        {
            std::vector<std::string> names;

            for (const auto& pair : m_WidgetVariables) names.push_back(pair.first);
            for (const auto& pair : m_ObjectVariables) names.push_back(pair.first);
            for (const auto& pair : m_BasicVariables) names.push_back(pair.first);

            return names;
        }

        // 7. 在指定控件树中指定父节点中插入新建子项
        ImWidget* InsertChildWidget(const std::string& WidgetTreeVarName, ImWidget* parent,const std::string& InsertWidgetRegisterName,
            int index)
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

        //在指定控件树中指定父节点插入给定子项
        bool InsertChildWidget(const std::string& WidgetTreeVarName, ImWidget* parent, ImWidget* child, int index)
        {
            if (!parent) return false;
            if (!child) return false;
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return false;
            if (!parent->IsInTree(WidgetTreeRoot)) return false;

            if (!parent->InsertChildAt(index, child))
            {
                return false;
            }

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
        }

        // 8.1 纯路径字符串版本的插入子控件（通过类型名创建）
        ImWidget* InsertChildWidgetByPath(const std::string& WidgetTreeVarName,
            const std::string& parentPath,
            const std::string& InsertWidgetRegisterName,
            int index)
        {
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return nullptr;

            // 找到父控件
            ImWidget* parent = FindWidgetByPath(WidgetTreeRoot, parentPath);
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


        // 8. 移除控件树子项(但不删除)
        bool RemoveChildWidget(const std::string& parentVarName, ImWidget* childWidget)
        {
            if (!childWidget) return false;
            ImWidget* WidgetTreeRoot = GetWidgetVariable(parentVarName);
            if (!WidgetTreeRoot) return false;
            ImWidget* parent = childWidget->GetParents();
            if (!parent) return false;
            if (!parent->IsInTree(WidgetTreeRoot))return false;
            if (WidgetTreeRoot == childWidget) return false;

            return parent->RemoveChild(childWidget);
        }

        // 8.3 纯路径字符串版本的移除子控件
        bool RemoveChildWidgetByPath(const std::string& WidgetTreeVarName,
            const std::string& parentPath,
            const std::string& childName)
        {
            ImWidget* WidgetTreeRoot = GetWidgetVariable(WidgetTreeVarName);
            if (!WidgetTreeRoot) return false;

            // 找到父控件
            ImWidget* parent = FindWidgetByPath(WidgetTreeRoot, parentPath);
            if (!parent) return false;

            // 找到子控件
            for (int i = 0; i < parent->GetChildNum(); i++)
            {
                ImWidget* child = parent->GetChildAt(i);
                if (child && child->GetWidgetName() == childName)
                {
                    return parent->RemoveChild(child, true);
                }
            }

            return false;
        }

        // 9. 重命名变量
        bool RenameVariable(const std::string& oldName, const std::string& newName)
        {
            // 检查新名称是否已使用
            if (IsNameUsed(newName) && newName != oldName)
                return false;

            // 重命名控件树变量
            auto widgetIt = m_WidgetVariables.find(oldName);
            if (widgetIt != m_WidgetVariables.end())
            {
                widgetIt->second->SetWidgetName(newName);
                ImWidget* widget = widgetIt->second;
                m_WidgetVariables.erase(widgetIt);
                m_WidgetVariables[newName] = widget;

                // 如果重命名的是默认根控件，更新默认根名称
                if (oldName == m_DefaultRootVariableName)
                {
                    m_DefaultRootVariableName = newName;
                }
                return true;
            }

            // 重命名对象变量
            auto objectIt = m_ObjectVariables.find(oldName);
            if (objectIt != m_ObjectVariables.end())
            {
                ImObject* obj = objectIt->second;
                m_ObjectVariables.erase(objectIt);
                m_ObjectVariables[newName] = obj;
                return true;
            }

            // 重命名基本变量
            auto basicIt = m_BasicVariables.find(oldName);
            if (basicIt != m_BasicVariables.end())
            {
                ImBasicVariable* var = basicIt->second;
                var->SetName(newName);
                m_BasicVariables.erase(basicIt);
                m_BasicVariables[newName] = var;
                return true;
            }

            return false;
        }

        // 10. 重命名控件树子项
        bool RenameChildWidget(const std::string& parentVarName, ImWidget* childWidget, const std::string& newName)
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

        // 9.1 纯路径字符串版本的重命名控件
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

        // 11. 获取所有控件树变量的名称
        std::vector<std::string> GetWidgetVariableNames() const
        {
            std::vector<std::string> names;
            for (const auto& pair : m_WidgetVariables)
                names.push_back(pair.first);
            return names;
        }

        // 12. 获取所有对象变量的名称
        std::vector<std::string> GetObjectVariableNames() const
        {
            std::vector<std::string> names;
            for (const auto& pair : m_ObjectVariables)
                names.push_back(pair.first);
            return names;
        }

        // 13. 获取所有基本变量的名称
        std::vector<std::string> GetBasicVariableNames() const
        {
            std::vector<std::string> names;
            for (const auto& pair : m_BasicVariables)
                names.push_back(pair.first);
            return names;
        }

        // 14. 递归获取控件树下所有子项名称
        std::vector<std::string> GetAllChildWidgetNames(const std::string& parentVarName) const
        {
            std::vector<std::string> names;
            ImWidget* parent = GetWidgetVariable(parentVarName);

            if (parent)
            {
                CollectChildWidgetNames(parent, names);
            }

            return names;
        }

        // 15. ImObject变量路径属性编辑接口
        // 通过变量名和属性路径编辑ImObject变量的属性
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
        // 通过控件变量名、控件树路径和属性路径编辑控件属性
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
        // 编辑控件树中指定控件的Slot属性
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

    private:
        // 递归收集控件树下所有子项名称
        void CollectChildWidgetNames(ImWidget* widget, std::vector<std::string>& names) const
        {
            if (!widget) return;

            // 添加当前控件名称
            names.push_back(widget->GetWidgetName());

            // 如果是容器控件，递归收集子项
            if (widget->GetChildNum()>0)
            {
                for (int i = 0; i < widget->GetChildNum(); i++)
                {
                    ImWidget* child = widget->GetChildAt(i);
                    CollectChildWidgetNames(child, names);
                }
            }
		}
	public:
		// 直接设置已创建的控件树变量
		bool SetWidgetVariableDirect(const std::string& name, ImWidget* widget)
		{
			if (IsNameUsed(name) && name != widget->GetWidgetName())
				return false;

			widget->SetWidgetName(name);
			m_WidgetVariables[name] = widget;
			return true;
		}

		// 直接设置已创建的对象变量
		bool SetObjectVariableDirect(const std::string& name, ImObject* obj)
		{
			if (IsNameUsed(name))
				return false;

			m_ObjectVariables[name] = obj;
			return true;
		}

		// 直接设置已创建的基本变量
		bool SetBasicVariableDirect(const std::string& name, ImBasicVariable* var)
		{
			if (IsNameUsed(name) && name != var->GetName())
				return false;

			var->SetName(name);
			m_BasicVariables[name] = var;
			return true;
		}

		// 清空所有变量（用于反序列化前清理）
		void ClearAllVariables()
		{
			for (auto& pair : m_WidgetVariables) delete pair.second;
			for (auto& pair : m_ObjectVariables) delete pair.second;
			for (auto& pair : m_BasicVariables) delete pair.second;

			m_WidgetVariables.clear();
			m_ObjectVariables.clear();
			m_BasicVariables.clear();
			m_DefaultRootVariableName.clear();
		}

        bool ExportToCppFiles(const std::string& className,
            const std::string& headerOutputPath,
            const std::string& sourceOutputPath) const;

        bool ExportToJsonFile(const std::string& jsonFileOutputPath);

        nlohmann::json ToJson();

	};



}