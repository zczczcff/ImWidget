#include "ImWidget/ImObject.h"

namespace ImGuiWidget
{
    class Serializer
    {
    private:
        std::vector<uint8_t> data;
        size_t read_pos = 0;

    public:
        // 写入方法
        void WriteUInt32(uint32_t value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            data.insert(data.end(), bytes, bytes + sizeof(value));
        }

        void WriteFloat(float value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            data.insert(data.end(), bytes, bytes + sizeof(value));
        }

        void WriteBool(bool value)
        {
            data.push_back(value ? 1 : 0);
        }

        void WriteInt(int value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            data.insert(data.end(), bytes, bytes + sizeof(value));
        }

        void WriteString(const std::string& str)
        {
            WriteUInt32(static_cast<uint32_t>(str.size()));
            data.insert(data.end(), str.begin(), str.end());
        }

        void WriteVec2(const ImVec2& vec)
        {
            WriteFloat(vec.x);
            WriteFloat(vec.y);
        }

        // 读取方法
        uint32_t ReadUInt32(const std::vector<uint8_t>& buffer)
        {
            if (read_pos + sizeof(uint32_t) > buffer.size()) return 0;
            uint32_t value;
            memcpy(&value, buffer.data() + read_pos, sizeof(value));
            read_pos += sizeof(uint32_t);
            return value;
        }

        float ReadFloat(const std::vector<uint8_t>& buffer)
        {
            if (read_pos + sizeof(float) > buffer.size()) return 0.0f;
            float value;
            memcpy(&value, buffer.data() + read_pos, sizeof(value));
            read_pos += sizeof(float);
            return value;
        }

        bool ReadBool(const std::vector<uint8_t>& buffer)
        {
            if (read_pos >= buffer.size()) return false;
            return buffer[read_pos++] != 0;
        }

        int ReadInt(const std::vector<uint8_t>& buffer)
        {
            if (read_pos + sizeof(int) > buffer.size()) return 0;
            int value;
            memcpy(&value, buffer.data() + read_pos, sizeof(value));
            read_pos += sizeof(int);
            return value;
        }

        std::string ReadString(const std::vector<uint8_t>& buffer)
        {
            uint32_t len = ReadUInt32(buffer);
            if (read_pos + len > buffer.size()) return "";
            std::string str(buffer.begin() + read_pos, buffer.begin() + read_pos + len);
            read_pos += len;
            return str;
        }

        ImVec2 ReadVec2(const std::vector<uint8_t>& buffer)
        {
            float x = ReadFloat(buffer);
            float y = ReadFloat(buffer);
            return ImVec2(x, y);
        }

        void ResetRead() { read_pos = 0; }
        std::vector<uint8_t>& GetData() { return data; }
        void Clear() { data.clear(); read_pos = 0; }
        size_t GetReadPos() const { return read_pos; }
        void SetReadPos(size_t pos) { read_pos = pos; }
    };

    // 序列化辅助函数
    namespace
    {
        void SerializeProperty(const PropertyInfo& prop, Serializer& ser)
        {
            // 写入属性名
            ser.WriteString(prop.name);

            // 写入属性类型
            ser.WriteUInt32(static_cast<uint32_t>(prop.type));

            // 根据类型写入值
            switch (prop.type)
            {
            case PropertyType::Color: {
                ImU32 color = prop.GetColorValue();
                ser.WriteUInt32(color);
                break;
            }
            case PropertyType::Float: {
                float value = prop.GetFloatValue();
                ser.WriteFloat(value);
                break;
            }
            case PropertyType::Bool: {
                bool value = prop.GetBoolValue();
                ser.WriteBool(value);
                break;
            }
            case PropertyType::Int: {
                int value = prop.GetIntValue();
                ser.WriteInt(value);
                break;
            }
            case PropertyType::String: {
                std::string value = prop.GetStringValue();
                ser.WriteString(value);
                break;
            }
            case PropertyType::Vec2: {
                ImVec2 value = prop.GetVec2Value();
                ser.WriteVec2(value);
                break;
            }
            case PropertyType::Struct: {
                ImObject* structValue = prop.GetStructValue();
                if (structValue)
                {
                    std::vector<uint8_t> nestedData = structValue->Serialize();
                    ser.WriteUInt32(static_cast<uint32_t>(nestedData.size()));
                    ser.GetData().insert(ser.GetData().end(), nestedData.begin(), nestedData.end());
                }
                else
                {
                    ser.WriteUInt32(0); // 空结构体
                }
                break;
            }
            case PropertyType::StringArray: {
                std::vector<std::string> array = prop.GetStringArrayValue();
                ser.WriteUInt32(static_cast<uint32_t>(array.size()));
                for (const auto& str : array)
                {
                    ser.WriteString(str);
                }
                break;
            }
            case PropertyType::Enum: {
                std::string currentValue = prop.GetEnumCurrentValue();
                ser.WriteString(currentValue);
                break;
            }
            }
        }

