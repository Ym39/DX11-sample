////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass() :m_Light(nullptr), m_TextureShader(nullptr), m_Texture(nullptr), m_Bitmap(nullptr), m_Text(nullptr),m_ModelList(nullptr),m_Frustum(nullptr),m_MultiTextureShader(nullptr),m_LightMapShader(nullptr),m_AlphaMapShader(nullptr),m_BumpMapShader(nullptr),m_SpecMapShader(nullptr),m_DebugWindow(nullptr),m_RenderTexture(nullptr),m_FogShader(nullptr),m_ClipPlaneShader(nullptr),m_TranslateShader(nullptr),m_TransparentShader(nullptr)
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);


	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_Direct3D->GetDevice(),m_Direct3D->GetDeviceContext(),"data/dirt01.dds","data/square.txt");
	/*result = m_Model->Initialize(m_Direct3D->GetDevice(), "data/square.txt", "data/stone01.dds", "data/dirt01.dds");*/
	/*result = m_Model->Initialize(m_Direct3D->GetDevice(), "data/square.txt", "data/stone01.dds", "data/light01.dds");*/
	//result = m_Model->Initialize(m_Direct3D->GetDevice(), "data/square.txt", "data/stone01.dds", "data/dirt01.dds", "data/alpha01.dds");
	/*result = m_Model->Initialize_Bump(m_Direct3D->GetDevice(), "data/cube.txt", "data/stone01.dds", "data/bump01.dds");*/
	//result = m_Model->Initialize_Spec(m_Direct3D->GetDevice(), "data/cube.txt", "data/stone02.dds", "data/bump02.dds", "data/spec02.dds");
	//m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/seafloor.dds", "data/triangle.txt");

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_Model2 = new ModelClass;
	if (!m_Model2)
	{
		return false;
	}

	result = m_Model2->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/stone01.dds", "data/square.txt");
	 

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_FbxModel = new FbxModelClass;
	if (!m_FbxModel)
	{
		return false;
	}

	result = m_FbxModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/character.fbx", "data/CharacterTexture.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the FBX model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	
	m_Light->SetAmbientColor(0.1f, 0.1f, 0.1f,1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetPosition(-5.0f, 0.0f, -200.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(50.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	

	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;

	result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, "stone01.tga", 300, 300);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
		return false;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	m_Text = new TextClass();
	if (!m_Text)
		return false;

	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	m_ModelList = new ModelListClass();
	if (!m_ModelList)
		return false;

	result = m_ModelList->Initiailze(25);
	if (!result)
		return false;

	m_Frustum = new FrustumClass();
	if (!m_Frustum)
		return false;

	m_MultiTextureShader = new MultiTextureShader;
	if (!m_MultiTextureShader)
		return false;

	result = m_MultiTextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK);
		return false;
	}

	m_LightMapShader = new LightMapShader;
	if (!m_LightMapShader)
		return false;

	result = m_LightMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the lightmap shader object.", L"Error", MB_OK);
		return false;
	}

	m_AlphaMapShader = new AlphaMapShaderClass;
	if (!m_AlphaMapShader)
		return false;

	result = m_AlphaMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Alphamap shader object.", L"Error", MB_OK);
		return false;
	}

	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader)
		return false;

	result = m_BumpMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bumpamap shader object.", L"Error", MB_OK);
		return false;
	}

	m_SpecMapShader = new SpecMapShaderClass;
	if (!m_SpecMapShader)
		return false;

	result = m_SpecMapShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the specmap shader object.", L"Error", MB_OK);
		return false;
	}

	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
		return false;

	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow)
		return false;

	result = m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the  debug window object.", L"Error", MB_OK);
		return false;
	}

	m_FogShader = new FogShaderClass;
	if (!m_FogShader)
		return false;

	result=m_FogShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the  fog shader object.", L"Error", MB_OK);
		return false;
	}

	m_ClipPlaneShader = new ClipPlaneShaderClass;
	if (!m_ClipPlaneShader)
		return false;

	result = m_ClipPlaneShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the clip plane shader object.", L"Error", MB_OK);
		return false;
	}

	m_TranslateShader = new TranslateShaderClass;
	if (!m_TranslateShader)
		return false;

	result = m_TranslateShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the translate shader object.", L"Error", MB_OK);
		return false;
	}

	m_TransparentShader = new TransparentShaderClass;
	if (!m_TransparentShader)
		return false;

	result = m_TransparentShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}

	if (m_TextureShader)
	{
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = nullptr;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = nullptr;
	}

	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = nullptr;
	}

	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = nullptr;
	}

	if (m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = nullptr;
	}

	if (m_LightMapShader)
	{
		m_LightMapShader->Shutdown();
		delete m_LightMapShader;
		m_LightMapShader = nullptr;
	}

	if (m_AlphaMapShader)
	{
		m_AlphaMapShader->Shutdown();
		delete m_AlphaMapShader;
		m_AlphaMapShader = nullptr;
	}

	if (m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = nullptr;
	}

	if (m_SpecMapShader)
	{
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = nullptr;
	}

	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = nullptr;
	}

	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = nullptr;
	}

	if (m_FogShader)
	{
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = nullptr;
	}

	if (m_ClipPlaneShader)
	{
		m_ClipPlaneShader->Shutdown();
		delete m_ClipPlaneShader;
		m_ClipPlaneShader = 0;
	}

	if (m_TranslateShader)
	{
		m_TranslateShader->Shutdown();
		delete m_TranslateShader;
		m_TranslateShader = 0;
	}

	if (m_TransparentShader)
	{
		m_TransparentShader->Shutdown();
		delete m_TransparentShader;
		m_TransparentShader = 0;
	}

	if (m_FbxModel)
	{
		m_FbxModel->Shutdown();
		delete m_FbxModel;
		m_FbxModel = nullptr;
	}

	return;
}


