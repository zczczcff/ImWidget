// 文档1: ImInputText.h
#pragma once
#include "ImWidget.h"
#include <string>
#include <functional>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "ImEvent/ImKeyEvent.h"
#include "ImEvent/ImInputEvent.h"
#include "ImEvent/ImDragEvent.h" // 添加拖拽事件支持

namespace ImGuiWidget
{

	// 输入文本模式枚举
	enum class ImInputTextMode
	{
		Text,       // 普通文本模式
		Integer,    // 只能输入整数
		Decimal     // 只能输入小数
	};

    class ImInputText : public ImWidget
    {
    protected:
        std::string m_Text;
        std::string m_PreviousText;
        bool m_IsHandlingEvent = false;
        bool m_NeedsTextUpdate = false;
        //std::string m_Text;
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_BackgroundColor = IM_COL32(255, 255, 255, 255);
        ImU32 m_BorderColor = IM_COL32(100, 100, 100, 255);
        ImU32 m_SelectionColor = IM_COL32(0, 120, 215, 255); // 选中文本背景色
        ImU32 m_SelectionTextColor = IM_COL32(255, 255, 255, 255); // 选中文本颜色
        float m_BorderThickness = 1.0f;
        float m_Rounding = 4.0f;

        int m_CursorPos = 0;
        int m_SelectionStart = -1;
        int m_SelectionEnd = -1;

        // 拖拽选择相关
        bool m_IsSelecting = false;
        int m_SelectionAnchor = -1; // 选择锚点

        // 光标闪烁计时器
        float m_CursorBlinkTimer = 0.0f;
        const float CURSOR_BLINK_RATE = 0.5f;

        int m_VisibleStart = 0;
        int m_VisibleEnd = 0;
        int m_TotalChars = 0;
        float m_VisibleWidth = 0.0f;

        // 输入过滤相关
        ImInputTextMode m_InputMode = ImInputTextMode::Text;
        bool m_AllowNegative = true;        // 是否允许负数
        int m_MaxIntegerDigits = 10;        // 整数部分最大位数
        int m_MaxDecimalDigits = 2;         // 小数部分最大位数（仅Decimal模式有效）

        std::function<void(const std::string&)> OnTextChanged;

        // 验证相关
        std::function<bool(const std::string&)> m_ValidationCallback;
    public:
        // 输入模式设置
        void SetInputMode(ImInputTextMode mode)
        {
            m_InputMode = mode;
            // 切换模式时验证当前文本
            ValidateCurrentText();
        }

        ImInputTextMode GetInputMode() const { return m_InputMode; }

        // 负数设置
        void SetAllowNegative(bool allow)
        {
            m_AllowNegative = allow;
            ValidateCurrentText();
        }
        bool GetAllowNegative() const { return m_AllowNegative; }

        // 位数限制设置
        void SetMaxIntegerDigits(int digits)
        {
            m_MaxIntegerDigits = std::max(1, digits);
            ValidateCurrentText();
        }
        int GetMaxIntegerDigits() const { return m_MaxIntegerDigits; }

        void SetMaxDecimalDigits(int digits)
        {
            m_MaxDecimalDigits = std::max(0, digits);
            ValidateCurrentText();
        }
        int GetMaxDecimalDigits() const { return m_MaxDecimalDigits; }

        // 设置自定义验证回调
        void SetValidationCallback(std::function<bool(const std::string&)> callback)
        {
            m_ValidationCallback = callback;
        }

        // 文本验证方法
        bool IsValidInput(const std::string& text) const
        {
            return ValidateText(text);
        }

        // 获取当前文本是否有效
        bool IsCurrentTextValid() const
        {
            return ValidateText(m_Text);
        }
    protected:
        // 文本验证核心逻辑
        bool ValidateText(const std::string& text) const
        {
            if (text.empty()) return true; // 空文本总是有效的

            // 首先检查自定义验证回调
            if (m_ValidationCallback && !m_ValidationCallback(text))
            {
                return false;
            }

            // 根据输入模式进行验证
            switch (m_InputMode)
            {
            case ImInputTextMode::Text:
                return ValidateTextMode(text);
            case ImInputTextMode::Integer:
                return ValidateIntegerMode(text);
            case ImInputTextMode::Decimal:
                return ValidateDecimalMode(text);
            default:
                return true;
            }
        }

        // 文本模式验证（基本无限制）
        bool ValidateTextMode(const std::string& text) const
        {
            // 文本模式基本不限制，可以添加一些基本检查
            return true;
        }

        // 整数模式验证
        bool ValidateIntegerMode(const std::string& text) const
        {
            if (text.empty()) return true;

            // 检查是否只包含数字和可能的负号
            int startIndex = 0;
            if (text[0] == '-')
            {
                if (!m_AllowNegative) return false;
                startIndex = 1;
                if (text.length() == 1) return false; // 只有负号无效
            }

            // 检查剩余字符是否都是数字
            for (int i = startIndex; i < text.length(); ++i)
            {
                if (text[i] < '0' || text[i] > '9')
                {
                    return false;
                }
            }

            // 检查整数位数限制
            int digitCount = static_cast<int>(text.length()) - startIndex;
            if (digitCount > m_MaxIntegerDigits)
            {
                return false;
            }

            return true;
        }

