#include "common.h"

#include <string>
#include <sstream>
#include <cassert>
#include <chrono>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "d3d11_driver.h"

#define D3D11_CHECK(command)                                                                                                    \
    do {                                                                                                                        \
        HRESULT status = command;                                                                                               \
        if (FAILED(status)) {                                                                                                   \
            std::string info;                                                                                                   \
            info.resize(1000);                                                                                                  \
            sprintf_s(const_cast<char*>(info.data()), info.size(), "D3D11 error at " __FILE__ ":%d : %d", __LINE__, status);    \
            OutputDebugString(info.c_str());                                                                                    \
            MessageBox(NULL, info.c_str(), "OH NOOOOOOOO", MB_OK | MB_ICONERROR);                                               \
            assert(false);                                                                                                      \
        }                                                                                                                       \
    } while(0, 0)

#define SAFE_RELEASE(ptr)       \
    do {                        \
        if (ptr != nullptr) {   \
            (ptr)->Release();   \
        }                       \
        (ptr = nullptr);        \
    } while (0, 0)


D3D11Driver::D3D11Driver(HWND hWnd) : m_hWnd{ hWnd }
{
    RECT rc;
    GetWindowRect(m_hWnd, &rc);

    // create device and swapchain
    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = m_swapchain_buffer_count;
    swapDesc.BufferDesc.Width = rc.right - rc.left;
    swapDesc.BufferDesc.Height = rc.bottom - rc.top;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    {
        uint32_t create_device_flags = 0;
#ifndef NDEBUG
        create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

#if 1
        // choose best adapter by memory
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;
        D3D11_CHECK(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
        int32_t best_adapter_index = 0;
        int32_t adapter_number = 0;
        size_t best_memory = 0;
        while (true)
        {
            Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
            HRESULT enum_result = factory->EnumAdapters(adapter_number, &adapter);
            if (enum_result != S_OK) {
                // enumerating finnished
                break;
            }
            DXGI_ADAPTER_DESC adapter_desc;
            adapter->GetDesc(&adapter_desc);
            if (best_memory < adapter_desc.DedicatedVideoMemory) {
                best_adapter_index = adapter_number;
                best_memory = adapter_desc.DedicatedVideoMemory;
            }
            ++adapter_number;
        }
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        factory->EnumAdapters(best_adapter_index, &adapter);

        D3D11_CHECK(D3D11CreateDeviceAndSwapChain(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN,
            nullptr, create_device_flags,
            featureLevel, 1, D3D11_SDK_VERSION,
            &swapDesc, &m_swapchain,
            &m_device, nullptr, &m_context));
#else
        // choose default adapter
        D3D11_CHECK(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, create_device_flags,
            featureLevel, 1, D3D11_SDK_VERSION,
            &swapDesc, &m_swapchain,
            &m_device, nullptr, &m_context));
#endif
    }

    // create render target view
    D3D11_CHECK(m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backbuffer_texture));
    D3D11_CHECK(m_device->CreateRenderTargetView(m_backbuffer_texture, nullptr, &m_backbuffer_target_view));

    // create aux texture
    D3D11_TEXTURE2D_DESC rtv_tex_desc{};
    m_backbuffer_texture->GetDesc(&rtv_tex_desc); // fill Width and Height
    rtv_tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    rtv_tex_desc.MipLevels = 1;
    rtv_tex_desc.ArraySize = 1;
    rtv_tex_desc.SampleDesc.Count = 1;
    rtv_tex_desc.SampleDesc.Quality = 0;
    rtv_tex_desc.Usage = D3D11_USAGE_DEFAULT;
    rtv_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    rtv_tex_desc.CPUAccessFlags = 0;
    rtv_tex_desc.MiscFlags = 0;

    D3D11_CHECK(m_device->CreateTexture2D(&rtv_tex_desc, nullptr, &m_aux_texture));
    D3D11_CHECK(m_device->CreateRenderTargetView(m_aux_texture, nullptr, &m_aux_texture_target));
    D3D11_CHECK(m_device->CreateShaderResourceView(m_aux_texture, nullptr, &m_aux_texture_view));

    CD3D11_RASTERIZER_DESC rast_desc = {};
    rast_desc.CullMode = D3D11_CULL_NONE;
    rast_desc.FillMode = D3D11_FILL_SOLID;
    D3D11_CHECK(m_device->CreateRasterizerState(&rast_desc, &m_rasterizer_state));

    // shaders
    HRESULT status;
    ID3DBlob* error_code = nullptr;
    unsigned int compile_flags = 0;
