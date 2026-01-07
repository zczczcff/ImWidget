#include "Tools/JAsyncLog.h"

// 构造函数
JAsyncLog::JAsyncLog(const std::string& file_path,
    size_t buffer_size,
    size_t recent_logs_capacity,
    std::chrono::milliseconds flush_interval)
    : file_path_(file_path),
    running_(true),
    buffer_size_(buffer_size),
    recent_logs_capacity_(recent_logs_capacity),
    flush_interval_(flush_interval) {

    // 初始化当前缓冲区
    current_buffer_ = GetFreeBuffer();

    // 启动消费者线程
    consumer_thread_ = std::thread(&JAsyncLog::ConsumerThread, this);

    // 启动定时器线程
    timer_thread_ = std::thread(&JAsyncLog::TimerThread, this);
}

// 析构函数
JAsyncLog::~JAsyncLog() {
    running_ = false;
    {
        std::lock_guard<std::mutex> lock(file_mutex_);
        cond_.notify_all(); // 通知所有等待的线程
    }

    if (consumer_thread_.joinable()) {
        consumer_thread_.join();
    }

    if (timer_thread_.joinable()) {
        timer_thread_.join();
    }

    Flush();
}

// 记录日志
void JAsyncLog::Log(const std::string& message) {
    // 1. 格式化日志
    auto formatted_log = FormatLog(message);

    // 2. 异步写入文件缓冲区
    AsyncWriteToFile(formatted_log);
}

// 获取最近的日志（UI线程调用），并移除已获取的日志
std::vector<std::string> JAsyncLog::GetRecentLogs(size_t max_count) {
    std::lock_guard<std::mutex> lock(recent_logs_mutex_);

    // 计算实际获取的数量
    size_t count = std::min(max_count, recent_logs_.size());

    // 复制请求数量的日志
    std::vector<std::string> result;
    for (size_t i = 0; i < count; i++) {
        result.push_back(std::move(recent_logs_.front()));
        recent_logs_.pop_front();
    }

    return result;
}

// 格式化日志
std::string JAsyncLog::FormatLog(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
    ss << message;
    return ss.str();
}

// 添加到最近日志缓存（后台线程调用）
void JAsyncLog::AddToRecentLogs(const std::vector<std::string>& logs) {
    std::lock_guard<std::mutex> lock(recent_logs_mutex_);

    for (const auto& log : logs) {
        recent_logs_.push_back(log);

        // 维护缓存大小
        if (recent_logs_.size() > recent_logs_capacity_) {
            recent_logs_.pop_front();
        }
    }
}

// 异步写入文件
void JAsyncLog::AsyncWriteToFile(const std::string& log) {
    std::lock_guard<std::mutex> lock(file_mutex_);

    size_t msg_size = log.size() + 1;  // +1 for newline

    // 尝试写入当前缓冲区
    if (current_write_ + msg_size <= current_buffer_->size()) {
        if (current_buffer_->write(current_write_, log.data(), log.size())) {
            current_buffer_->write(current_write_ + log.size(), "\n", 1);
            current_write_ += msg_size;
            return;
        }
    }

    // 缓冲区不足时交换缓冲区
    SwapBuffers(log);
}

// 交换缓冲区
void JAsyncLog::SwapBuffers(const std::string& log) {
    // 将当前缓冲区加入待写入队列
    buffers_to_write_.push(current_buffer_);

    // 获取新的缓冲区
    current_buffer_ = GetFreeBuffer();

    // 重置写入位置
    current_write_ = 0;

    // 写入新日志
    current_buffer_->write(0, log.data(), log.size());
    current_buffer_->write(log.size(), "\n", 1);
    current_write_ = log.size() + 1;

    // 通知后台线程
    cond_.notify_one();
}

