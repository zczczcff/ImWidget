#include <vector>
#include <string>
#include <algorithm>

#if ((__cplusplus >= 201703L)||(_MSVC_LANG>=201703L)) && __has_include(<filesystem>)
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
    // 获取文件夹下指定后缀名的文件（不区分大小写，可选择递归）
    static std::vector<std::string> getFilesWithExtension(
        const std::string& folder_path,
        const std::string& extension,
        bool recursive = false  // 新增递归参数，默认不递归
    )
    {
        std::vector<std::string> result;

        // 验证路径是否存在且为目录
        fs::path dir(folder_path);
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            return result;
        }

        // 统一后缀名格式
        std::string normalized_ext = normalizeExtension(extension);

        // 根据递归参数选择迭代器类型
        if (recursive) {
            // 递归遍历所有子目录
            for (const auto& entry : fs::recursive_directory_iterator(dir))
            {
                if (fs::is_regular_file(entry.status()))
                {
                    const std::string ext = entry.path().extension().string();
                    if (caseInsensitiveCompare(normalized_ext, ext))
                    {
                        result.push_back(entry.path().string());
                    }
                }
            }
        }
        else {
            // 非递归只遍历当前目录
            for (const auto& entry : fs::directory_iterator(dir))
            {
                if (fs::is_regular_file(entry.status()))
                {
                    const std::string ext = entry.path().extension().string();
                    if (caseInsensitiveCompare(normalized_ext, ext))
                    {
                        result.push_back(entry.path().string());
                    }
                }
            }
        }

        return result;
    }

    // 创建目录（如果不存在）
    static bool createDirectory(const std::string& folder_path)
    {
        fs::path dir(folder_path);

        // 如果路径已存在且是目录，直接返回成功
        if (fs::exists(dir) && fs::is_directory(dir)) {
            return true;
        }

        // 尝试创建目录（包括所有必要的父目录）
        std::error_code ec; // 使用错误码避免异常
        bool success = fs::create_directories(dir, ec);

        // 返回创建结果（成功或失败）
        return success && !ec;
    }

    static std::string getFileNameWithExtension(const std::string& file_path) 
    {
        return fs::path(file_path).filename().string();
    }

    // 新增函数：获取路径B相对于路径A的相对路径（跨平台）
    static std::string getRelativePath(
        const std::string& folderA,
        const std::string& folderB
    )
    {
        fs::path base(folderA);
        fs::path target(folderB);

        // 规范化路径（处理点号/双点号）
        base = base.lexically_normal();
        target = target.lexically_normal();

        // 检查是否在同一个根路径下
        if (base.root_path() != target.root_path()) {
            return target.string(); // 不同根路径时返回绝对路径
        }

        // 计算相对路径
        fs::path relative = target.lexically_relative(base);

        // 处理相同路径的特殊情况
        if (relative.empty()) {
            return "."; // 表示当前目录
        }

        return relative.string();
    }

    //获取输入文件路径的所在文件夹路径
    static std::string getParentDirectory(const std::string& file_path)
    {
        fs::path p(file_path);

        // 处理根目录的特殊情况
        if (p.has_parent_path()) {
            return p.parent_path().string();
        }

        // 当路径为根目录时返回空字符串
        return "";
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