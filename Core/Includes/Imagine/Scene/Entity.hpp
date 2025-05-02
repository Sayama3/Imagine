//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core
{

    struct Entity {
        UUID ID;
        Quat Rotation;
        Vec3 Position;
    };
}
