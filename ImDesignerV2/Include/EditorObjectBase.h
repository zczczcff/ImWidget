#pragma once
#include <EditorKit/StatePath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// 编辑器对象基类
class EditorObjectBase
{
protected:
    // 状态路径
    std::string m_Path;

    // 状态路径系统
    class StatePath* m_StateSystem;

    // 绑定列表
    std::vector<std::string> BindList;

private:
    // 存储监听器ID，用于取消监听
    std::unordered_map<std::string, std::vector<ListenerId>> m_ListenerMap;

    // 存储绑定信息
    struct BindingInfo
    {
        std::string path;
        void* valuePtr;
        enum class ValueType { INT, BOOL, FLOAT, STRING } type;
        bool isRelative;
        std::string relativePath; // 仅对相对路径有效
    };
    std::vector<BindingInfo> m_BindingInfos;

public:
    // 构造函数
    EditorObjectBase(StatePath* stateSystem, const std::string& Path = "")
        : m_StateSystem(stateSystem),
        m_Path(Path)
    {
    }

    // 虚析构函数
    virtual ~EditorObjectBase()
    {
        UnbindAllListeners();
    }

    // 路径变更方法
    bool ChangePath(const std::string& newPath);

    // 获取当前路径
    std::string GetPath() const { return m_Path; }

    // 获取状态系统
    StatePath* GetStateSystem() const { return m_StateSystem; }

    // 绑定整数值（相对路径）
    bool BindIntValueRelative(const std::string& RelativePath, int* value_Ptr);

    // 绑定整数值（全局路径）
    bool BindIntValueGlobal(const std::string& GlobalPath, int* value_Ptr);

    // 绑定布尔值（相对路径）
    bool BindBoolValueRelative(const std::string& RelativePath, bool* value_Ptr);

    // 绑定布尔值（全局路径）
    bool BindBoolValueGlobal(const std::string& GlobalPath, bool* value_Ptr);

    // 绑定字符串（相对路径）
    bool BindStringValueRelative(const std::string& RelativePath, std::string* value_Ptr);

    // 绑定字符串（全局路径）
    bool BindStringValueGlobal(const std::string& GlobalPath, std::string* value_Ptr);

    // 绑定浮点数（相对路径）
    bool BindFloatValueRelative(const std::string& RelativePath, float* value_Ptr);

    // 绑定浮点数（全局路径）
    bool BindFloatValueGlobal(const std::string& GlobalPath, float* value_Ptr);

    // 取消所有绑定
    void UnbindAll();

    // 获取绑定数量
    size_t GetBindingCount() const
    {
        return m_BindingInfos.size();
    }

private:
    // 取消所有监听
    void UnbindAllListeners();

public:
	// 监听整型值变化 - 相对路径
	bool WatchIntRelative(const std::string& relativePath,
        std::function<void(int)> callback);

	// 监听整型值变化 - 绝对路径
	bool WatchIntGlobal(const std::string& globalPath,
        std::function<void(int)> callback);

	// 监听浮点值变化 - 相对路径
	bool WatchFloatRelative(const std::string& relativePath,
        std::function<void(float)> callback);

	// 监听浮点值变化 - 绝对路径
	bool WatchFloatGlobal(const std::string& globalPath,
        std::function<void(float)> callback);

	// 监听布尔值变化 - 相对路径
	bool WatchBoolRelative(const std::string& relativePath,
        std::function<void(bool)> callback);

	// 监听布尔值变化 - 绝对路径
	bool WatchBoolGlobal(const std::string& globalPath,
        std::function<void(bool)> callback);

	// 监听字符串变化 - 相对路径
	bool WatchStringRelative(const std::string& relativePath,
        std::function<void(const std::string&)> callback);

	// 监听字符串变化 - 绝对路径
	bool WatchStringGlobal(const std::string& globalPath,
        std::function<void(const std::string&)> callback);

	// 监听指针变化 - 相对路径
	bool WatchPointerRelative(const std::string& relativePath,
        std::function<void(void*)> callback);

	// 监听指针变化 - 绝对路径
	bool WatchPointerGlobal(const std::string& globalPath,
        std::function<void(void*)> callback);

	// 移除特定路径的所有监听器
    void UnwatchPath(const std::string& path);

	// 获取当前监听的路径数量
	size_t GetWatchCount() const
	{
		return m_WatchInfos.size();
	}

private:
    // 监听回调信息结构
    struct WatchInfo
    {
        std::string path;
        bool isRelative;
        std::string relativePath; // 相对路径部分
        enum class ValueType { INT, FLOAT, BOOL, STRING, POINTER } type;

        // 使用std::function存储各种类型的回调
        std::function<void(int)> intCallback;
        std::function<void(float)> floatCallback;
        std::function<void(bool)> boolCallback;
        std::function<void(const std::string&)> stringCallback;
        std::function<void(void*)> pointerCallback;
    };

    std::vector<WatchInfo> m_WatchInfos;
    std::unordered_map<std::string, std::vector<ListenerId>> m_WatchListenerMap;

    // 内部监听实现方法
    bool WatchIntInternal(const std::string& fullPath,
        std::function<void(int)> callback,
        bool isRelative,
        const std::string& relativePath);

    bool WatchFloatInternal(const std::string& fullPath,
        std::function<void(float)> callback,
        bool isRelative,
        const std::string& relativePath);

    bool WatchBoolInternal(const std::string& fullPath,
        std::function<void(bool)> callback,
        bool isRelative,
        const std::string& relativePath);

    bool WatchStringInternal(const std::string& fullPath,
        std::function<void(const std::string&)> callback,
        bool isRelative,
        const std::string& relativePath);

    bool WatchPointerInternal(const std::string& fullPath,
        std::function<void(void*)> callback,
        bool isRelative,
        const std::string& relativePath);

};