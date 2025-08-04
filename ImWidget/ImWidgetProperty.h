#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <unordered_set> 

namespace ImGuiWidget 
{
    // 自定义颜色分量提取宏（替代缺失的IM_COL32_R等）
    #define IM_COLOR_GET_R(col) ((col) & 0xFF)
    #define IM_COLOR_GET_G(col) (((col) >> 8) & 0xFF)
    #define IM_COLOR_GET_B(col) (((col) >> 16) & 0xFF)
    #define IM_COLOR_GET_A(col) (((col) >> 24) & 0xFF)
    #define IM_COLOR_SET_RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
    // 属性数据类型枚举
    enum class PropertyType 
    {
        Color,      // ImU32
        Float,      // float
        Bool,       // bool
        Int,        // int
        String,     // std::string
        Vec2,       // ImVec2
        Struct      // PropertyStruct 派生类
    };

    // 属性信息结构
    struct PropertyInfo 
    {
        std::string name;
        PropertyType type;
        std::string category;
        std::function<void(void*)> setter;   // 设置函数
        std::function<void* ()> getter;       // 获取函数

        bool operator==(const PropertyInfo& other) const {
            return name == other.name;
        }

        struct Hasher {
            size_t operator()(const PropertyInfo& pi) const {
                return std::hash<std::string>()(pi.name);
            }
        };
    };

    // 属性结构基类
    class PropertyStruct 
    {
    public:
        virtual ~PropertyStruct() = default;

        // 获取所有可编辑属性
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() = 0;

        // 类型安全的属性访问
        template<typename T>
        void SetProperty(const std::string& name, const T& value)
        {
            for (auto& prop : GetProperties()) 
            {
                if (prop.name == name) 
                {
                    prop.setter(const_cast<T*>(&value));
                    return;
                }
            }
        }

        template<typename T>
        T GetProperty(const std::string& name) const 
        {
            for (auto& prop : GetProperties()) 
            {
                if (prop.name == name) 
                {
                    return *static_cast<T*>(prop.getter());
                }
            }
            return T{};
        }

        template<typename T>
        T* GetPropertyPtr(const std::string& name)
        {
            auto properties = GetProperties();
            PropertyInfo temp;
            temp.name = name;

            auto it = properties.find(temp);
            if (it != properties.end())
            {
                return ((T*)it->getter());
            }
        }
    };



} // namespace ImGuiWidget


    //template<>
    //struct std::hash<ImGuiWidget::PropertyInfo>
    //{
    //    size_t operator()(const ImGuiWidget::PropertyInfo& pi) const
    //    {
    //        return hash<string>()(pi.name);
    //    }
    //};