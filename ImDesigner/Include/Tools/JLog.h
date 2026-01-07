#pragma once
#include <functional>

void SetLogFun(std::function<void(const std::string&)> Fun);

void AddLogLine(const std::string& Msg);

void AddLogLine(const char* Msg);

#include <sstream>

// Ê¹ÓÃ×Ö·û´®Á÷
template<typename... Args>
void AddLogLineEx(Args&&... args) {
    std::ostringstream oss;
    (oss << ... << std::forward<Args>(args));
    AddLogLine(oss.str());
}