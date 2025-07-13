#include "drawable/box.hpp"

#include "bindable/bindable_base.hpp" // IWYU pragma: keep

namespace drop::drawable
{
    Box::Box(graphics::Graphics& gfx, std::mt19937& rng,
             std::uniform_real_distribution<f32>& adist,
             std::uniform_real_distribution<f32>& ddist,
             std::uniform_real_distribution<f32>& odist,
             std::uniform_real_distribution<f32>& rdist)
        : _r {rdist(rng)},
          _droll {ddist(rng)},
          _dpitch {ddist(rng)},
          _dyaw {ddist(rng)},
          _dtheta {odist(rng)},
          _dphi {odist(rng)},
          _dchi {odist(rng)},
          _chi {adist(rng)},
          _phi {adist(rng)},
          _theta {adist(rng)}
    {
        struct Vertex
        {
            struct
            {
                f32 x {0.0f};
                f32 y {0.0f};
                f32 z {0.0f};
            } pos;
        };

        const std::vector<Vertex> vertices {
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
        };

        AddBind(std::make_unique<bindable::VertexBuffer>(gfx, vertices));

        auto pvs {std::make_unique<bindable::VertexShader>(gfx, L"basic_shader_vs.cso")};
        auto pvsbc {pvs->GetBlob()};
        AddBind(std::move(pvs));

        AddBind(std::make_unique<bindable::PixelShader>(gfx, L"basic_shader_ps.cso"));

        const std::vector<u16> indices {
            0, 2, 1, 2, 3, 1,
            1, 3, 5, 3, 7, 5,
            2, 6, 3, 3, 6, 7,
            4, 5, 7, 4, 7, 6,
            0, 4, 2, 2, 4, 6,
            0, 1, 4, 1, 5, 4};

        AddIndexBuffer(std::make_unique<bindable::IndexBuffer>(gfx, indices));

        struct ConstantBuffer2
        {
            struct
            {
                f32 r;
                f32 g;
                f32 b;
                f32 a;
            } face_colors[6];
        };

        const ConstantBuffer2 cb2 {
            {
                {1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 1.0f, 0.0f},
                {0.0f, 1.0f, 1.0f},
            }};

        AddBind(std::make_unique<bindable::PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied {
            {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        AddBind(std::make_unique<bindable::InputLayout>(gfx, ied, pvsbc));

        AddBind(std::make_unique<bindable::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

        AddBind(std::make_unique<bindable::TransformCbuffer>(gfx, *this));
    }

    void Box::Update(f32 dt) noexcept
    {
        _roll += _droll * dt;
        _pitch += _dpitch * dt;
        _yaw += _dyaw * dt;
        _theta += _dtheta * dt;
        _phi += _dphi * dt;
        _chi += _dchi * dt;
    }

    DirectX::XMMATRIX Box::GetTransformXM() const noexcept
    {
        return DirectX::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
               DirectX::XMMatrixTranslation(_r, 0.0f, 0.0f) *
               DirectX::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *
               DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
    }

} // namespace drop::drawable