#include "Model/Command/Command_PropertyEdit.h"
#include "ImWidget/ImWidget.h"

//PropertyEditCommand::PropertyEditCommand(ImGuiWidget::ImWidget* widget,
//    const std::string& propertyName,
//    void* oldValue, void* newValue,
//    ImGuiWidget::PropertyType type)
//    : m_Widget(widget)
//    , m_PropertyName(propertyName)
//    , m_PropertyType(type)
//{
//    SerializeValue(oldValue, m_OldValue);
//    SerializeValue(newValue, m_NewValue);
//}

//void PropertyEditCommand::Execute()
//{
//    if (!m_Widget) return;
//
//    auto properties = m_Widget->GetProperties();
//    for (const auto& prop : properties)
//    {
//        if (prop.name == m_PropertyName)
//        {
//            DeserializeValue(m_NewValue, prop.getter());
//            prop.setter(prop.getter());
//            break;
//        }
//    }
//}
//
//void PropertyEditCommand::Undo()
//{
//    if (!m_Widget) return;
//
//    auto properties = m_Widget->GetProperties();
//    for (const auto& prop : properties)
//    {
//        if (prop.name == m_PropertyName)
//        {
//            DeserializeValue(m_OldValue, prop.getter());
//            prop.setter(prop.getter());
//            break;
//        }
//    }
//}

//std::string PropertyEditCommand::GetDescription() const
//{
//    std::stringstream ss;
//    ss << "Edit " << m_PropertyName << " of " << m_Widget->GetWidgetName();
//    return ss.str();
//}

//bool PropertyEditCommand::MergeWith(ICommand* other)
//{
//    if (auto* otherCmd = dynamic_cast<PropertyEditCommand*>(other))
//    {
//        if (otherCmd->m_Widget == m_Widget &&
//            otherCmd->m_PropertyName == m_PropertyName)
//        {
//            m_NewValue = otherCmd->m_NewValue;
//            return true;
//        }
//    }
//    return false;
//}

//void PropertyEditCommand::SerializeValue(void* value, std::vector<uint8_t>& buffer)
//{
//    size_t size = GetTypeSize(m_PropertyType);
//    buffer.resize(size);
//    memcpy(buffer.data(), value, size);
//}
//
//void PropertyEditCommand::DeserializeValue(std::vector<uint8_t>& buffer, void* value)
//{
//    if (buffer.empty()) return;
//    memcpy(value, buffer.data(), buffer.size());
//}

//size_t PropertyEditCommand::GetTypeSize(ImGuiWidget::PropertyType type)
//{
//    switch (type)
//    {
//    case PropertyType::Color: return sizeof(ImU32);
//    case PropertyType::Float: return sizeof(float);
//    case PropertyType::Bool: return sizeof(bool);
//    case PropertyType::Int: return sizeof(int);
//    case PropertyType::String: return sizeof(std::string);
//    case PropertyType::Vec2: return sizeof(ImVec2);
//    default: return 0;
//    }
//}