#pragma once


struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
class Texture;

struct SurfaceData
{
public:
	ID3D11RenderTargetView* m_backBufferView = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
};
