#pragma once
#include <map>
#include <functional>
#include <memory>
#include <string>
#include "ImObject.h"

namespace ImGuiWidget
{
    // 对象工厂
    class ImObjectFactory
    {
        friend class ImGlobalInstance;
    private:
        // 构造函数私有化
        ImObjectFactory() { Initialize(); };
        // 对象创建器
        using ObjectCreator = std::function<ImObject* ()>;

        // 类型映射
        std::map<std::string, ObjectCreator> m_creators;

        // 类型名称与实际类型映射
        std::map<std::string, std::string> m_typeNames;

    public:
        // 删除拷贝构造函数和赋值操作符
        ImObjectFactory(const ImObjectFactory&) = delete;
        ImObjectFactory& operator=(const ImObjectFactory&) = delete;

        // 获取单例实例
        static ImObjectFactory& GetInstance()
        {
            static ImObjectFactory instance;
            return instance;
        }

        /**
         * @brief 注册对象类型
         * @tparam ObjectType 对象类型
         * @param typeName 类型名称字符串标识
         */
        template<typename ObjectType>
        void RegisterType()
        {
            // 获取实际类型名称（用于调试）
            std::string actualTypeName = typeid(ObjectType).name();
            ImObject* tempobject = new ObjectType();

            // 注册创建器
            m_creators[tempobject->GetRegisterTypeName()] = []() -> ImObject*
            {
                return new ObjectType();
            };

            // 存储类型映射
            m_typeNames[tempobject->GetRegisterTypeName()] = actualTypeName;

            delete tempobject;
        }

        /**
         * @brief 创建对象实例
         * @param typeName 对象类型
         * @param objectName 对象名称
         * @return 目标对象指针，如果未注册则返回nullptr
         */
        ImObject* CreateObject(const std::string& typeName)
        {
            auto it = m_creators.find(typeName);
            if (it != m_creators.end())
            {
                return it->second();
            }
            return nullptr;
        }

        /**
         * @brief 获取注册类型列表
         * @return 类型列表
         */
        std::vector<std::string> GetRegisteredTypes() const
        {
            std::vector<std::string> types;
            for (const auto& pair : m_creators)
            {
                types.push_back(pair.first);
            }
            return types;
        }

        /**
         * @brief 获取类型对应的实际类型名称
         * @param typeName 类型名称
         * @return 实际类型名称（typeid）
         */
        std::string GetActualTypeName(const std::string& typeName) const
        {
            auto it = m_typeNames.find(typeName);
            if (it != m_typeNames.end())
            {
                return it->second;
            }
            return "Unknown";
        }

        /**
         * @brief 初始化注册所有对象类型
         */
        void Initialize() {};
    };

    // 注册对象的宏
#define REGISTER_OBJECT_TYPE(ObjectType) \
            ImGuiWidget::ImObjectFactory::GetInstance().RegisterType<ObjectType>()

    // 创建对象的宏
#define CREATE_OBJECT(typeName) \
            ImGuiWidget::ImObjectFactory::GetInstance().CreateObject(typeName)
}