bool GraphicsClass::Frame(float rotationY,float time)
{
	bool result;
	static float rotation = 0.0f;

	rotation += 0.001f * time;

	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -500.0f);
	//m_Camera->SetRotation(0.0f,rotationY,0.0f);

	/*result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;

	result = m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext());
	if (!result)
		return false;*/

	// Render the graphics scene.
	result = Render(rotation,time);
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(float rotation ,float time)
{
	XMMATRIX worldMatrix,worldMatrix_2D, viewMatrix, projectionMatrix,orthoMatrix;
	float positionX, positionY, positionZ, radius;
	radius = 1.0f;
	XMFLOAT4 color;
	float fogColor, fogStart, fogEnd;
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	bool result;
	float blendAmount = 0.5f;

	//result = RenderToTexture(rotation);
	//if (!result)
	//	return false;

	fogColor = 0.0f;
	fogStart = 0.0f;
	fogEnd = 10.0f;
	

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	/*result = RenderScene(rotation);
	if (!result)
		return false;*/

	//멀티 텍스쳐 그리기
    
	/*m_Model->Render(m_Direct3D->GetDeviceContext());

	m_MultiTextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray());*/


	//라이트맵 그리기

	/*m_Model->Render(m_Direct3D->GetDeviceContext());

	m_LightMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray());*/

	//알파맵 그리기
	/*m_Model->Render(m_Direct3D->GetDeviceContext());
	m_AlphaMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray());*/

	//범프맵 그리기
	/*worldMatrix = XMMatrixRotationY(rotation);
	m_Model->Render(m_Direct3D->GetDeviceContext());
	m_BumpMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());*/

	//반사맵 그리기
	//worldMatrix = XMMatrixRotationY(rotation);
	/*m_Model->Render(m_Direct3D->GetDeviceContext());
	m_SpecMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor(),m_Camera->GetPosition(),m_Light->GetSpecularColor(),m_Light->GetSpecularPower());*/

	//안개 그리기
	/*m_Model->Render(m_Direct3D->GetDeviceContext());
	m_FogShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), fogStart, fogEnd);*/

	//클리핑 평면 그리기
	/*worldMatrix = XMMatrixRotationY(rotation);
	m_Model->Render(m_Direct3D->GetDeviceContext());
	m_ClipPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), clipPlane);*/

	//텍스쳐 이동
	static float textureTranslation = 0.0f;
	
	/*textureTranslation += 0.01f * time * 0.1f;
	m_Model->Render(m_Direct3D->GetDeviceContext());
	m_TranslateShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), textureTranslation);*/
	
	//그냥 평면 그리기
	//m_Model->Render(m_Direct3D->GetDeviceContext());
	//m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

	/*worldMatrix = worldMatrix * XMMatrixTranslation(1.0f, 0.0f, -1.0f);
	m_Model2->Render(m_Direct3D->GetDeviceContext());
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), m_Light->GetPosition(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());*/

	/*m_Model2->Render(m_Direct3D->GetDeviceContext());
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture());
	if (!result)
		return false;*/


    //FBX 모델 그리기
	//worldMatrix = worldMatrix * XMMatrixRotationRollPitchYaw(0.0f, 90.0f, 0.0f);
	//worldMatrix = worldMatrix * XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 90.0f);
	m_FbxModel->Render(m_Direct3D->GetDeviceContext());
	m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FbxModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_FbxModel->GetTexture());

	//프러스텀 컬링으로 그리기
	/*m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	int modelCount = m_ModelList->GetModelCount();

	int renderCount = 0;

	for (int index = 0; index < modelCount; index++)
	{
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		if (m_Frustum->CheckCube(positionX, positionY, positionZ, radius))
		{
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);
			
			m_Model->Render(m_Direct3D->GetDeviceContext());

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetPosition(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

			m_Direct3D->GetWorldMatrix(worldMatrix);

			renderCount++;
		}
	}*/


	///렌더 카운터 숫자 설정
	/*result = m_Text->SetRanderCount(renderCount, m_Direct3D->GetDeviceContext());

	if (!result)
	{
		return false;
	}*/

	//// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//m_Model->Render(m_Direct3D->GetDeviceContext());

	//// Render the model using the color shader.
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetPosition(), m_Light->GetDiffuseColor(),m_Light->GetAmbientColor(),
	//	m_Camera->GetPosition(),m_Light->GetSpecularColor(),m_Light->GetSpecularPower());
	//if (!result)
	//{
	//	return false;
	//}

	//이 밑에서 2D 렌더링

	m_Direct3D->TurnZBufferOff();

	m_Direct3D->TurnOnAlphaBlending();

	/*result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
	if (!result)
		return false;

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix_2D, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());*/

	// Render the text strings.
	/*result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}*/

	//디버그 위도우의 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 넣음
	/*result = m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50);
	if (!result)
		return false;

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView());
	if (!result)
		return false;
*/

	//투명한 평면 그리기
	/*worldMatrix = worldMatrix * XMMatrixTranslation(1.0f, 0.0f, -1.0f);
	m_Model2->Render(m_Direct3D->GetDeviceContext());
	result = m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(),blendAmount);
	if (!result)
		return false;

	worldMatrix = worldMatrix * XMMatrixTranslation(1.0f, 0.0f, -1.0f);
	m_Model2->Render(m_Direct3D->GetDeviceContext());
	result = m_TransparentShader->Render(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), blendAmount);
	if (!result)
		return false;*/


	m_Direct3D->TurnOffAlphaBlending();

	m_Direct3D->TurnZBufferOn();

	//Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture(float rotation)
{
	bool result;
	 
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	result = RenderScene(rotation);
	if (!result)
		return false;

	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderScene(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix = worldMatrix * XMMatrixRotationY(rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());
	result = m_SpecMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

	if (!result)
		return false;

	return true;
}
