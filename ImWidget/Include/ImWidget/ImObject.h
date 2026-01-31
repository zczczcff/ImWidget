#pragma once
#include <vector>
#include <functional>
#include <memory>
#include <unordered_set> 
#include <imgui.h>
#include <ROP/RunTimeObjectProperty.h>
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
        Struct,      // ImObject 派生类
        StringArray,     //新增：数组
        Enum
    };

    inline std::string PropertyTypeToString(const PropertyType& type)
    {
        switch (type)
        {
        case PropertyType::Color: return "Color";
        case PropertyType::Float: return "Float";
        case PropertyType::Bool: return "Bool";
        case PropertyType::Int: return "Int";
        case PropertyType::String: return "String";
        case PropertyType::Vec2: return "Vec2";
        case PropertyType::Struct: return "Struct";
        case PropertyType::StringArray:return "StringArray";
        case PropertyType::Enum:return "Enum";
        default: return "UnknownType";
        }
    }

    inline PropertyType StringToPropertyType(const std::string& s)
    {
        if (s == "Color") return PropertyType::Color;
        else if (s == "Float") return PropertyType::Float;
        else if (s == "Bool") return PropertyType::Bool;
        else if (s == "Int") return PropertyType::Int;
        else if (s == "String") return PropertyType::String;
        else if (s == "Vec2") return PropertyType::Vec2;
        else if (s == "Struct") return PropertyType::Struct;
        else if (s == "StringArray") return PropertyType::StringArray;
        else if (s == "Enum") return PropertyType::Enum;
        else return PropertyType::Float;
    }

    class ImObject;
    typedef ImObject PropertyStruct ;
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

        bool SetStructValue(ImObject* value)
        {
            if (type != PropertyType::Struct) return false;
            setter(static_cast<void*>(value));
            return true;
        }

        ImObject* GetStructValue()const
        {
            if (type != PropertyType::Struct) return nullptr;
            return static_cast<ImObject*>(getter());
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

    using ImProperty = ROP::Property<PropertyType>;

    // 属性结构基类
    class ImObject :public ROP::PropertyObject<PropertyType>
    {
		DECLARE_OBJECT(PropertyType, ImObject)
		END_DECLARE_OBJECT()
    public:
        virtual ~ImObject() = default;

        // 获取所有可编辑属性
        //virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() { return std::unordered_set<PropertyInfo, PropertyInfo::Hasher>(); }

        // 类型安全的属性访问
        //bool SetProperty(const std::string& name, void* value)
        //{
        //    auto properties = GetProperties();
        //    PropertyInfo temp;
        //    temp.name = name;

        //    auto it = properties.find(temp);
        //    if (it != properties.end())
        //    {
        //        it->setter(value);
        //        return true;
        //    }
        //    return false;
        //}

        //template<typename T>
        //T GetProperty(const std::string& name)
        //{
        //    for (auto& prop : GetProperties()) 
        //    {
        //        if (prop.name == name) 
        //        {
        //            return *static_cast<T*>(prop.getter());
        //        }
        //    }
        //    return T{};
        //}

 // 通过路径获取属性指针
        template<typename T>
        T* GetPropertyPtr(const std::string& name)
        {
            ImProperty prop = ROP::PropertyObject<PropertyType>::GetProperty(name);
            return prop.GetPointer<T>();
        }

        // 通过路径获取属性指针（嵌套路径）
        template<typename T>
        T* GetPathPropertyPtr(const std::string& path)
        {
            ImProperty prop = GetPathProperty(path);
            if (prop.IsValid())
            {
                return prop.GetPointer<T>();
            }
            return nullptr;
        }

        template<typename T>
        bool SetPropertyValue(const std::string& name, const T& value)
        {
            ImProperty prop = ROP::PropertyObject<PropertyType>::GetProperty(name);
            if (prop.IsValid())
            {
                prop.SetValue<T>(value);
                return true;
            }
            else
            {
                return false;
            }
        }

        // 通过路径设置属性值（嵌套路径）
        template<typename T>
        bool SetPathProperty(const std::string& path, const T& value)
        {
            ImProperty prop = GetPathProperty(path);
            if (prop.IsValid())
            {
                prop.SetValue<T>(value);
                return true;
            }
            return false;
        }

        // 通过路径获取属性（支持嵌套路径）
        ImProperty GetPathProperty(const std::string& path)
        {
            // 分割路径
            std::vector<std::string> pathParts;
            std::stringstream ss(path);
            std::string part;
            while (std::getline(ss, part, '/'))
            {
                if (!part.empty())
                {
                    pathParts.push_back(part);
                }
            }

            if (pathParts.empty())
            {
                return ImProperty(); // 无效属性
            }

            // 从当前对象开始
            ImObject* currentObject = this;
            ImProperty currentProp;

            // 遍历除最后一部分外的所有路径部分
            for (size_t i = 0; i < pathParts.size() - 1; ++i)
            {
                const std::string& partName = pathParts[i];

                // 获取当前对象的属性
                currentProp = currentObject->ROP::PropertyObject<PropertyType>::GetProperty(partName);

                if (!currentProp.IsValid())
                {
                    return ImProperty(); // 无效属性
                }

                // 检查属性类型是否为Struct
                if (currentProp.GetType() != PropertyType::Struct)
                {
                    return ImProperty(); // 不是Struct类型，无法继续导航
                }

                // 获取结构对象
                ImObject* structObj = currentProp.GetPointer<ImObject>();
                if (!structObj)
                {
                    return ImProperty(); // 结构对象为空
                }

                // 更新当前对象
                currentObject = structObj;
            }

            // 获取最后一个属性
            const std::string& lastPartName = pathParts.back();
            return currentObject->ROP::PropertyObject<PropertyType>::GetProperty(lastPartName);
        }

        // 通过路径获取属性值（模板特化）
        template<typename T>
        T GetPathPropertyValue(const std::string& path)
        {
            ImProperty prop = GetPathProperty(path);
            if (prop.IsValid())
            {
                return prop.GetValue<T>();
            }
            return T{};
        }
        // 序列化函数
        std::vector<uint8_t> Serialize();

        // 反序列化函数
        bool Deserialize(const std::vector<uint8_t>& data);

        virtual std::string GetRegisterTypeName() { return ""; }
    };



} // namespace ImGuiWidget


#define DECLARE_IMOBJECT(classname,parentclassname) DECLARE_OBJECT_WITH_PARENT(ImGuiWidget::PropertyType,classname,parentclassname)
#define END_DECLARE_IMOBJECT() END_DECLARE_OBJECT()

    //template<>
    //struct std::hash<ImGuiWidget::PropertyInfo>
    //{
    //    size_t operator()(const ImGuiWidget::PropertyInfo& pi) const
    //    {
    //        return hash<string>()(pi.name);
    //    }
    //};