#ifndef NDEBUG
    compile_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    // vertex shader
    {
        std::string fullscreen_triangle_vs_code{
R"(float4 VSMain( unsigned int id : SV_VertexID ) : SV_POSITION {
    return float4(4 * ((id & 2) >> 1) - 1.0, 4 * (id & 1) - 1.0, 0, 1);
}
)"
        };
        status = D3DCompile(fullscreen_triangle_vs_code.data(), fullscreen_triangle_vs_code.size(),
                            nullptr, nullptr, nullptr,
                            "VSMain", "vs_5_0",
                            compile_flags, 0,
                            &m_fullscreen_triangle_bc, &error_code);

        if (FAILED(status))
        {
            if (error_code)
            {
                std::stringstream err;
                err << (char*)(error_code->GetBufferPointer());
                MessageBox(nullptr, err.str().c_str(), "Shader compile error", MB_OK | MB_ICONERROR);
            }
            assert(false);
        }
        D3D11_CHECK(m_device->CreateVertexShader(m_fullscreen_triangle_bc->GetBufferPointer(), m_fullscreen_triangle_bc->GetBufferSize(),
                                     nullptr, &m_fullscreen_triangle));
    }

    // calc pixel shader
    {
        std::string calc_image_ps_code{
R"(cbuffer UniformData : register(b0)
{
    double time;
    float width;
    float height;
};

float4 PSMain( float4 position : SV_POSITION ) : SV_Target0
{
    double time_scale = time * 1e-4;
    double3 g_pos = float3(position.x / width, position.y / height, 0.8 * double(sin(time_scale * 0.47 + 50)) * double(cos(time_scale * 1.37 + 20)));
    double3 pos_x = g_pos + double(sin(time_scale * 1.32 + 13)) * double(cos(time_scale * 0.83 + 16));
    double3 pos_y = g_pos + double(sin(time_scale * 0.36 + 14)) * double(cos(time_scale * 1.56 + 17));
    double3 pos_z = g_pos + double(sin(time_scale * 0.57 + 15)) * double(cos(time_scale * 1.33 + 18));

    double3 color = (0).xxx;
    color.r = abs(pos_x.x * 1.3 * double(sin(time_scale * 0.89 + 1)) * double(cos(time_scale * 1.23 + 4))) + abs(pos_x.y * 0.7 * double(sin(time_scale * 1.13 + 7)) * double(cos(time_scale * 0.86 + 10))) + abs(pos_x.z * 0.32 * double(sin(time_scale * 1.43 + 13)) * double(cos(time_scale * 0.59 + 16)));
    color.g = abs(pos_y.x * 0.5 * double(sin(time_scale * 0.47 + 2)) * double(cos(time_scale * 0.97 + 5))) + abs(pos_y.y * 0.4 * double(sin(time_scale * 1.43 + 8)) * double(cos(time_scale * 0.75 + 11))) + abs(pos_y.z * 0.74 * double(sin(time_scale * 0.97 + 14)) * double(cos(time_scale * 1.82 + 17)));
    color.b = abs(pos_z.x * 0.8 * double(sin(time_scale * 1.32 + 3)) * double(cos(time_scale * 0.73 + 6))) + abs(pos_z.y * 1.5 * double(sin(time_scale * 0.93 + 9)) * double(cos(time_scale * 1.27 + 12))) + abs(pos_z.z * 1.25 * double(sin(time_scale * 0.54 + 15)) * double(cos(time_scale * 0.89 + 18)));

    return float4(pow(color, 1/2.2), 1);
}
)"
        };
        status = D3DCompile(calc_image_ps_code.data(), calc_image_ps_code.size(),
            nullptr, nullptr, nullptr,
            "PSMain", "ps_5_0",
            compile_flags, 0,
            &m_calc_image_bc, &error_code);

        if (FAILED(status))
        {
            if (error_code)
            {
                std::stringstream err;
                err << (char*)(error_code->GetBufferPointer());
                OutputDebugString(err.str().c_str());
                MessageBox(nullptr, err.str().c_str(), "Shader compile error", MB_OK | MB_ICONERROR);
            }
            assert(false);
        }
        D3D11_CHECK(m_device->CreatePixelShader(m_calc_image_bc->GetBufferPointer(), m_calc_image_bc->GetBufferSize(),
            nullptr, &m_calc_image));
    }

    // copy image shader
    {
        std::string copy_image_ps_code{
R"(Texture2D<float4> texture_to_present    : register(t0);
float4 PSMain( float4 position : SV_POSITION ) : SV_Target0
{
    return texture_to_present.Load(int3(position.xy, 0));
}
)"
        };
        status = D3DCompile(copy_image_ps_code.data(), copy_image_ps_code.size(),
            nullptr, nullptr, nullptr,
            "PSMain", "ps_5_0",
            compile_flags, 0,
            &m_copy_image_bc, &error_code);

        if (FAILED(status))
        {
            if (error_code)
            {
                std::stringstream err;
                err << (char*)(error_code->GetBufferPointer());
                MessageBox(nullptr, err.str().c_str(), "Shader compile error", MB_OK | MB_ICONERROR);
            }
            assert(false);
        }
        D3D11_CHECK(m_device->CreatePixelShader(m_copy_image_bc->GetBufferPointer(), m_copy_image_bc->GetBufferSize(),
            nullptr, &m_copy_image));
    }

    // create const buffer
    D3D11_BUFFER_DESC const_buffer_desc = {};
    const_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    const_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    const_buffer_desc.MiscFlags = 0;
    const_buffer_desc.StructureByteStride = 0;
    const_buffer_desc.ByteWidth = sizeof(m_uniform_data);

    D3D11_CHECK(m_device->CreateBuffer(&const_buffer_desc, nullptr, &m_uniform_buffer));
}

