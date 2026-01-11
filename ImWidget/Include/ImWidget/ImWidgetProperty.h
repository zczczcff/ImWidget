#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <unordered_set> 
#include <imgui.h>
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
        Struct,      // PropertyStruct 派生类
        StringArray,     //新增：数组
        Enum
    };

    //std::string PropertyTypeToString(const PropertyType& type)
    //{
    //    switch (type)
    //    {
    //    case PropertyType::Color: return "Color";
    //    case PropertyType::Float: return "Float";
    //    case PropertyType::Bool: return "Bool";
    //    case PropertyType::Int: return "Int";
    //    case PropertyType::String: return "String";
    //    case PropertyType::Vec2: return "Vec2";
    //    case PropertyType::Struct: return "Struct";
    //    case PropertyType::StringArray:return "StringArray";
    //    case PropertyType::Enum:return "Enum";
    //    default: return "UnknownType";
    //    }
    //}

    //PropertyType StringToPropertyType(const std::string& s)
    //{
    //    if (s == "Color") return PropertyType::Color;
    //    else if (s == "Float") return PropertyType::Float;
    //    else if (s == "Bool") return PropertyType::Bool;
    //    else if (s == "Int") return PropertyType::Int;
    //    else if (s == "String") return PropertyType::String;
    //    else if (s == "Vec2") return PropertyType::Vec2;
    //    else if (s == "Struct") return PropertyType::Struct;
    //    else if (s == "StringArray") return PropertyType::StringArray;
    //    else if (s == "Enum") return PropertyType::Enum;
    //    else return PropertyType::Float;
    //}

    class PropertyStruct;
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

        bool SetColorValue(ImU32 value)
        {
            if (type != PropertyType::Color) return false;
            setter(&value);
            return true;
        }

        ImU32 GetColorValue() const
        {
            if (type != PropertyType::Color) return INT32_MAX;
            return *static_cast<ImU32*>(getter());
        }

        bool SetFloatValue(float value)
        {
            if (type != PropertyType::Float) return false;
            setter(&value);
            return true;
        }

        float GetFloatValue()const
        {
            if (type != PropertyType::Float) return 0.0f;
            return *static_cast<float*>(getter());
        }

        bool SetBoolValue(bool value)
        {
            if (type != PropertyType::Bool) return false;
            setter(&value);
            return true;
        }

        bool GetBoolValue()const
        {
            if (type != PropertyType::Bool) return false;
            return *static_cast<bool*>(getter());
        }

        bool SetIntValue(int value)
        {
            if (type != PropertyType::Int) return false;
            setter(&value);
            return true;
        }

        int GetIntValue()const
        {
            if (type != PropertyType::Int) return 0;
            return *static_cast<int*>(getter());
        }

        bool SetStringValue(const std::string& value)
        {
            if (type != PropertyType::String) return false;
            setter(const_cast<void*>(static_cast<const void*>(&value)));
            return true;
        }

        std::string GetStringValue()const
        {
            if (type != PropertyType::String) return "";
            return *static_cast<std::string*>(getter());
        }

        bool SetVec2Value(const ImVec2& value)
        {
            if (type != PropertyType::Vec2) return false;
            setter(const_cast<void*>(static_cast<const void*>(&value)));
            return true;
        }

        ImVec2 GetVec2Value()const
        {
            if (type != PropertyType::Vec2) return ImVec2(0, 0);
            return *static_cast<ImVec2*>(getter());
        }

        bool SetStructValue(PropertyStruct* value)
        {
            if (type != PropertyType::Struct) return false;
            setter(static_cast<void*>(value));
            return true;
        }

        PropertyStruct* GetStructValue()const
        {
            if (type != PropertyType::Struct) return nullptr;
            return static_cast<PropertyStruct*>(getter());
        }

        bool SetStringArrayValue(const std::vector<std::string>& value)
        {
            if (type != PropertyType::StringArray) return false;
            setter(const_cast<void*>(static_cast<const void*>(&value)));
            return true;
        }

        std::vector<std::string> GetStringArrayValue()const
        {
            if (type != PropertyType::StringArray) return {};
            return *static_cast<std::vector<std::string>*>(getter());
        }

        bool SetEnumValue(std::string value)//使用字符串设置枚举
        {
            if (type != PropertyType::Enum) return false;
            setter(const_cast<void*>(static_cast<const void*>(&value)));
            return true;
        }

        std::vector<std::string> GetEnumOptions()const//获取枚举列表
        {
            if (type != PropertyType::Enum) return std::vector<std::string>();
            std::vector<std::string> temp= *static_cast<std::vector<std::string>*>(getter());
            temp.pop_back();
            return temp;
        }

        std::string GetEnumCurrentValue()const//获取枚举变量字符串表示
        {
            if (type != PropertyType::Enum) return "";
            std::vector<std::string> temp = *static_cast<std::vector<std::string>*>(getter());
            return temp.back();
        }

    };

    // 属性结构基类
    class PropertyStruct 
    {
    public:
        virtual ~PropertyStruct() = default;

        // 获取所有可编辑属性
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() { return std::unordered_set<PropertyInfo, PropertyInfo::Hasher>(); }

        // 类型安全的属性访问
        bool SetProperty(const std::string& name, void* value)
        {
            auto properties = GetProperties();
            PropertyInfo temp;
            temp.name = name;

            auto it = properties.find(temp);
            if (it != properties.end())
            {
                it->setter(value);
                return true;
            }
            return false;
        }

        template<typename T>
        T GetProperty(const std::string& name)
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

        template<typename T>
        bool SetPropertyValue(const std::string& name, const T& value)
        {
            T copy = value;
            return SetProperty(name, &copy);
        }

        // 序列化函数
        std::vector<uint8_t> Serialize();

        // 反序列化函数
        bool Deserialize(const std::vector<uint8_t>& data);

        virtual std::string GetRegisterName() { return ""; }
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