#pragma once
#include <imgui.h>

class ImApplication
{
public:
	virtual ImTextureID LoadTextureFromFile(const char* filename, int& width, int& height) = 0;
	virtual void ReleaseTexture(ImTextureID TextureID) = 0;
};