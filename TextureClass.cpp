#include "TextureClass.h"

TextureClass::TextureClass():m_targaData(nullptr),m_texture(nullptr),m_textureView(nullptr)
{
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	/*bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;


	
	result = LoadTarga(filename, height, width);
	if (!result)
	{
		return false;
	}

	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	rowPitch = (width * 4) * sizeof(unsigned char);

	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}
	deviceContext->GenerateMips(m_textureView);

	delete[] m_targaData;
	m_targaData = nullptr;*/

	HRESULT result;
	DirectX::ScratchImage image = LoadTextureFromFile(filename);
	result = CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &m_textureView);

	if (FAILED(result))
		return false;

	return true;
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = nullptr;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = nullptr;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	FILE* filePtr;
	int error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	//파일 헤더를 읽는다.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//헤더에서 중요한 정보를 가져옴
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	//32비트 인지랑 24비트 아닌지 확인
	if (bpp != 32)
	{
		return false;
	}

	//32비트 이미지 사이즈를 계산한다.
	int imageSize = width * height * 4;

	//targa 이미지 데이터를 메모리에 할당한다.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa 이미지 데이터를 읽는다.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	//파일을 닫는다.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	//targa 목적 테이터를 메모리에 할당한다.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	//targa 대상 데이터 배열에 인덱스를 초기화
	int index = 0;

	//targa 이미지 데이터에 인덱스를 초기화
	int k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for(int j = 0; j<height;++j)
	{ 
		for (int i = 0; i < width; ++i)
		{
			m_targaData[index + 0] = targaImage[k + 2];//Red
			m_targaData[index + 1] = targaImage[k + 1];//Green
			m_targaData[index + 2] = targaImage[k + 0];//Blue
			m_targaData[index + 3] = targaImage[k + 3];//Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}
		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

ScratchImage TextureClass::LoadTextureFromFile(LPCSTR filename)
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


