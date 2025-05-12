//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core
{
    struct EntityID {
        uint32_t id;
        [[nodiscard]] UUID AsUUID() const {return UUID(id);}
    };

    struct Entity {
        EntityID ID;
        Quat Rotation;
        Vec3 Position;
    };
}
