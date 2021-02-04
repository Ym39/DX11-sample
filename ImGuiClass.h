#pragma once
#include<Windows.h>
#include "d3dclass.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"
#include <string>
#include <filesystem>
#include <stack>
namespace fs = std::filesystem;

class ImGuiClass
{
public:
	ImGuiClass();
	~ImGuiClass();

	void Initialize(HWND hwnd,ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Render();

private:
    fs::path mRootPath;
	fs::path mCurrentPath;
	std::vector<std::string> mCurrentDirectoryFileNames;
	std::vector<fs::path> mPopup;

    std::string mSelectFilePath;
	const char* mSeletFilePathCstr;

	int mCurrentItemNumber;
	bool mIsLatest;
};

