#pragma once

struct ID3D11Buffer;

class VertexBuffer
{
	friend class Renderer;

public:
	virtual ~VertexBuffer();
	VertexBuffer(size_t size);
	VertexBuffer(VertexBuffer const& copy) = delete;

	void SetIsLinePrimitive(bool isLinePrimitive);

public:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;
	int m_stride = 0;
	bool m_isLinePrimitive = false;
};