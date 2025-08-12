#pragma once
#include <imgui.h>
#include "CHFONT.h"
#include <map>
#include <string>
class ImApplication
{
protected:
	static int DefalutFontSize;
	static std::string DefaultFont;
	static std::map<int, ImFont*> DefalutFonts;
protected:
	static ImFont* LoadDefaultFontInternal(int Size);
public:
	void Render();
	static ImFont* GetFont(int Size, const std::string& FontFile = "")
	{
		if (!(Size >= 1 && Size <= 80))
			return nullptr;
		if (FontFile.empty())
		{
			auto it = DefalutFonts.find(Size);
			if (it != DefalutFonts.end())
			{
				return it->second;
			}
			else
			{
				DefalutFonts.insert(std::make_pair(Size, LoadDefaultFontInternal(Size)));
				return GetFont(Size);
			}
		}
	}
	virtual ImTextureID LoadTextureFromFile(const char* filename, int& width, int& height) = 0;
	virtual void ReleaseTexture(ImTextureID TextureID) = 0;

	
};