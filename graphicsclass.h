////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "LightClass.h"
#include "Bitmapclass.h"
#include "TextureShaderClass.h"
#include "TextureClass.h"
#include"TextClass.h"
#include"FrustumClass.h"
#include"ModelListClass.h"
#include"MultiTextureShader.h"
#include"LightMapShader.h"
#include"AlphaMapShaderClass.h"
#include"BumpMapShaderClass.h"
#include"SpecMapShaderClass.h"
#include"DebugWindowClass.h"
#include"RenderTextureClass.h"
#include"FogShaderClass.h"
#include"ClipPlaneShaderClass.h"
#include"TranslateShaderClass.h"
#include"TransparentShaderClass.h"
#include"FbxModelClass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float,float,float,float);
	bool Render(float rotation,float);
	bool RenderToTexture(float rotation);
	bool RenderScene(float rotation);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ModelClass* m_Model2;
	ColorShaderClass* m_ColorShader;
	LightClass* m_Light;
	TextureShaderClass* m_TextureShader;
	TextureClass* m_Texture;
	BitmapClass* m_Bitmap;
	TextClass* m_Text;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
	MultiTextureShader* m_MultiTextureShader;
	LightMapShader* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	SpecMapShaderClass* m_SpecMapShader;
	DebugWindowClass* m_DebugWindow;
	RenderTextureClass* m_RenderTexture;
	FogShaderClass* m_FogShader;
	ClipPlaneShaderClass* m_ClipPlaneShader;
	TranslateShaderClass* m_TranslateShader;
	TransparentShaderClass* m_TransparentShader;
	FbxModelClass* m_FbxModel;
};

#endif