#include "ImGuiClass.h"

namespace ImGui
{
	bool Getter(void* list, int count, const char** outText)
	{
		std::vector<std::string>& vector = *static_cast<std::vector<std::string>*>(list);
		if (count < 0 || count > vector.size())
		{
			return false;
		}
		*outText = vector[count].c_str();

		return true;
	}
}

ImGuiClass::ImGuiClass()
{
}

ImGuiClass::~ImGuiClass()
{
}

void ImGuiClass::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGui::StyleColorsDark();

	mIsLatest = false;
	mRootPath = fs::current_path();
	mCurrentPath = mRootPath;
	mCurrentItemNumber = -1;

	mCurrentDirectoryFileNames.reserve(200);
	mPopup.reserve(10);

	mSelectFilePath = "";
}

void ImGuiClass::Render()
{
	/*static std::filesystem::path currentPath = std::filesystem::current_path();
	static std::vector<const char*> directoryList;
	static int currentItem = 0;
	for (const auto entry : std::filesystem::directory_iterator(currentPath))
	{
		directoryList.push_back(entry.path().string().c_str());
	}

	char* listbox_Items = new char[directoryList.size()];
	for (int i = 0; i < directoryList.size(); i++)
	{
		listbox_Items[i] = reinterpret_cast<char>(directoryList[i]);
	}*/
	if (mIsLatest == false)
	{
	    mCurrentDirectoryFileNames.clear();
		for (const auto entry : std::filesystem::directory_iterator(mCurrentPath))
		{
			mCurrentDirectoryFileNames.push_back(entry.path().filename().string());
		}
		mIsLatest = true;
	}

	//Imgui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	float my_color[4] = { 0.0f, };

	bool my_tool_active = true;
	// Create a window called "My First Tool", with a menu bar.
	ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Edit a color (stored as ~4 floats)
	ImGui::ColorEdit4("Color", my_color);

	// Plot some values
	const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
	ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

	// Display contents in a scrolling region
	/*ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
	ImGui::BeginChild("Scrolling");
	for (int n = 0; n < 50; n++)
		ImGui::Text("%04d: Some text", n);

	ImGui::EndChild();*/
	//ImGui::ListBox("Directory", &currentItem, reinterpret_cast<const char* const*>(listbox_Items), directoryList.size(), 6);


	ImGui::End();

	ImGui::Begin("File");
	ImGui::Text((mCurrentPath.string()).c_str());
	if (ImGui::Button("Root"))
	{
	   mCurrentPath = mRootPath;
	   mPopup.clear();
	   mIsLatest = false;
	}

	
		for (int i = 0; i < mPopup.size(); ++i)
		{
			std::string directoryName = mPopup[i].filename().string();
			ImGui::SameLine();
			if (ImGui::Button(directoryName.c_str()))
			{
			    mCurrentPath = mPopup[i];
				mPopup.erase(mPopup.begin()+i+1,mPopup.end());
				mIsLatest = false;
			}
	    }
	

	ImGui::NewLine();
	ImGui::PushItemWidth(-1);
	bool select = ImGui::ListBox("##listbox2", &mCurrentItemNumber, ImGui::Getter, static_cast<void*>(&mCurrentDirectoryFileNames), (int)mCurrentDirectoryFileNames.size(),16);
	if(select == true)
	{ 
	   fs::path selectPath = mCurrentPath / mCurrentDirectoryFileNames[mCurrentItemNumber];
	   if (fs::directory_entry(selectPath).is_directory())
	   {
	      mCurrentPath = selectPath;
		  mPopup.push_back(selectPath);
		  mIsLatest = false;
	   }
	   else
	   {
		   mSelectFilePath = selectPath.string();
		   mSeletFilePathCstr = mSelectFilePath.c_str();

	   }
	}

	ImGui::Text(mSelectFilePath.c_str());
	ImGui::End();


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