// 消费者线程函数
void JAsyncLog::ConsumerThread() {
    std::ofstream log_file(file_path_, std::ios::app);
    if (!log_file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + file_path_);
    }

    while (running_ || !buffers_to_write_.empty()) {
        std::unique_lock<std::mutex> lock(file_mutex_);

        // 等待缓冲区或退出信号
        if (buffers_to_write_.empty() && running_) {
            cond_.wait_for(lock, std::chrono::seconds(2), [this] {
                return !buffers_to_write_.empty() || !running_;
                });
        }

        // 检查是否有待处理缓冲区
        if (buffers_to_write_.empty()) {
            if (!running_) break;
            continue;
        }

        // 获取待处理缓冲区
        std::queue<std::shared_ptr<Buffer>> temp_queue;
        temp_queue.swap(buffers_to_write_);
        lock.unlock();

        // 处理所有缓冲区
        while (!temp_queue.empty()) {
            auto buffer = temp_queue.front();

            // 1. 写入文件
            log_file.write(buffer->data(), buffer->written());

            // 2. 解析日志并添加到UI队列
            auto logs = buffer->ParseLogs();
            if (!logs.empty()) {
                AddToRecentLogs(logs);
            }

            temp_queue.pop();

            // 3. 清空并回收缓冲区
            buffer->clear();
            ReturnBufferToPool(buffer);
        }

        log_file.flush();
    }
}

// 定时器线程函数
void JAsyncLog::TimerThread() {
    while (running_) {
        // 等待指定的时间间隔
        std::this_thread::sleep_for(flush_interval_);

        if (!running_) break;

        // 执行刷盘操作
        std::lock_guard<std::mutex> lock(file_mutex_);

        // 检查当前缓冲区是否有数据
        if (current_write_ > 0) {
            // 将当前缓冲区加入待写入队列
            buffers_to_write_.push(current_buffer_);
            current_buffer_ = GetFreeBuffer();
            current_write_ = 0;

            // 通知消费者线程
            cond_.notify_one();
        }
    }
}

// 从空闲队列获取缓冲区
std::shared_ptr<JAsyncLog::Buffer> JAsyncLog::GetFreeBuffer() {
    std::lock_guard<std::mutex> lock(buffer_pool_mutex_);

    if (buffer_pool_.empty()) {
        return std::make_shared<Buffer>(buffer_size_);
    }

    auto buffer = buffer_pool_.front();
    buffer_pool_.pop();
    return buffer;
}

// 将缓冲区返回到空闲队列
void JAsyncLog::ReturnBufferToPool(std::shared_ptr<Buffer> buffer) {
    std::lock_guard<std::mutex> lock(buffer_pool_mutex_);
    buffer_pool_.push(buffer);
}

// 刷新日志到文件
void JAsyncLog::Flush() {
    std::lock_guard<std::mutex> lock(file_mutex_);

    // 将当前缓冲区加入待写入队列
    if (current_write_ > 0) {
        buffers_to_write_.push(current_buffer_);
        current_buffer_ = GetFreeBuffer();
        current_write_ = 0;
        cond_.notify_one();
    }

    // 处理所有剩余缓冲区
    while (!buffers_to_write_.empty()) {
        auto buffer = buffers_to_write_.front();
        buffers_to_write_.pop();

        // 写入文件
        std::ofstream log_file(file_path_, std::ios::app);
        log_file.write(buffer->data(), buffer->written());
        log_file.flush();

        // 回收缓冲区
        buffer->clear();
        ReturnBufferToPool(buffer);
    }
}

// 设置刷盘时间间隔
void JAsyncLog::SetFlushInterval(std::chrono::milliseconds interval) {
    flush_interval_ = interval;
}

// 解析缓冲区中的日志
std::vector<std::string> JAsyncLog::Buffer::ParseLogs() {
    std::vector<std::string> logs;
    const char* start = data_.data();
    const char* end = start + write_index_;
    const char* current = start;

    while (current < end) {
        const char* newline = static_cast<const char*>(std::memchr(current, '\n', end - current));
        if (!newline) break;

        logs.emplace_back(current, newline);
        current = newline + 1;
    }

    return logs;
}