        // 修改后的反序列化属性函数，接受属性映射
        bool DeserializeProperty(
            ImObject* obj,
            const std::string& propName,
            Serializer& ser,
            const std::vector<uint8_t>& data,
            std::unordered_map<std::string, PropertyInfo>& propertyMap)
        {

            // 从映射中查找属性
            auto it = propertyMap.find(propName);
            if (it == propertyMap.end())
            {
                return false;
            }

            PropertyInfo& prop = it->second;
            uint32_t type = ser.ReadUInt32(data);
            if (static_cast<PropertyType>(type) != prop.type)
            {
                return false;
            }

            switch (prop.type)
            {
            case PropertyType::Color: {
                ImU32 color = ser.ReadUInt32(data);
                return prop.SetColorValue(color);
            }
            case PropertyType::Float: {
                float value = ser.ReadFloat(data);
                return prop.SetFloatValue(value);
            }
            case PropertyType::Bool: {
                bool value = ser.ReadBool(data);
                return prop.SetBoolValue(value);
            }
            case PropertyType::Int: {
                int value = ser.ReadInt(data);
                return prop.SetIntValue(value);
            }
            case PropertyType::String: {
                std::string value = ser.ReadString(data);
                return prop.SetStringValue(value);
            }
            case PropertyType::Vec2: {
                ImVec2 value = ser.ReadVec2(data);
                return prop.SetVec2Value(value);
            }
            case PropertyType::Struct: {
                uint32_t nestedSize = ser.ReadUInt32(data);
                if (nestedSize > 0)
                {
                    ImObject* structValue = prop.GetStructValue();
                    if (structValue)
                    {
                        std::vector<uint8_t> nestedData(data.begin() + ser.GetReadPos(),
                            data.begin() + ser.GetReadPos() + nestedSize);
                        bool result = structValue->Deserialize(nestedData);
                        ser.SetReadPos(ser.GetReadPos() + nestedSize);
                        return result;
                    }
                }
                return true;
            }
            case PropertyType::StringArray: {
                uint32_t arraySize = ser.ReadUInt32(data);
                std::vector<std::string> array;
                for (uint32_t i = 0; i < arraySize; ++i)
                {
                    array.push_back(ser.ReadString(data));
                }
                return prop.SetStringArrayValue(array);
            }
            case PropertyType::Enum: {
                std::string value = ser.ReadString(data);
                return prop.SetEnumValue(value);
            }
            }
            return false;
        }

        void SkipProperty(PropertyType type, Serializer& ser, const std::vector<uint8_t>& data)
        {
            switch (type)
            {
            case PropertyType::Color:
                ser.ReadUInt32(data);
                break;
            case PropertyType::Float:
                ser.ReadFloat(data);
                break;
            case PropertyType::Bool:
                ser.ReadBool(data);
                break;
            case PropertyType::Int:
                ser.ReadInt(data);
                break;
            case PropertyType::String:
                ser.ReadString(data);
                break;
            case PropertyType::Vec2:
                ser.ReadVec2(data);
                break;
            case PropertyType::Struct: {
                uint32_t nestedSize = ser.ReadUInt32(data);
                ser.SetReadPos(ser.GetReadPos() + nestedSize);
                break;
            }
            case PropertyType::StringArray: {
                uint32_t arraySize = ser.ReadUInt32(data);
                for (uint32_t i = 0; i < arraySize; ++i)
                {
                    ser.ReadString(data);
                }
                break;
            }
            case PropertyType::Enum:
                ser.ReadString(data);
                break;
            }
        }
    }

    // ImObject 的序列化实现
    std::vector<uint8_t> ImObject::Serialize()
    {
        Serializer serializer;
        //auto properties = GetProperties();

        //// 写入属性数量
        //serializer.WriteUInt32(static_cast<uint32_t>(properties.size()));

        //// 序列化每个属性
        //for (const auto& prop : properties)
        //{
        //    SerializeProperty(prop, serializer);
        //}

        return serializer.GetData();
    }

    // ImObject 的反序列化实现 - 修改后只调用一次GetProperties()
    bool ImObject::Deserialize(const std::vector<uint8_t>& data)
    {
        //if (data.empty()) return false;

        //Serializer serializer;

        //// 只调用一次GetProperties()，并创建属性名到PropertyInfo的映射
        //auto properties = GetProperties();
        //std::unordered_map<std::string, PropertyInfo> propertyMap;
        //for (const auto& prop : properties)
        //{
        //    propertyMap[prop.name] = prop;
        //}

        //// 读取属性数量
        //uint32_t propertyCount = serializer.ReadUInt32(data);

        //for (uint32_t i = 0; i < propertyCount; ++i)
        //{
        //    // 读取属性名
        //    std::string name = serializer.ReadString(data);

        //    // 保存当前位置以便回退
        //    size_t savedPos = serializer.GetReadPos();

        //    // 在映射中查找对应的属性
        //    auto it = propertyMap.find(name);

        //    if (it != propertyMap.end())
        //    {
        //        // 重置到属性名之后的位置
        //        serializer.SetReadPos(savedPos - sizeof(uint32_t) - name.length());
        //        // 重新读取属性名（DeserializeProperty会期望先读取属性名）
        //        std::string propName = serializer.ReadString(data);

        //        // 使用属性映射进行反序列化
        //        if (!DeserializeProperty(this, propName, serializer, data, propertyMap))
        //        {
        //            return false;
        //        }
        //    }
        //    else
        //    {
        //        // 跳过未知属性
        //        serializer.SetReadPos(savedPos);
        //        uint32_t type = serializer.ReadUInt32(data);
        //        SkipProperty(static_cast<PropertyType>(type), serializer, data);
        //    }
        //}

        return true;
    }
}