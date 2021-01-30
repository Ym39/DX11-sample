#pragma once
#include<Windows.h>
#include "d3dclass.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class ImGuiClass
{
public:
	ImGuiClass();
	~ImGuiClass();

	void Initialize(HWND hwnd,ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Render();

private:
	fs::path mCurrentPath;
	std::vector<const char*> mCurrentDirectoryFiles;
	std::vector<std::string> list;

	int mCurrentItemNumber;
	bool mIsLatest;
};

