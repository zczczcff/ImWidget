#include <vector>
#include <string>
#include <algorithm>

#if __cplusplus >= 201703L && __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "需要至少支持 C++17 或 experimental/filesystem"
#endif

class FileUtil 
{
public:
    // 获取文件夹下指定后缀名的文件（不区分大小写）
    static std::vector<std::string> getFilesWithExtension(
        const std::string& folder_path,
        const std::string& extension
    ) 
    {
        std::vector<std::string> result;

        // 验证路径是否存在且为目录
        fs::path dir(folder_path);
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            return result;
        }

        // 统一后缀名格式（确保以 '.' 开头）
        std::string normalized_ext = normalizeExtension(extension);

        // 遍历目录中的所有文件
        for (const auto &entry : fs::directory_iterator(dir))
        {
            // 修改点：使用统一的文件状态检查方式
            if (fs::is_regular_file(entry.status()))
            {
                const std::string ext = entry.path().extension().string();
                if (caseInsensitiveCompare(normalized_ext, ext))
                {
                    result.push_back(entry.path().string());
                }
            }
        }

        return result;
    }

    static std::string getFileNameWithExtension(const std::string& file_path) 
    {
        return fs::path(file_path).filename().string();
    }

    // 获取纯净文件名（不含路径和后缀）
    // 示例："/home/user/document.txt" -> "document"
    static std::string getPureFileName(const std::string& file_path) 
    {
        return fs::path(file_path).stem().string();
    }
private:
    // 统一后缀名格式（确保以 '.' 开头）
    static std::string normalizeExtension(std::string ext) 
    {
        if (ext.empty()) return ".";

        // 移除开头的空格
        ext.erase(0, ext.find_first_not_of(" \t"));
        ext.erase(ext.find_last_not_of(" \t") + 1);

        // 添加缺失的点号
        if (ext[0] != '.') {
            ext = "." + ext;
        }

        return ext;
    }

    // 不区分大小写的字符串比较
    static bool caseInsensitiveCompare(const std::string& str1, const std::string& str2) 
    {
        if (str1.length() != str2.length()) return false;

        return std::equal(str1.begin(), str1.end(), str2.begin(),
            [](char a, char b) {
                return std::tolower(static_cast<unsigned char>(a)) ==
                    std::tolower(static_cast<unsigned char>(b));
            });
    }
};