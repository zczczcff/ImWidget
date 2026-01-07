#include <atomic>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>
#include <memory>
#include <queue>
#include <deque>
#include <iomanip>
#include <sstream>
#include <cstring>

class JAsyncLog {
public:
    JAsyncLog(const std::string& file_path,
        size_t buffer_size = 4 * 1024 * 1024,
        size_t recent_logs_capacity = 10000,
        std::chrono::milliseconds flush_interval = std::chrono::milliseconds(3000));

    ~JAsyncLog();

    void Log(const std::string& message);

    // 获取最近的日志（UI线程调用），并移除已获取的日志
    std::vector<std::string> GetRecentLogs(size_t max_count = 1000);

    // 刷新日志到文件
    void Flush();

    // 设置刷盘时间间隔
    void SetFlushInterval(std::chrono::milliseconds interval);

private:
    class Buffer {
    public:
        Buffer(size_t size) : data_(size), write_index_(0) {}

        bool write(size_t pos, const char* msg, size_t len) {
            if (pos + len > data_.size()) return false;
            std::memcpy(&data_[pos], msg, len);
            if (pos + len > write_index_) {
                write_index_ = pos + len;
            }
            return true;
        }

        void clear() { write_index_ = 0; }
        size_t size() const { return data_.size(); }
        size_t written() const { return write_index_; }
        const char* data() const { return data_.data(); }

        // 解析缓冲区中的日志
        std::vector<std::string> ParseLogs();

    private:
        std::vector<char> data_;
        size_t write_index_;
    };

    // 格式化日志
    std::string FormatLog(const std::string& message);

    // 添加到最近日志缓存（后台线程调用）
    void AddToRecentLogs(const std::vector<std::string>& logs);

    // 异步写入文件
    void AsyncWriteToFile(const std::string& log);

    // 交换缓冲区
    void SwapBuffers(const std::string& log);

    // 消费者线程函数
    void ConsumerThread();

    // 定时器线程函数
    void TimerThread();

    // 从空闲队列获取缓冲区
    std::shared_ptr<Buffer> GetFreeBuffer();

    // 将缓冲区返回到空闲队列
    void ReturnBufferToPool(std::shared_ptr<Buffer> buffer);

private:
    // 文件日志相关
    std::string file_path_;
    std::atomic<bool> running_;
    size_t buffer_size_;

    std::shared_ptr<Buffer> current_buffer_;
    size_t current_write_ = 0;

    std::mutex file_mutex_; // 保护所有文件写入操作
    std::condition_variable cond_;
    std::queue<std::shared_ptr<Buffer>> buffers_to_write_;
    std::thread consumer_thread_;

    // 定时器线程相关
    std::thread timer_thread_;
    std::chrono::milliseconds flush_interval_;

    // 空闲缓冲区队列
    std::mutex buffer_pool_mutex_;
    std::queue<std::shared_ptr<Buffer>> buffer_pool_;

    // 最近日志缓存（供UI查询）
    std::mutex recent_logs_mutex_;
    std::deque<std::string> recent_logs_;
    size_t recent_logs_capacity_;
};