        // 小数模式验证
        bool ValidateDecimalMode(const std::string& text) const
        {
            if (text.empty()) return true;

            int startIndex = 0;
            if (text[0] == '-')
            {
                if (!m_AllowNegative) return false;
                startIndex = 1;
                if (text.length() == 1) return false; // 只有负号无效
            }

            // 查找小数点
            int dotPosition = -1;
            for (int i = startIndex; i < text.length(); ++i)
            {
                if (text[i] == '.')
                {
                    if (dotPosition != -1) return false; // 多个小数点
                    dotPosition = i;
                }
            }

            // 检查字符有效性
            for (int i = startIndex; i < text.length(); ++i)
            {
                if (text[i] != '.' && (text[i] < '0' || text[i] > '9'))
                {
                    return false;
                }
            }

            // 检查位数限制
            if (dotPosition == -1)
            {
                // 没有小数点，按整数部分检查
                int integerDigits = static_cast<int>(text.length()) - startIndex;
                if (integerDigits > m_MaxIntegerDigits)
                {
                    return false;
                }
            }
            else
            {
                // 有小数点，分别检查整数部分和小数部分
                int integerDigits = dotPosition - startIndex;
                int decimalDigits = static_cast<int>(text.length()) - dotPosition - 1;

                if (integerDigits > m_MaxIntegerDigits)
                {
                    return false;
                }
                if (decimalDigits > m_MaxDecimalDigits)
                {
                    return false;
                }

                // 小数点不能在开头或结尾（如果后面有数字限制）
                if (integerDigits == 0 && decimalDigits == 0)
                {
                    return false; // 只有小数点无效
                }
            }

            return true;
        }

        // 验证并修正当前文本
        void ValidateCurrentText()
        {
            if (!ValidateText(m_Text))
            {
                // 如果当前文本无效，尝试修正或清空
                std::string corrected = CorrectText(m_Text);
                if (ValidateText(corrected))
                {
                    SetText(corrected);
                }
                else
                {
                    SetText(""); // 如果无法修正，清空文本
                }
            }
        }

        // 文本修正逻辑
        std::string CorrectText(const std::string& text) const
        {
            if (text.empty()) return "";

            std::string result;
            bool hasDot = false;
            bool hasNegative = false;

            for (char c : text)
            {
                if (c >= '0' && c <= '9')
                {
                    result += c;
                }
                else if (c == '-' && !hasNegative && m_AllowNegative)
                {
                    if (result.empty())
                    {
                        result += c;
                        hasNegative = true;
                    }
                }
                else if (c == '.' && !hasDot && m_InputMode == ImInputTextMode::Decimal)
                {
                    result += c;
                    hasDot = true;
                }
            }

            return result;
        }

    protected:
        // UTF-8工具函数
        static int Utf8NextChar(int byteIndex, const std::string& str)
        {
            if (byteIndex < 0 || byteIndex >= static_cast<int>(str.size()))
                return byteIndex;

            unsigned char c = static_cast<unsigned char>(str[byteIndex]);

            // 根据UTF-8编码规则判断字符长度
            if ((c & 0x80) == 0x00)
            {
                // 单字节字符 (0xxxxxxx)
                return byteIndex + 1;
            }
            else if ((c & 0xE0) == 0xC0)
            {
                // 双字节字符 (110xxxxx 10xxxxxx)
                return byteIndex + 2;
            }
            else if ((c & 0xF0) == 0xE0)
            {
                // 三字节字符 (1110xxxx 10xxxxxx 10xxxxxx)
                return byteIndex + 3;
            }
            else if ((c & 0xF8) == 0xF0)
            {
                // 四字节字符 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
                return byteIndex + 4;
            }

            // 无效的UTF-8编码，返回下一个字节
            return byteIndex + 1;
        }

        static int Utf8PrevChar(int byteIndex, const std::string& str)
        {
            if (byteIndex <= 0 || byteIndex > static_cast<int>(str.size()))
                return byteIndex;

            // 向前查找UTF-8字符的起始字节
            // UTF-8字符的后续字节都以10xxxxxx开头
            for (int i = byteIndex - 1; i >= 0; --i)
            {
                unsigned char c = static_cast<unsigned char>(str[i]);
                if ((c & 0xC0) != 0x80)
                {
                    // 找到了字符的起始字节
                    return i;
                }
            }

            // 没有找到起始字节，返回字符串开头
            return 0;
        }
        // 获取UTF-8字符串中的字符数量
        size_t Utf8StrLen(const std::string& str) const
        {
            size_t len = 0;
            for (char c : str)
            {
                if ((c & 0xC0) != 0x80) len++;
            }
            return len;
        }

        // 获取第n个字符的字节位置
        int Utf8CharPos(int charIndex) const
        {
            int bytePos = 0;
            int currentChar = 0;
            const char* text = m_Text.c_str();

            while (*text && currentChar < charIndex)
            {
                unsigned int c;
                text += ImTextCharFromUtf8(&c, text, nullptr);
                bytePos += text - (m_Text.c_str() + bytePos);
                currentChar++;
            }
            return bytePos;
        }

        void UpdateVisibleRange()
        {
            ImFont* font = ImGui::GetFont();
            const float fontSize = ImGui::GetFontSize();
            const float padding = 4.0f;
            const float innerWidth = Size.x - padding * 2;

            m_TotalChars = Utf8StrLen(m_Text);
            m_VisibleWidth = innerWidth;

            if (m_TotalChars == 0 || innerWidth <= 0)
            {
                m_VisibleStart = 0;
                m_VisibleEnd = 0;
                return;
            }

            int cursorCharIndex = ByteIndexToCharIndex(m_CursorPos);
            if (cursorCharIndex < m_VisibleStart)
            {
                m_VisibleStart = cursorCharIndex;
            }
            else if (cursorCharIndex >= m_VisibleEnd)
            {
                m_VisibleStart = FindVisibleStartForCursor(cursorCharIndex, innerWidth);
            }
            m_VisibleStart = ImClamp(m_VisibleStart, 0, m_TotalChars - 1);

            CalculateVisibleEnd(innerWidth);
        }

