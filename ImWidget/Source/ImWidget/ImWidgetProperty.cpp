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

}