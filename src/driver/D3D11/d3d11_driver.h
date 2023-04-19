#pragma once

#include <Windows.h>
#include <cstdint>
#include <wrl.h>
#include <d3d11.h>

using namespace Microsoft::WRL;

class D3D11Driver {
private:
	bool m_error_flag{ false };

	HWND m_hWnd;

	constexpr static uint32_t m_swapchain_buffer_count{ 2 };

	ComPtr<ID3D11Device> m_device{ nullptr };
	ComPtr<ID3D11DeviceContext> m_context{ nullptr };
	ComPtr<IDXGISwapChain> m_swapchain{ nullptr };

	ID3D11Texture2D* m_backbuffer_texture{ nullptr };
	ID3D11RenderTargetView* m_backbuffer_target_view{ nullptr };

	ID3D11Texture2D* m_aux_texture{ nullptr };
	ID3D11RenderTargetView* m_aux_texture_target{ nullptr };
	ID3D11ShaderResourceView* m_aux_texture_view{ nullptr };

	ID3D11RasterizerState* m_rasterizer_state{ nullptr };

	ID3D11VertexShader* m_fullscreen_triangle{ nullptr };
	ID3DBlob* m_fullscreen_triangle_bc{ nullptr };
	ID3D11PixelShader* m_calc_image{ nullptr };
	ID3DBlob* m_calc_image_bc{ nullptr };
	ID3D11PixelShader* m_copy_image{ nullptr };
	ID3DBlob* m_copy_image_bc{ nullptr };

	struct
	{
		double time;
		float width;
		float height;
	} m_uniform_data;
	ID3D11Buffer* m_uniform_buffer{ nullptr };

public:
	D3D11Driver(HWND);
	~D3D11Driver();

	void clear();
	void resize(uint32_t w, uint32_t h);
	void draw();

	uint32_t width();
	uint32_t height();

	[[nodiscard]] bool is_error();
};
