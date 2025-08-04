#pragma once
#include <map>
#include <functional>
#include <memory>
#include <string>
#include "ImWidget.h"

namespace ImGuiWidget
{
    // 控件工厂类
    class ImWidgetFactory
    {
    private:
        // 构造函数私有化
        ImWidgetFactory() { Initialize(); };

        // 控件创建函数类型
        using WidgetCreator = std::function<ImWidget* (const std::string&)>;

        // 类型名称到创建函数的映射
        std::map<std::string, WidgetCreator> m_creators;

        // 类型名称到实际类型的映射
        std::map<std::string, std::string> m_typeNames;

    public:
        // 删除拷贝构造函数和赋值运算符
        ImWidgetFactory(const ImWidgetFactory&) = delete;
        ImWidgetFactory& operator=(const ImWidgetFactory&) = delete;

        // 获取单例实例
        static ImWidgetFactory& GetInstance()
        {
            static ImWidgetFactory instance;
            return instance;
        }

        /**
         * @brief 注册控件类型
         * @tparam WidgetType 控件类型
         * @param typeName 类型名称（字符串标识符）
         */
        template<typename WidgetType>
        void RegisterType(const std::string& typeName)
        {
            // 获取实际类型名称（用于调试）
            std::string actualTypeName = typeid(WidgetType).name();

            // 注册创建函数
            m_creators[typeName] = [](const std::string& name) -> ImWidget* {
                return new WidgetType(name);
            };

            // 保存实际类型名称
            m_typeNames[typeName] = actualTypeName;
        }

        /**
         * @brief 创建控件实例
         * @param typeName 控件类型名称
         * @param widgetName 控件名称
         * @return 创建的控件指针，如果类型未注册返回nullptr
         */
        ImWidget* CreateWidget(const std::string& typeName, const std::string& widgetName)
        {
            auto it = m_creators.find(typeName);
            if (it != m_creators.end()) {
                return it->second(widgetName);
            }
            return nullptr;
        }

        /**
         * @brief 获取已注册的类型列表
         * @return 类型名称列表
         */
        std::vector<std::string> GetRegisteredTypes() const
        {
            std::vector<std::string> types;
            for (const auto& pair : m_creators) {
                types.push_back(pair.first);
            }
            return types;
        }

        /**
         * @brief 获取类型对应的实际类型名称
         * @param typeName 类型名称
         * @return 实际类型名称（来自typeid）
         */
        std::string GetActualTypeName(const std::string& typeName) const
        {
            auto it = m_typeNames.find(typeName);
            if (it != m_typeNames.end()) {
                return it->second;
            }
            return "Unknown";
        }

        /**
         * @brief 初始化工厂，注册所有内置控件类型
         */
        void Initialize();
    };

    // 简化注册的宏
#define REGISTER_WIDGET_TYPE(WidgetType) \
        ImGuiWidget::ImWidgetFactory::GetInstance().RegisterType<WidgetType>(#WidgetType)

    // 简化创建控件的宏
#define CREATE_WIDGET(typeName, widgetName) \
        ImGuiWidget::ImWidgetFactory::GetInstance().CreateWidget(typeName, widgetName)
}