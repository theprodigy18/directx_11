#pragma once

#include "drawable.hpp"
#include <random>

namespace drop::drawable
{
    class Box : public Drawable
    {
    public:
        Box(graphics::Graphics& gfx, std::mt19937& rng,
            std::uniform_real_distribution<f32>& adist,
            std::uniform_real_distribution<f32>& ddist,
            std::uniform_real_distribution<f32>& odist,
            std::uniform_real_distribution<f32>& rdist);

        void              Update(f32 dt) noexcept override;
        DirectX::XMMATRIX GetTransformXM() const noexcept override;

    private:
        // Position.
        f32 _r;
        f32 _roll {0.0f};
        f32 _pitch {0.0f};
        f32 _yaw {0.0f};
        f32 _theta {0.0f};
        f32 _phi {0.0f};
        f32 _chi {0.0f};
        // Speed/delta.
        f32 _droll;
        f32 _dpitch;
        f32 _dyaw;
        f32 _dtheta;
        f32 _dphi;
        f32 _dchi;
    };
} // namespace drop::drawable