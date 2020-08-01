#include "FontClass.h"

FontClass::FontClass():m_Font(nullptr),m_Texture(nullptr)
{
}

FontClass::FontClass(const FontClass&)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename)
{
	bool result = LoadFontData(fontFilename);
	if (!result)
		return false;

	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
		return false;

	return true;
}

void FontClass::Shutdown()
{
	ReleaseTexture();
	ReleaseFontData();

	return;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	//Coerce the input vertices into a VertexType structure.
	VertexType* vertexPtr = (VertexType*)vertices;

	//문장에 문자 갯수를 구합니다.
	int numLetters = (int)strlen(sentence);

	int index = 0;

	for (int i = 0; i < numLetters; ++i)
	{
		int letter = ((int)sentence[i]) - 32;

		//만약 문자가 공백이라면 3픽셀을 이동시킨다.
		if (letter == 0)
		{
			drawX += 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f); // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX += m_Font[letter].size + 1.0f;

		}
		
	}

	return;
}

bool FontClass::LoadFontData(char* filename)
{

	m_Font = new FontType[m_charNum];
	if (!m_Font)
		return false;

	ifstream fin;
	fin.open(filename);
	if (fin.fail())
		return false;

	//텍스트에 사용 된 95개의 아스키 문자를 읽습니다.
	char temp;
	for (int i = 0; i < m_charNum; ++i)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = nullptr;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	m_Texture = new TextureClass();
	if (!m_Texture)
		return false;

	bool result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
		return false;

	return true;
}

void FontClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

	return;
}