        void CalculateVisibleEnd(float maxWidth)
        {
            ImFont* font = ImGui::GetFont();
            const float fontSize = ImGui::GetFontSize();

            m_VisibleEnd = m_VisibleStart;
            float currentWidth = 0.0f;
            const char* textPtr = m_Text.c_str() + CharIndexToByteIndex(m_VisibleStart);
            int charIndex = m_VisibleStart;
            int endByteIndex = CharIndexToByteIndex(m_VisibleStart);

            while (charIndex < m_TotalChars && currentWidth < maxWidth)
            {
                unsigned int c;
                const char* prevPtr = textPtr;
                textPtr += ImTextCharFromUtf8(&c, textPtr, nullptr);
                int charSize = static_cast<int>(textPtr - prevPtr);

                float charWidth = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, prevPtr, textPtr).x;

                if (currentWidth + charWidth > maxWidth && charIndex > m_VisibleStart)
                {
                    break;
                }

                currentWidth += charWidth;
                m_VisibleEnd = charIndex + 1;
                charIndex++;
                endByteIndex += charSize;
            }
        }

        int FindVisibleStartForCursor(int cursorCharIndex, float maxWidth)
        {
            ImFont* font = ImGui::GetFont();
            const float fontSize = ImGui::GetFontSize();

            float currentWidth = 0.0f;
            int startChar = cursorCharIndex;
            const char* textPtr = m_Text.c_str() + CharIndexToByteIndex(cursorCharIndex);

            while (startChar > 0 && currentWidth < maxWidth)
            {
                int prevByte = Utf8PrevChar(CharIndexToByteIndex(startChar),m_Text);
                const char* charStart = m_Text.c_str() + prevByte;
                unsigned int c;
                ImTextCharFromUtf8(&c, charStart, nullptr);

                int charSize = CharIndexToByteIndex(startChar) - prevByte;
                float charWidth = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, charStart, charStart + charSize).x;

                if (currentWidth + charWidth > maxWidth && startChar < cursorCharIndex)
                {
                    break;
                }

                currentWidth += charWidth;
                startChar--;
            }

