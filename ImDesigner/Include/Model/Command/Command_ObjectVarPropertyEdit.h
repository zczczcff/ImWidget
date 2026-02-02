#pragma once

#include "ImDesignerCommandBase.h"
#include <boost/type_index/ctti_type_index.hpp>

// =========================== 3. Object Ù–‘±‡º≠√¸¡Óª˘¿‡ ===========================
template<typename T>
class ObjectPropertyEditCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_ObjectVarName;
    std::string m_PropertyPath;
    T m_OldValue;
    T m_NewValue;

public:
    ObjectPropertyEditCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& objectVarName,
        const std::string& propertyPath,
        const T& oldValue,
        const T& newValue)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::ObjectPropertyEdit,
                static_cast<int>(ObjectPropertyEditSubType::SetObjectProperty),
                boost::typeindex::ctti_type_index::type_id<T>().pretty_name())),
        m_ObjectVarName(objectVarName),
        m_PropertyPath(propertyPath),
        m_OldValue(oldValue),
        m_NewValue(newValue)
    {
    }

    virtual ~ObjectPropertyEditCommandBase() = default;

    virtual bool Execute() override
    {
        return m_TargetClass->SetObjectPropertyByPath<T>(m_ObjectVarName, m_PropertyPath, m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_TargetClass->SetObjectPropertyByPath<T>(m_ObjectVarName, m_PropertyPath, m_OldValue);
    }

    virtual std::string GetDescription() const override
    {
        return "Edit Object Property: " + m_ObjectVarName + " -> " + m_PropertyPath;
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        if (!ImUserWidgetClassCommandBase::IsWithinMergeWindow(other))
            return false;

        return (m_TargetClass == otherCmd->m_TargetClass) &&
            (m_ObjectVarName == otherCmd->m_ObjectVarName) &&
            (m_PropertyPath == otherCmd->m_PropertyPath)&&
            (m_Data==other->m_Data)
            ;
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        if (!CanMergeWith(other.get())) return false;

        auto* otherCmd = static_cast<ObjectPropertyEditCommandBase<T>*>(other.get());
        m_NewValue = otherCmd->m_NewValue;
        return true;
    }
};