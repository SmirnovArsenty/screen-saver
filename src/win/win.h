#pragma once

#include "Windows.h"
#include "driver/D3D11/d3d11_driver.h"

class win {
private:
	win();

	HINSTANCE m_hInstance{ nullptr };
	HWND m_hWnd{ nullptr };
	D3D11Driver* m_d3d11{ nullptr };

	bool m_is_closed{ false };
public:
	~win();

	void set_instance(HINSTANCE hInstance);
	HINSTANCE instance();

	void init(HWND hWnd);
	void deinit();

	void draw(HDC hDC);
	void resize(uint32_t w, uint32_t h);

	bool is_closed();

	static win g_win;
};
