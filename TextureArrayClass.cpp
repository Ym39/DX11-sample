#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass()
{
	m_textures[0] = nullptr;
	m_textures[1] = nullptr;
	m_textures[2] = nullptr;
}

TextureArrayClass::TextureArrayClass(const TextureArrayClass&)
{
}

TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::Initialize(ID3D11Device* device, char* filename1, char* filename2)
{
	HRESULT result;

	DirectX::ScratchImage image1 = LoadTextureFromFile(filename1);
	result = CreateShaderResourceView(device, image1.GetImages(), image1.GetImageCount(), image1.GetMetadata(), &m_textures[0]);
	if (FAILED(result))
		return false;

	DirectX::ScratchImage image2 = LoadTextureFromFile(filename2);
	result = CreateShaderResourceView(device, image2.GetImages(), image2.GetImageCount(), image2.GetMetadata(), &m_textures[1]);
	if (FAILED(result))
		return false;

	return true;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, char* filename1, char* filename2, char* filename3)
{
	HRESULT result;

	DirectX::ScratchImage image1 = LoadTextureFromFile(filename1);
	result = CreateShaderResourceView(device, image1.GetImages(), image1.GetImageCount(), image1.GetMetadata(), &m_textures[0]);
	if (FAILED(result))
		return false;

	DirectX::ScratchImage image2 = LoadTextureFromFile(filename2);
	result = CreateShaderResourceView(device, image2.GetImages(), image2.GetImageCount(), image2.GetMetadata(), &m_textures[1]);
	if (FAILED(result))
		return false;

	DirectX::ScratchImage image3 = LoadTextureFromFile(filename3);
	result = CreateShaderResourceView(device, image3.GetImages(), image3.GetImageCount(), image3.GetMetadata(), &m_textures[2]);
	if (FAILED(result))
		return false;

	return true;
}

void TextureArrayClass::Shutdown()
{

	if (m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if (m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	if (m_textures[2])
	{
		m_textures[2]->Release();
		m_textures[2] = 0;
	}

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}

ScratchImage TextureArrayClass::LoadTextureFromFile(LPCSTR filename)
{
	// Load the texture.
	string str(filename);
	wstring wsTmp(str.begin(), str.end());

	wstring ws = wsTmp;
	// Load the texture.
	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(ws.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	HRESULT hr;
	ScratchImage image;
	if (_wcsicmp(ext, L".dds") == 0)
	{
		hr = LoadFromDDSFile(ws.c_str(), DDS_FLAGS_NONE, nullptr, image);
	}

	else if (_wcsicmp(ext, L".tga") == 0)
	{
		hr = LoadFromTGAFile(ws.c_str(), nullptr, image);
	}

	else if (_wcsicmp(ext, L".wic") == 0)
	{
		hr = LoadFromWICFile(ws.c_str(), WIC_FLAGS_NONE, nullptr, image);
	}

	return image;
}