D3D11Driver::~D3D11Driver()
{
    SAFE_RELEASE(m_backbuffer_texture);
    SAFE_RELEASE(m_backbuffer_target_view);
    SAFE_RELEASE(m_aux_texture);
    SAFE_RELEASE(m_aux_texture_target);
    SAFE_RELEASE(m_aux_texture_view);
    SAFE_RELEASE(m_rasterizer_state);
    SAFE_RELEASE(m_fullscreen_triangle_bc);
    SAFE_RELEASE(m_fullscreen_triangle);
    SAFE_RELEASE(m_calc_image_bc);
    SAFE_RELEASE(m_calc_image);
    SAFE_RELEASE(m_copy_image_bc);
    SAFE_RELEASE(m_copy_image);
    SAFE_RELEASE(m_uniform_buffer);
}

void D3D11Driver::draw()
{
    m_context->ClearState();

    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(rc.right - rc.left);
    viewport.Height = static_cast<float>(rc.bottom - rc.top);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(1, &viewport);

    m_context->RSSetState(m_rasterizer_state);

    // update uniform
    //m_uniform_data.time = float(clock());
    static LONGLONG start = 0;
    static double freq = 0.0;
    if (start == 0)
    {
        LARGE_INTEGER start_int = {};
        QueryPerformanceCounter(&start_int);
        start = start_int.QuadPart;
        QueryPerformanceFrequency(&start_int);
        freq = start_int.QuadPart / 1000.0;
    }
    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);

    m_uniform_data.time = double(now.QuadPart - start) / freq;
    m_uniform_data.width = viewport.Width;
    m_uniform_data.height = viewport.Height;
    D3D11_MAPPED_SUBRESOURCE mss;
    m_context->Map(m_uniform_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mss);
    memcpy(mss.pData, &m_uniform_data, sizeof(m_uniform_data));
    m_context->Unmap(m_uniform_buffer, 0);

    m_context->PSSetConstantBuffers(0, 1, &m_uniform_buffer);

    m_context->VSSetShader(m_fullscreen_triangle, nullptr, 0);

    { // #1 step - calc image
        m_context->OMSetRenderTargets(1, &m_aux_texture_target, nullptr);
        float clear_color[4] = { 0.f, 0.f, 0.f, 1.f };
        m_context->ClearRenderTargetView(m_aux_texture_target, clear_color);

        m_context->OMSetDepthStencilState(nullptr, 0);

        m_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_context->PSSetShader(m_calc_image, nullptr, 0);
        m_context->Draw(3, 0);
    }

    { // #2 step
        m_context->OMSetRenderTargets(1, &m_backbuffer_target_view, nullptr);
        float clear_color[4] = { 0.f, 0.f, 0.f, 1.f };
        m_context->ClearRenderTargetView(m_backbuffer_target_view, clear_color);

        m_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_context->PSSetShader(m_copy_image, nullptr, 0);

        m_context->PSSetShaderResources(0, 1, &m_aux_texture_view);
        m_context->Draw(3, 0);
    }

    m_context->OMSetRenderTargets(0, nullptr, nullptr);

    D3D11_CHECK(m_swapchain->Present(1, 0));
}

void D3D11Driver::resize(uint32_t w, uint32_t h)
{
    // destroy backbuffer target view
    SAFE_RELEASE(m_backbuffer_texture);
    SAFE_RELEASE(m_backbuffer_target_view);
    SAFE_RELEASE(m_aux_texture);
    SAFE_RELEASE(m_aux_texture_target);
    SAFE_RELEASE(m_aux_texture_view);

    m_swapchain->ResizeBuffers(m_swapchain_buffer_count, UINT(w), UINT(h),
        DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    // create render target view
    D3D11_CHECK(m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backbuffer_texture));
    D3D11_CHECK(m_device->CreateRenderTargetView(m_backbuffer_texture, nullptr, &m_backbuffer_target_view));

    // recreate aux texture
    D3D11_TEXTURE2D_DESC rtv_tex_desc{};
    m_backbuffer_texture->GetDesc(&rtv_tex_desc); // fill Width and Height
    rtv_tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    rtv_tex_desc.MipLevels = 1;
    rtv_tex_desc.ArraySize = 1;
    rtv_tex_desc.SampleDesc.Count = 1;
    rtv_tex_desc.SampleDesc.Quality = 0;
    rtv_tex_desc.Usage = D3D11_USAGE_DEFAULT;
    rtv_tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    rtv_tex_desc.CPUAccessFlags = 0;
    rtv_tex_desc.MiscFlags = 0;

    D3D11_CHECK(m_device->CreateTexture2D(&rtv_tex_desc, nullptr, &m_aux_texture));
    D3D11_CHECK(m_device->CreateRenderTargetView(m_aux_texture, nullptr, &m_aux_texture_target));
    D3D11_CHECK(m_device->CreateShaderResourceView(m_aux_texture, nullptr, &m_aux_texture_view));
}
