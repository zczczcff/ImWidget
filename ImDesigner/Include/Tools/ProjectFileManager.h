#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <memory>

namespace fs = std::filesystem;

class ProjectFileManager
{
private:
    fs::path rootPath;  // 根目录路径
    std::vector<std::string> filters;  // 文件后缀过滤器
    std::map<std::string, std::vector<fs::path>> directoryContents;  // 目录内容
    std::vector<fs::path> allFiles;  // 所有符合条件的文件

    // 文件/目录信息结构
    struct FileSystemInfo
    {
        fs::path path;
        bool isDirectory;
    };

    std::map<std::string, std::vector<FileSystemInfo>> fileSystemStructure;  // 完整的文件系统结构

    // 检查文件后缀是否符合过滤器
    bool matchesFilter(const fs::path& filePath)
    {
        if (filters.empty())
        {
            return true;  // 如果没有过滤器，接受所有文件
        }

        std::string extension = filePath.extension().string();
        if (extension.empty())
        {
            return false;  // 没有后缀的文件
        }

        // 查找是否在过滤器中
        return std::find(filters.begin(), filters.end(), extension) != filters.end();
    }

    // 递归扫描目录
    void scanDirectory(const fs::path& currentPath, const std::string& relativePath = "")
    {
        std::vector<fs::path> filesInDir;
        std::vector<FileSystemInfo> itemsInDir;

        try
        {
            for (const auto& entry : fs::directory_iterator(currentPath))
            {
                if (fs::is_directory(entry.status()))
                {
                    // 如果是目录
                    std::string dirName = entry.path().filename().string();
                    std::string newRelativePath = relativePath.empty() ?
                        dirName : relativePath + "/" + dirName;

                    // 添加到当前目录的项目列表
                    FileSystemInfo dirInfo;
                    dirInfo.path = entry.path().filename();
                    dirInfo.isDirectory = true;
                    itemsInDir.push_back(dirInfo);

                    // 递归扫描子目录
                    scanDirectory(entry.path(), newRelativePath);

                    // 将子目录的信息添加到文件系统结构
                    if (fileSystemStructure.find(newRelativePath) != fileSystemStructure.end() &&
                        !fileSystemStructure[newRelativePath].empty())
                    {
                        // 子目录非空，已在递归中添加到结构
                    }
                }
                else if (fs::is_regular_file(entry.status()))
                {
                    // 如果是文件，检查是否符合过滤器
                    if (matchesFilter(entry.path()))
                    {
                        filesInDir.push_back(entry.path());
                        allFiles.push_back(entry.path());

                        // 添加到当前目录的项目列表
                        FileSystemInfo fileInfo;
                        fileInfo.path = entry.path().filename();
                        fileInfo.isDirectory = false;
                        itemsInDir.push_back(fileInfo);
                    }
                }
            }

            // 存储当前目录的内容
            if (!itemsInDir.empty())
            {
                directoryContents[relativePath] = filesInDir;
                fileSystemStructure[relativePath] = itemsInDir;
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error accessing directory: " << currentPath << " - " << e.what() << std::endl;
        }
    }

public:
    // 文件信息结构
    struct FileInfo
    {
        std::string filename;      // 文件名
        std::string fullPath;      // 完整路径
        bool isDirectory;          // 是否为文件夹
        std::string relativePath;  // 相对于根目录的路径

        FileInfo(const std::string& name = "",
            const std::string& path = "",
            bool isDir = false,
            const std::string& relPath = "")
            : filename(name), fullPath(path), isDirectory(isDir), relativePath(relPath)
        {
        }
    };

    // 构造函数
    ProjectFileManager(const std::string& root, const std::vector<std::string>& fileFilters = {})
        : rootPath(root), filters(fileFilters)
    {
        if (!fs::exists(rootPath) || !fs::is_directory(rootPath))
        {
            throw std::runtime_error("Invalid root directory: " + root);
        }
        scanDirectory(rootPath);
    }
    ProjectFileManager()
    {

    }

    // 查询指定相对目录下的内容，返回FileInfo向量
    std::vector<FileInfo> queryDirectory(const std::string& relativePath = "")
    {
        std::vector<FileInfo> result;

        if (fileSystemStructure.find(relativePath) != fileSystemStructure.end())
        {
            for (const auto& item : fileSystemStructure[relativePath])
            {
                FileInfo info;
                info.filename = item.path.string();
                info.isDirectory = item.isDirectory;

                // 构建完整路径
                fs::path fullPath = rootPath;
                if (!relativePath.empty())
                {
                    fullPath /= relativePath;
                }
                fullPath /= item.path;
                info.fullPath = fullPath.string();

                // 构建相对路径
                if (!relativePath.empty())
                {
                    info.relativePath = relativePath + "/" + info.filename;
                }
                else
                {
                    info.relativePath = info.filename;
                }

                result.push_back(info);
            }
        }

        return result;
    }

    // 遍历整个目录结构，返回所有项目的FileInfo
    std::vector<FileInfo> traverseAll()
    {
        std::vector<FileInfo> allItems;

        for (const auto& dirPair : fileSystemStructure)
        {
            const std::string& relativePath = dirPair.first;

            for (const auto& item : dirPair.second)
            {
                FileInfo info;
                info.filename = item.path.string();
                info.isDirectory = item.isDirectory;

                // 构建完整路径
                fs::path fullPath = rootPath;
                if (!relativePath.empty())
                {
                    fullPath /= relativePath;
                }
                fullPath /= item.path;
                info.fullPath = fullPath.string();

                // 构建相对路径
                if (!relativePath.empty())
                {
                    info.relativePath = relativePath + "/" + info.filename;
                }
                else
                {
                    info.relativePath = info.filename;
                }

                allItems.push_back(info);
            }
        }

        return allItems;
    }

    // 获取所有符合条件的文件
    std::vector<fs::path> getAllFiles() const
    {
        return allFiles;
    }

    // 获取所有符合条件的文件的FileInfo
    std::vector<FileInfo> getAllFilesInfo() const
    {
        std::vector<FileInfo> result;
        for (const auto& filePath : allFiles)
        {
            FileInfo info;
            info.filename = filePath.filename().string();
            info.fullPath = filePath.string();
            info.isDirectory = false;

            // 计算相对于根目录的路径
            fs::path relative = fs::relative(filePath, rootPath);
            info.relativePath = relative.string();

            result.push_back(info);
        }
        return result;
    }

    // 获取指定目录下的所有文件（不包括子目录）的FileInfo
    std::vector<FileInfo> getFilesInDirectory(const std::string& relativePath = "")
    {
        std::vector<FileInfo> result;

        auto items = queryDirectory(relativePath);
        for (const auto& item : items)
        {
            if (!item.isDirectory)
            {
                result.push_back(item);
            }
        }

        return result;
    }

    // 获取指定目录下的所有子目录的FileInfo
    std::vector<FileInfo> getSubdirectories(const std::string& relativePath = "")
    {
        std::vector<FileInfo> result;

        auto items = queryDirectory(relativePath);
        for (const auto& item : items)
        {
            if (item.isDirectory)
            {
                result.push_back(item);
            }
        }

        return result;
    }

    // 递归获取所有子目录的FileInfo
    std::vector<FileInfo> getAllSubdirectories()
    {
        std::vector<FileInfo> result;
        auto allItems = traverseAll();

        for (const auto& item : allItems)
        {
            if (item.isDirectory)
            {
                result.push_back(item);
            }
        }

        return result;
    }

    // 根据是否为文件夹过滤结果
    std::vector<FileInfo> filterByType(const std::vector<FileInfo>& items, bool wantDirectories)
    {
        std::vector<FileInfo> result;
        for (const auto& item : items)
        {
            if (item.isDirectory == wantDirectories)
            {
                result.push_back(item);
            }
        }
        return result;
    }

    // 获取根目录
    std::string getRootPath() const
    {
        return rootPath.string();
    }

    // 获取过滤器
    std::vector<std::string> getFilters() const
    {
        return filters;
    }

    // 打印目录结构
    void printDirectoryStructure(const std::string& relativePath = "", int indent = 0)
    {
        auto items = queryDirectory(relativePath);

        if (!items.empty())
        {
            std::string indentStr(indent, ' ');
            std::cout << indentStr << "[Dir] " << (relativePath.empty() ? "." : relativePath) << std::endl;

            for (const auto& item : items)
            {
                std::string prefix = item.isDirectory ? "[Dir] " : "[File] ";
                std::cout << indentStr << "  ├─ " << prefix << item.filename;

                // 可选：显示文件后缀
                if (!item.isDirectory)
                {
                    std::cout << " (suffix: " << fs::path(item.filename).extension().string() << ")";
                }
                std::cout << std::endl;

                // 如果是目录，递归打印
                if (item.isDirectory)
                {
                    std::string newRelativePath = relativePath.empty() ?
                        item.filename : relativePath + "/" + item.filename;
                    printDirectoryStructure(newRelativePath, indent + 4);
                }
            }
        }
    }

    // 重新扫描目录
    void rescan()
    {
        directoryContents.clear();
        fileSystemStructure.clear();
        allFiles.clear();
        scanDirectory(rootPath);
    }

    // 统计信息
    struct Statistics
    {
        int totalFiles;
        int totalDirectories;
        int filteredFiles;
        std::map<std::string, int> filesByExtension;
    };

    Statistics getStatistics() const
    {
        Statistics stats;
        stats.totalFiles = 0;
        stats.totalDirectories = 0;
        stats.filteredFiles = allFiles.size();

        // 统计目录
        for (const auto& pair : fileSystemStructure)
        {
            for (const auto& item : pair.second)
            {
                if (item.isDirectory)
                {
                    stats.totalDirectories++;
                }
                else
                {
                    stats.totalFiles++;

                    // 按后缀统计
                    std::string ext = item.path.extension().string();
                    if (!ext.empty())
                    {
                        stats.filesByExtension[ext]++;
                    }
                }
            }
        }

        return stats;
    }

    // 打印统计信息
    void printStatistics() const
    {
        Statistics stats = getStatistics();

        std::cout << "=== Directory Statistics ===" << std::endl;
        std::cout << "Root: " << rootPath.string() << std::endl;
        std::cout << "Total directories: " << stats.totalDirectories << std::endl;
        std::cout << "Total files: " << stats.totalFiles << std::endl;
        std::cout << "Filtered files: " << stats.filteredFiles << std::endl;

        if (!stats.filesByExtension.empty())
        {
            std::cout << "Files by extension:" << std::endl;
            for (const auto& pair : stats.filesByExtension)
            {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
    }
};