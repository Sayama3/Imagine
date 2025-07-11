//
// Created by Sayama on 05/12/2024.
//

#pragma once

#include "AssetField.forward.hpp"
#include "AssetHandle.hpp"
#include "AssetType.hpp"
#include "Imagine/Core/SmartPointers.hpp"


namespace Imagine {
    template<typename T>
    class AssetField {
    public:
        AssetField();
        ~AssetField();
        explicit AssetField(AssetHandle handle);
        explicit AssetField(Ref<T> asset);
        explicit AssetField(const T &asset);
        explicit AssetField(const T *asset);

    public:
        [[nodiscard]] bool HasHandle() const;
        [[nodiscard]] Ref<T> GetAsset() const;
        [[nodiscard]] AssetHandle GetHandle() const;
        void SetHandle(AssetHandle handle);
        void Reset();

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] explicit operator Ref<T>() const;
        [[nodiscard]] explicit operator AssetHandle() const;

        [[nodiscard]] explicit operator bool() const;
        [[nodiscard]] static AssetType GetStaticType();
        [[nodiscard]] AssetType GetType() const;

        bool operator==(AssetField rhs) const;
        bool operator!=(AssetField rhs) const;
    private:
        AssetHandle Handle = NULL_ASSET_HANDLE;
    };
}