            return startChar;
        }

        // 获取字符在屏幕上的像素位置
        float GetCharPixelPosition(int charIndex)
        {
            if (charIndex < 0) return 0.0f;
            if (charIndex >= m_TotalChars)
            {
                charIndex = m_TotalChars;
            }

            ImFont* font = ImGui::GetFont();
            const float fontSize = ImGui::GetFontSize();

            float position = 0.0f;
            const char* textPtr = m_Text.c_str();
            int currentChar = 0;
            int currentByte = 0;

            while (currentChar < charIndex && *textPtr)
            {
                unsigned int c;
                const char* prevPtr = textPtr;
                textPtr += ImTextCharFromUtf8(&c, textPtr, nullptr);
                int charSize = static_cast<int>(textPtr - prevPtr);

                float charWidth = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, prevPtr, textPtr).x;
                position += charWidth;

                currentChar++;
                currentByte += charSize;
            }

            return position;
        }

        void EnsureCursorVisible()
        {
            if (m_CursorPos < 0) return;

            int cursorCharIndex = ByteIndexToCharIndex(m_CursorPos);

            if (cursorCharIndex < m_VisibleStart)
            {
                m_VisibleStart = cursorCharIndex;
                CalculateVisibleEnd(m_VisibleWidth);
            }
            else if (cursorCharIndex >= m_VisibleEnd)
            {
                m_VisibleStart = FindVisibleStartForCursor(cursorCharIndex, m_VisibleWidth);
                CalculateVisibleEnd(m_VisibleWidth);
            }
        }

        // 字节索引转字符索引
        int ByteIndexToCharIndex(int byteIndex) const
        {
            if (byteIndex <= 0) return 0;
            if (byteIndex >= static_cast<int>(m_Text.size())) return m_TotalChars;

            const char* text = m_Text.c_str();
            int charIndex = 0;
            int currentByte = 0;

            while (currentByte < byteIndex && *text)
            {
                unsigned int c;
                const char* next = text + ImTextCharFromUtf8(&c, text, nullptr);
                int charSize = static_cast<int>(next - text);

                if (currentByte + charSize > byteIndex) break;

                currentByte += charSize;
                charIndex++;
                text = next;
            }

            return charIndex;
        }

        // 字符索引转字节索引
        int CharIndexToByteIndex(int charIndex) const
        {
            if (charIndex <= 0) return 0;
            if (charIndex >= m_TotalChars) return static_cast<int>(m_Text.size());

            const char* text = m_Text.c_str();
            int byteIndex = 0;
            int currentChar = 0;

            while (currentChar < charIndex && *text)
            {
                unsigned int c;
                const char* next = text + ImTextCharFromUtf8(&c, text, nullptr);
                byteIndex += static_cast<int>(next - text);
                currentChar++;
                text = next;
            }

            return byteIndex;
        }

    public:
        ImInputText(const std::string& WidgetName)
            : ImWidget(WidgetName), m_PreviousText("")
        {
            SetFocusable(true);
        }

        void HandleEventInternal(ImEvent* event) override
        {
            if (!m_hasFocus && !event->Is<ImFocusEvent>())
            {
                return;
            }

            m_IsHandlingEvent = true;

            // 根据事件类型分发处理
            if (auto keyEvent = event->As<ImKeyEvent>())
            {
                if (HandleKeyEvent(*keyEvent))
                {
                    event->StopPropagation();
                }
            }
            else if (auto textEvent = event->As<ImTextInputEvent>())
            {
                if (HandleTextInputEvent(*textEvent))
                {
                    event->StopPropagation();
                }
            }
            else if (auto mouseEvent = event->As<ImMouseEvent>())
            {
                if (HandleMouseEvent(*mouseEvent))
                {
                    event->StopPropagation();
                }
            }
            else if (auto dragEvent = event->As<ImDragEvent>())
            {
                if (HandleDragEvent(*dragEvent))
                {
                    event->StopPropagation();
                }
            }
            else if (auto focusEvent = event->As<ImFocusEvent>())
            {
                HandleFocusEvent(*focusEvent);
            }

            m_IsHandlingEvent = false;

            if (m_NeedsTextUpdate)
            {
                //m_Text = m_Text;
                m_NeedsTextUpdate = false;
                UpdateVisibleRange();
            }
        }

        bool HandleKeyEvent(const ImKeyEvent& event)
        {
            if (!m_hasFocus) return false;

            switch (event.GetType())
            {
            case ImEventType::KeyDown:
                return HandleKeyDown(event);
            case ImEventType::KeyUp:
                return HandleKeyUp(event);
            default:
                return false;
            }
        }

        bool HandleKeyDown(const ImKeyEvent& event)
        {
            if (event.GetPhase() != ImEventPhase::Target) return false;

            switch (event.GetKeyCode())
            {
            case ImGuiKey_Escape:
                CancelEditing();
                return true;

            case ImGuiKey_Enter:
            case ImGuiKey_KeypadEnter:
                SubmitEditing();
                return true;

            case ImGuiKey_LeftArrow:
                HandleArrowKey(-1, event.IsShiftPressed());
                return true;

            case ImGuiKey_RightArrow:
                HandleArrowKey(1, event.IsShiftPressed());
                return true;

            case ImGuiKey_Home:
                MoveToHome(event.IsShiftPressed());
                return true;

            case ImGuiKey_End:
                MoveToEnd(event.IsShiftPressed());
                return true;

            case ImGuiKey_Backspace:
                HandleBackspace();
                return true;

            case ImGuiKey_Delete:
                HandleDelete();
                return true;

            case ImGuiKey_A:
                if (event.IsCtrlPressed())
                {
                    SelectAll();
                    return true;
                }
                break;

            case ImGuiKey_C:
                if (event.IsCtrlPressed())
                {
                    CopySelection();
                    return true;
                }
                break;

            case ImGuiKey_X:
                if (event.IsCtrlPressed())
                {
                    CutSelection();
                    return true;
                }
                break;

            case ImGuiKey_V:
                if (event.IsCtrlPressed())
                {
                    PasteFromClipboard();
                    return true;
                }
                break;

            default:
                break;
            }

            return false;
        }

        bool HandleKeyUp(const ImKeyEvent& event)
        {
            // 处理按键释放事件（如果需要）
            return false;
        }

        bool HandleTextInputEvent(const ImTextInputEvent& event)
        {
            if (!m_hasFocus) return false;

            const std::string& inputText = event.GetText();
            if (inputText.empty()) return false;

            // 应用输入过滤
            std::string filteredText = FilterInputText(inputText);
            if (filteredText.empty()) return false;

            // 模拟插入后的文本进行验证
            std::string newText = m_Text;
            if (HasSelection())
            {
                int start = ImMin(m_SelectionStart, m_SelectionEnd);
                int end = ImMax(m_SelectionStart, m_SelectionEnd);
                newText.erase(start, end - start);
                newText.insert(start, filteredText);
            }
            else
            {
                newText.insert(m_CursorPos, filteredText);
            }

            // 验证新文本
            if (!ValidateText(newText))
            {
                // 输入无效，可以在这里添加提示音或视觉反馈
                return false;
            }

            m_NeedsTextUpdate = true;
            if (HasSelection())
            {
                DeleteSelection();
            }

            //m_Text = m_Text;
            m_Text.insert(m_CursorPos, filteredText);
            m_CursorPos += static_cast<int>(filteredText.length());
            ClearSelection();

            return true;
        }

        // 输入文本过滤
        std::string FilterInputText(const std::string& inputText) const
        {
            std::string result;

            for (char c : inputText)
            {
                if (IsAllowedCharacter(c))
                {
                    result += c;
                }
            }

            return result;
        }

        // 检查字符是否允许输入
        bool IsAllowedCharacter(char c) const
        {
            switch (m_InputMode)
            {
            case ImInputTextMode::Text:
                return true; // 文本模式允许所有字符

            case ImInputTextMode::Integer:
                // 整数模式只允许数字和负号
                if (c >= '0' && c <= '9') return true;
                if (c == '-' && m_AllowNegative) return true;
                return false;

            case ImInputTextMode::Decimal:
                // 小数模式允许数字、负号和小数点
                if (c >= '0' && c <= '9') return true;
                if (c == '-' && m_AllowNegative) return true;
                if (c == '.') return true;
                return false;

            default:
                return true;
            }
        }

        bool HandleMouseEvent(const ImMouseEvent& event)
        {
            ImVec2 mousePos = event.GetPosition();

            switch (event.GetType())
            {
            case ImEventType::MouseDown:
                if (event.GetButton() == ImMouseButton::Left)
                {
                    RequestFocus();
                    ProcessMouseClick(mousePos);

                    // 鼠标点击时取消选中
                    ClearSelection();
                    m_SelectionAnchor = -1; // 重置选择锚点
                    m_IsSelecting = false; // 不在鼠标点击时开始选择

                    return true;
                }
                break;

            case ImEventType::MouseUp:
                if (event.GetButton() == ImMouseButton::Left)
                {
                    // 鼠标释放时结束选择
                    m_IsSelecting = false;
                    return true;
                }
                break;

            case ImEventType::MouseMove:
                // 鼠标移动时，如果正在选择，则更新选择
                if (m_IsSelecting)
                {
                    ProcessMouseClick(mousePos);
                    UpdateSelectionFromAnchor();
                    return true;
                }
                break;

            default:
                break;
            }

            return false;
        }

        bool HandleDragEvent(const ImDragEvent& event)
        {
            if (!m_hasFocus) return false;

            switch (event.GetType())
            {
            case ImEventType::DragStart:
                // 拖拽开始时设置选择锚点并开始选择
                if (m_SelectionAnchor == -1) // 确保只在拖拽开始时设置一次
                {
                    m_SelectionAnchor = m_CursorPos;
                    m_IsSelecting = true;

                    // 初始化选择范围
                    m_SelectionStart = m_CursorPos;
                    m_SelectionEnd = m_CursorPos;
                }
                return true;

            case ImEventType::DragUpdate:
                // 拖拽过程中更新选择
                if (m_IsSelecting)
                {
                    ProcessMouseClick(event.GetPosition());
                    UpdateSelectionFromAnchor();
                    return true;
                }
                break;

            case ImEventType::DragEnd:
                // 拖拽结束时结束选择
                m_IsSelecting = false;
                m_SelectionAnchor = -1; // 重置选择锚点
                return true;

            default:
                break;
            }

            return false;
        }

        void ProcessMouseClick(const ImVec2& globalMousePos)
        {
            ImVec2 textPos = Position + ImVec2(4, (Size.y - ImGui::GetTextLineHeight()) * 0.5f);
            float mouseX = globalMousePos.x - textPos.x;

            // 找到鼠标点击位置的字符索引
            int newCursorPos = FindCursorPositionFromPixel(mouseX);
            if (newCursorPos != m_CursorPos)
            {
                m_CursorPos = newCursorPos;
                m_CursorBlinkTimer = 0.0f;
                EnsureCursorVisible();
            }
        }

        int FindCursorPositionFromPixel(float pixelX)
        {
            if (pixelX < 0) return CharIndexToByteIndex(m_VisibleStart);

            float currentX = 0.0f;
            int bytePos = CharIndexToByteIndex(m_VisibleStart);
            const char* text = m_Text.c_str() + bytePos;
            int bestPos = bytePos;

            while (*text && currentX < pixelX)
            {
                unsigned int c;
                const char* prevText = text;
                text += ImTextCharFromUtf8(&c, text, nullptr);
                int charSize = static_cast<int>(text - prevText);

                float charWidth = ImGui::GetFont()->CalcTextSizeA(
                    ImGui::GetFontSize(), FLT_MAX, -1.f, prevText, prevText + charSize).x;

                // 如果鼠标位置在字符中间偏右，则定位到下一个字符
                if (pixelX < currentX + charWidth * 0.5f)
                {
                    break;
                }

                currentX += charWidth;
                bestPos += charSize;
            }

            return bestPos;
        }

        void UpdateSelectionFromAnchor()
        {
            if (m_SelectionAnchor == -1) return;

            if (m_CursorPos < m_SelectionAnchor)
            {
                m_SelectionStart = m_CursorPos;
                m_SelectionEnd = m_SelectionAnchor;
            }
            else
            {
                m_SelectionStart = m_SelectionAnchor;
                m_SelectionEnd = m_CursorPos;
            }
        }

        bool HandleFocusEvent(const ImFocusEvent& event)
        {
            switch (event.GetType())
            {
            case ImEventType::FocusIn:
                //m_hasFocus = true;
                m_CursorBlinkTimer = 0.0f;
                return true;

            case ImEventType::FocusOut:
                //m_hasFocus = false;
                CheckTextChanged();
                ClearSelection();
                return true;

            default:
                break;
            }

            return false;
        }

        // 选择相关方法
        bool HasSelection() const
        {
            return m_SelectionStart != -1 && m_SelectionEnd != -1 && m_SelectionStart != m_SelectionEnd;
        }

        void ClearSelection()
        {
            m_SelectionStart = -1;
            m_SelectionEnd = -1;
            m_SelectionAnchor = -1;
        }

        void SelectAll()
        {
            m_SelectionStart = 0;
            m_SelectionEnd = static_cast<int>(m_Text.size());
            m_CursorPos = m_SelectionEnd;
        }

        std::string GetSelectedText() const
        {
            if (!HasSelection()) return "";
            int start = ImMin(m_SelectionStart, m_SelectionEnd);
            int end = ImMax(m_SelectionStart, m_SelectionEnd);
            return m_Text.substr(start, end - start);
        }

        void HandleArrowKey(int direction, bool shiftPressed)
        {
            if (shiftPressed)
            {
                if (!HasSelection())
                {
                    m_SelectionAnchor = m_CursorPos;
                }
                MoveCursor(direction);
                UpdateSelectionFromAnchor();
            }
            else
            {
                MoveCursor(direction);
                ClearSelection();
            }
        }

        void MoveToHome(bool shiftPressed)
        {
            if (shiftPressed)
            {
                if (!HasSelection())
                {
                    m_SelectionAnchor = m_CursorPos;
                }
                m_CursorPos = 0;
                UpdateSelectionFromAnchor();
            }
            else
            {
                m_CursorPos = 0;
                ClearSelection();
            }
            m_CursorBlinkTimer = 0.0f;
        }

        void MoveToEnd(bool shiftPressed)
        {
            if (shiftPressed)
            {
                if (!HasSelection())
                {
                    m_SelectionAnchor = m_CursorPos;
                }
                m_CursorPos = static_cast<int>(m_Text.size());
                UpdateSelectionFromAnchor();
            }
            else
            {
                m_CursorPos = static_cast<int>(m_Text.size());
                ClearSelection();
            }
            m_CursorBlinkTimer = 0.0f;
        }

        void HandleBackspace()
        {
            if (HasSelection())
            {
                DeleteSelection();
            }
            else if (m_CursorPos > 0)
            {
                int prevCharPos = Utf8PrevChar(m_CursorPos,m_Text);
                //m_Text = m_Text;
                m_Text.erase(prevCharPos, m_CursorPos - prevCharPos);
                m_NeedsTextUpdate = true;
                m_CursorPos = prevCharPos;
            }
        }

        void HandleDelete()
        {
            if (HasSelection())
            {
                DeleteSelection();
            }
            else if (m_CursorPos < static_cast<int>(m_Text.size()))
            {
                int nextCharPos = Utf8NextChar(m_CursorPos,m_Text);
                //m_Text = m_Text;
                m_Text.erase(m_CursorPos, nextCharPos - m_CursorPos);
                m_NeedsTextUpdate = true;
            }
        }

        void CutSelection()
        {
            if (HasSelection())
            {
                CopySelection();
                DeleteSelection();
            }
        }

        void PasteFromClipboard()
        {
            const char* clipboard = ImGui::GetClipboardText();
            if (clipboard)
            {
                std::string clipboardText(clipboard);

                // 应用过滤
                std::string filteredText = FilterInputText(clipboardText);
                if (filteredText.empty()) return;

                // 验证粘贴后的文本
                std::string newText = m_Text;
                if (HasSelection())
                {
                    int start = ImMin(m_SelectionStart, m_SelectionEnd);
                    int end = ImMax(m_SelectionStart, m_SelectionEnd);
                    newText.erase(start, end - start);
                    newText.insert(start, filteredText);
                }
                else
                {
                    newText.insert(m_CursorPos, filteredText);
                }

                if (!ValidateText(newText))
                {
                    return; // 粘贴后文本无效，拒绝粘贴
                }

                if (HasSelection())
                {
                    DeleteSelection();
                }

                //m_Text = m_Text;
                m_Text.insert(m_CursorPos, filteredText);
                m_NeedsTextUpdate = true;
                m_CursorPos += static_cast<int>(filteredText.length());
                ClearSelection();
            }
        }

        void DeleteSelection()
        {
            if (!HasSelection()) return;

            int start = ImMin(m_SelectionStart, m_SelectionEnd);
            int end = ImMax(m_SelectionStart, m_SelectionEnd);

            //m_Text = m_Text;
            m_Text.erase(start, end - start);
            m_NeedsTextUpdate = true;
            m_CursorPos = start;
            ClearSelection();
        }

        void CopySelection()
        {
            if (HasSelection())
            {
                std::string selectedText = GetSelectedText();
                ImGui::SetClipboardText(selectedText.c_str());
            }
        }

        // 绘制选中文本
        void DrawSelectedText(ImDrawList* drawList, const ImVec2& textPos)
        {
            if (!HasSelection()) return;

            int start = ImMin(m_SelectionStart, m_SelectionEnd);
            int end = ImMax(m_SelectionStart, m_SelectionEnd);

            // 转换为字符索引
            int startChar = ByteIndexToCharIndex(start);
            int endChar = ByteIndexToCharIndex(end);

            // 确保选择在可见范围内
            if (endChar < m_VisibleStart || startChar > m_VisibleEnd) return;

            // 计算可见部分的选择
            int visibleStartChar = ImMax(startChar, m_VisibleStart);
            int visibleEndChar = ImMin(endChar, m_VisibleEnd);

            if (visibleStartChar >= visibleEndChar) return;

            // 提取选中文本的可见部分
            int startByte = CharIndexToByteIndex(visibleStartChar);
            int endByte = CharIndexToByteIndex(visibleEndChar);
            std::string visibleSelectedText = m_Text.substr(startByte, endByte - startByte);

            // 计算文本位置
            float startX = GetCharPixelPosition(visibleStartChar) - GetCharPixelPosition(m_VisibleStart) + 4.0f;

            // 绘制选中文本
            drawList->AddText(
                ImVec2(textPos.x + startX, textPos.y),
                m_SelectionTextColor,
                visibleSelectedText.c_str()
            );
        }

    public:
        virtual void SetText(const std::string& text)
        {
            m_Text = text;
            m_PreviousText = text;
            m_CursorPos = static_cast<int>(m_Text.size());
            ClearSelection();
            UpdateVisibleRange();
        }

        const std::string& GetText() const { return m_Text; }

        void SetTextColor(ImU32 color) { m_TextColor = color; }
        void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }
        void SetBorderColor(ImU32 color) { m_BorderColor = color; }
        void SetSelectionColor(ImU32 color) { m_SelectionColor = color; }
        void SetSelectionTextColor(ImU32 color) { m_SelectionTextColor = color; }
        void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }
        void SetRounding(float rounding) { m_Rounding = rounding; }

        void SetOnTextChanged(std::function<void(const std::string&)> callback)
        {
            OnTextChanged = callback;
        }

        virtual ImVec2 GetMinSize() override
        {
            ImFont* font = ImGui::GetFont();
            float MinWidth = font->CalcTextSizeA(
                ImGui::GetFontSize(),
                FLT_MAX,
                0.0f,
                "W"
            ).x * 10;

            float textwidth = font->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, m_Text.c_str()).x;
            float height = ImGui::GetTextLineHeight() + 8;

            return ImVec2(ImMax(MinWidth, textwidth + ImGui::GetFontSize()), height);
        }

        virtual void Render() override
        {
            UpdateVisibleRange();
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // 绘制背景和边框
            drawList->AddRectFilled(
                Position,
                Position + Size,
                m_BackgroundColor,
                m_Rounding
            );

            drawList->AddRect(
                Position,
                Position + Size,
                m_BorderColor,
                m_Rounding,
                0,
                m_BorderThickness
            );

            // 更新光标闪烁
            if (m_hasFocus)
            {
                UpdateCursorBlink();
            }

            ImVec2 textPos = Position + ImVec2(4, (Size.y - ImGui::GetTextLineHeight()) * 0.5f);

            // 绘制选择背景
            DrawSelection(drawList, textPos);

            // 绘制文本（分段绘制以支持选择高亮）
            if (m_VisibleStart < m_TotalChars)
            {
                int startByte = CharIndexToByteIndex(m_VisibleStart);
                int endByte = CharIndexToByteIndex(m_VisibleEnd);

                if (HasSelection())
                {
                    // 分段绘制文本：选择前、选中、选择后
                    int selStart = ImMin(m_SelectionStart, m_SelectionEnd);
                    int selEnd = ImMax(m_SelectionStart, m_SelectionEnd);

                    // 确保选择区域在可见范围内
                    if (selEnd > startByte && selStart < endByte)
                    {
                        // 第一段：选择前的文本
                        if (selStart > startByte)
                        {
                            int segEnd = ImMin(selStart, endByte);
                            std::string segment = m_Text.substr(startByte, segEnd - startByte);
                            drawList->AddText(textPos, m_TextColor, segment.c_str());

                            // 更新文本位置
                            float segWidth = ImGui::GetFont()->CalcTextSizeA(
                                ImGui::GetFontSize(), FLT_MAX, 0.0f, segment.c_str()).x;
                            textPos.x += segWidth;
                        }

                        // 第二段：选中的文本
                        int visSelStart = ImMax(selStart, startByte);
                        int visSelEnd = ImMin(selEnd, endByte);
                        if (visSelStart < visSelEnd)
                        {
                            std::string selectedSegment = m_Text.substr(visSelStart, visSelEnd - visSelStart);
                            drawList->AddText(textPos, m_SelectionTextColor, selectedSegment.c_str());

                            float selWidth = ImGui::GetFont()->CalcTextSizeA(
                                ImGui::GetFontSize(), FLT_MAX, 0.0f, selectedSegment.c_str()).x;
                            textPos.x += selWidth;
                        }

                        // 第三段：选择后的文本
                        if (selEnd < endByte)
                        {
                            std::string remainingSegment = m_Text.substr(selEnd, endByte - selEnd);
                            drawList->AddText(textPos, m_TextColor, remainingSegment.c_str());
                        }
                    }
                    else
                    {
                        // 没有选择或选择不在可见范围内，直接绘制整个文本
                        std::string visibleText = m_Text.substr(startByte, endByte - startByte);
                        drawList->AddText(textPos, m_TextColor, visibleText.c_str());
                    }
                }
                else
                {
                    // 没有选择，直接绘制整个文本
                    std::string visibleText = m_Text.substr(startByte, endByte - startByte);
                    drawList->AddText(textPos, m_TextColor, visibleText.c_str());
                }
            }

            // 绘制光标
            if (m_hasFocus && (static_cast<int>(m_CursorBlinkTimer / CURSOR_BLINK_RATE) % 2 == 0))
            {
                int cursorCharIndex = ByteIndexToCharIndex(m_CursorPos);

                if (cursorCharIndex >= m_VisibleStart && cursorCharIndex <= m_VisibleEnd)
                {
                    float cursorX = GetCharPixelPosition(cursorCharIndex) - GetCharPixelPosition(m_VisibleStart) + 4.0f;

                    drawList->AddLine(
                        ImVec2(Position.x + cursorX, Position.y + 4),
                        ImVec2(Position.x + cursorX, Position.y + Size.y - 4),
                        m_TextColor,
                        1.0f
                    );
                }
            }
        }

        // 绘制选择背景
        void DrawSelection(ImDrawList* drawList, const ImVec2& textPos)
        {
            if (!HasSelection()) return;

            int selStart = ImMin(m_SelectionStart, m_SelectionEnd);
            int selEnd = ImMax(m_SelectionStart, m_SelectionEnd);

            // 转换为字符索引
            int selStartChar = ByteIndexToCharIndex(selStart);
            int selEndChar = ByteIndexToCharIndex(selEnd);

            // 确保选择在可见范围内
            if (selEndChar < m_VisibleStart || selStartChar > m_VisibleEnd) return;

            // 计算可见部分的选择
            int visSelStartChar = ImMax(selStartChar, m_VisibleStart);
            int visSelEndChar = ImMin(selEndChar, m_VisibleEnd);

            if (visSelStartChar >= visSelEndChar) return;

            // 计算选择区域的像素位置
            float startX = GetCharPixelPosition(visSelStartChar) - GetCharPixelPosition(m_VisibleStart);
            float endX = GetCharPixelPosition(visSelEndChar) - GetCharPixelPosition(m_VisibleStart);

            // 添加内边距
            startX += 4.0f;
            endX += 4.0f;

            // 绘制选择背景矩形
            drawList->AddRectFilled(
                ImVec2(Position.x + startX, Position.y + 2),
                ImVec2(Position.x + endX, Position.y + Size.y - 2),
                m_SelectionColor,
                m_Rounding * 0.5f
            );
        }

        // 工具函数
        void MoveCursor(int direction)
        {
            if (direction < 0 && m_CursorPos > 0)
            {
                m_CursorPos = Utf8PrevChar(m_CursorPos,m_Text);
            }
            else if (direction > 0 && m_CursorPos < static_cast<int>(m_Text.size()))
            {
                m_CursorPos = Utf8NextChar(m_CursorPos,m_Text);
            }
            m_CursorBlinkTimer = 0.0f;
            EnsureCursorVisible();
        }

        void UpdateCursorBlink()
        {
            ImGuiIO& io = ImGui::GetIO();
            m_CursorBlinkTimer += io.DeltaTime;
            if (m_CursorBlinkTimer > 2 * CURSOR_BLINK_RATE)
            {
                m_CursorBlinkTimer = 0.0f;
            }
        }

        virtual void CheckTextChanged()
        {
            if (m_Text != m_PreviousText && OnTextChanged)
            {
                OnTextChanged(m_Text);
            }
            m_PreviousText = m_Text;
        }

        void CancelEditing()
        {
            m_Text = m_PreviousText;
            //m_hasFocus = false;
            ClearSelection();
        }

        void SubmitEditing()
        {
            //m_hasFocus = false;
            CheckTextChanged();
            ClearSelection();
        }

    public:
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto baseProps = ImWidget::GetProperties();

            baseProps.insert({ "Text", PropertyType::String, "Data",
                    [this](void* v)
                    {
                    std::string newText = *static_cast<std::string*>(v);
                    SetText(newText);
                    },
                    [this]() -> void* { return &m_Text; } });
            baseProps.insert({ "TextColor", PropertyType::Color, "Style",
                    [this](void* v) { m_TextColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_TextColor; } });
            baseProps.insert({ "BackgroundColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BackgroundColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BackgroundColor; } });
            baseProps.insert({ "BorderColor", PropertyType::Color, "Style",
                    [this](void* v) { m_BorderColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_BorderColor; } });
            baseProps.insert({ "SelectionColor", PropertyType::Color, "Style",
                    [this](void* v) { m_SelectionColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_SelectionColor; } });
            baseProps.insert({ "SelectionTextColor", PropertyType::Color, "Style",
                    [this](void* v) { m_SelectionTextColor = *static_cast<ImU32*>(v); },
                    [this]() -> void* { return &m_SelectionTextColor; } });
            baseProps.insert({ "BorderThickness", PropertyType::Float, "Style",
                    [this](void* v) { m_BorderThickness = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_BorderThickness; } });
            baseProps.insert({ "Rounding", PropertyType::Float, "Style",
                    [this](void* v) { m_Rounding = *static_cast<float*>(v); },
                    [this]() -> void* { return &m_Rounding; } });
            // 新增输入过滤属性
            baseProps.insert
            ({
                "InputMode", 
                PropertyType::Enum, 
                "Validation",
                [this](void* v)
                {
                    std::string str = *static_cast<std::string*>(v);
                    if (str == "Text") SetInputMode(ImInputTextMode::Text);
                    else if(str=="Integer")SetInputMode(ImInputTextMode::Integer);
                    else if (str == "Decimal")SetInputMode(ImInputTextMode::Decimal);
                },
                [this]() -> void* 
                {
                    static std::vector<std::string> options;
                    options = { "Text", "Integer", "Decimal" };
                    switch (m_InputMode)
                    {
                    case ImInputTextMode::Text: options.push_back("Text"); break;
                    case ImInputTextMode::Integer: options.push_back("Integer"); break;
                    case ImInputTextMode::Decimal: options.push_back("Decimal"); break;
                    }
                    return static_cast<void*>(&options);
                } });

            baseProps.insert({ "AllowNegative", PropertyType::Bool, "Validation",
                [this](void* v) { SetAllowNegative(*static_cast<bool*>(v)); },
                [this]() -> void* { return &m_AllowNegative; } });

            baseProps.insert({ "MaxIntegerDigits", PropertyType::Int, "Validation",
                [this](void* v) { SetMaxIntegerDigits(*static_cast<int*>(v)); },
                [this]() -> void* { return &m_MaxIntegerDigits; } });

            baseProps.insert({ "MaxDecimalDigits", PropertyType::Int, "Validation",
                [this](void* v) { SetMaxDecimalDigits(*static_cast<int*>(v)); },
                [this]() -> void* { return &m_MaxDecimalDigits; } });
            return baseProps;
        }

        virtual std::string GetRegisterTypeName()override { return "ImInputText"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImInputText(*this);
        }
    };
}