#pragma once

#include "ImWidget.h"
#include "../Application/ImApplication.h"
#include <imgui_internal.h>


namespace ImGuiWidget
{
	extern ImApplication* GlobalApp;
	class ImImage :public ImWidget
	{
	private:
		ImTextureID m_TextureID;
		int OriginalWidth, OriginalHeight;
		ImU32 TintColor;
		bool m_MaintainAspectRatio; // 新增：保持原始长宽比属性

	public:
		ImImage(const std::string& WidgetName) :ImWidget(WidgetName), m_TextureID(0), OriginalWidth(0), OriginalHeight(0), TintColor(IM_COL32(255, 255, 255, 255)), m_MaintainAspectRatio(true) {}

		ImImage(const std::string& WidgetName, const std::string& FilePath) :ImWidget(WidgetName), m_TextureID(0), TintColor(IM_COL32(255, 255, 255, 255)), m_MaintainAspectRatio(true)
		{
			LoadImageFromFile(FilePath);
		}

		ImImage(const std::string& WidgetName, ImTextureID TextureID, int Width, int Height) :ImWidget(WidgetName), m_MaintainAspectRatio(true), TintColor(IM_COL32(255, 255, 255, 255))
		{
			SetTextureID(TextureID, Width, Height);
		}

		// 拷贝构造函数
		ImImage(const ImImage& other)
			: ImWidget(other),
			m_TextureID(other.m_TextureID), // 注意：纹理ID是共享的，可能需要引用计数管理
			OriginalWidth(other.OriginalWidth),
			OriginalHeight(other.OriginalHeight),
			TintColor(other.TintColor),
			m_MaintainAspectRatio(other.m_MaintainAspectRatio)
		{
		}

		void SetTextureID(ImTextureID TextureID, int newOriginalWidth = -1, int newOriginalHeight = -1)
		{
			m_TextureID = TextureID;
			if (newOriginalWidth > 0)
			{
				OriginalWidth = newOriginalWidth;
			}
			if (newOriginalHeight > 0)
			{
				OriginalHeight = newOriginalHeight;
			}
		}

		bool LoadImageFromFile(const std::string& FilePath)
		{
			if (m_TextureID != 0)
			{
				GlobalApp->ReleaseTexture(m_TextureID);
				m_TextureID = 0;
			}
			m_TextureID = GlobalApp->LoadTextureFromFile(FilePath.c_str(), OriginalWidth, OriginalHeight);
			if (m_TextureID != 0)
			{
				return true;
			}
			else
			{
				OriginalWidth = 0;
				OriginalHeight = 0;
				return false;
			}
		}

		virtual ImVec2 GetMinSize() { return ImVec2(OriginalWidth, OriginalHeight); }

		void SetAlpha(float alpha)
		{
			ImVec4 Temp = ImGui::ColorConvertU32ToFloat4(TintColor);
			Temp.w = alpha;
			TintColor = ImGui::ColorConvertFloat4ToU32(Temp);
		}

		virtual void Render() override
		{
			if (m_TextureID == 0) return;

			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImVec2 renderSize = Size;
			ImVec2 renderPos = Position;

			// 如果启用了保持长宽比，调整渲染尺寸
			if (m_MaintainAspectRatio && OriginalWidth > 0 && OriginalHeight > 0)
			{
				float aspectRatio = (float)OriginalWidth / (float)OriginalHeight;
				float currentAspectRatio = Size.x / Size.y;

				if (currentAspectRatio > aspectRatio)
				{
					// 当前宽度相对高度过大，调整宽度以匹配高度
					renderSize.x = Size.y * aspectRatio;
					renderPos.x += (Size.x - renderSize.x) * 0.5f; // 居中显示
				}
				else
				{
					// 当前高度相对宽度过大，调整高度以匹配宽度
					renderSize.y = Size.x / aspectRatio;
					renderPos.y += (Size.y - renderSize.y) * 0.5f; // 居中显示
				}
			}

			window->DrawList->AddImage(m_TextureID, renderPos, renderPos + renderSize, ImVec2(0, 0), ImVec2(1, 1), TintColor);
		}

		virtual std::string GetRegisterTypeName()override { return "ImImage"; }

		virtual ImWidget* CopyWidget() override
		{
			return new ImImage(*this);
		}

		void SetOriginalSize(int Width, int Height)
		{
			OriginalWidth = Width;
			OriginalHeight = Height;
		}

		// 新增：设置是否保持原始长宽比
		void SetMaintainAspectRatio(bool maintain)
		{
			m_MaintainAspectRatio = maintain;
		}

		// 新增：获取是否保持原始长宽比
		bool GetMaintainAspectRatio() const
		{
			return m_MaintainAspectRatio;
		}

		// 新增：重写GetProperties以包含新属性
		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
		{
			auto props = ImWidget::GetProperties();
			props.insert({
				"MaintainAspectRatio", PropertyType::Bool, "Maintain Aspect Ratio",
				[this](void* v) { m_MaintainAspectRatio = *(bool*)v; },
				[this]() -> void* { return &m_MaintainAspectRatio; }
				});
			props.insert({
				"TintColor", PropertyType::Color, "Tint Color",
				[this](void* v) { TintColor = *(ImU32*)v; },
				[this]() -> void* { return &TintColor; }
				});
			return props;
		}

		// 新增：计算保持长宽比后的尺寸
		ImVec2 CalculateAspectRatioSize(const ImVec2& availableSize) const
		{
			if (!m_MaintainAspectRatio || OriginalWidth <= 0 || OriginalHeight <= 0)
				return availableSize;

			float aspectRatio = (float)OriginalWidth / (float)OriginalHeight;
			float availableAspectRatio = availableSize.x / availableSize.y;

			ImVec2 result = availableSize;

			if (availableAspectRatio > aspectRatio)
			{
				// 可用宽度过大，调整宽度
				result.x = availableSize.y * aspectRatio;
			}
			else
			{
				// 可用高度过大，调整高度
				result.y = availableSize.x / aspectRatio;
			}

			return result;
		}
	};
}