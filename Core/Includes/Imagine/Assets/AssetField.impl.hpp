//
// Created by Sayama on 05/12/2024.
//

#pragma once

#include "AssetField.decl.hpp"
#include "AssetManager.hpp"
#include <spdlog/spdlog.h>


namespace Imagine::Core
{
    template<typename T>
    AssetField<T>::AssetField() = default;

    template<typename T>
    AssetField<T>::~AssetField() = default;

    template<typename T>
    AssetField<T>::AssetField(AssetHandle handle): Handle(handle) {}

    template<typename T>
    AssetField<T>::AssetField(Ref<T> asset): Handle(asset->Handle) {}

    template<typename T>
    AssetField<T>::AssetField(const T &asset): Handle(asset.Handle) {}

    template<typename T>
    AssetField<T>::AssetField(const T *asset): Handle(asset->Handle) {}

    template<typename T>
    bool AssetField<T>::HasHandle() const {
        return Handle != NULL_ASSET_HANDLE;
    }

    template<typename T>
    Ref<T> AssetField<T>::GetAsset() const {
        MGN_CORE_CASSERT(AssetManager::IsAssetHandleValid(Handle));
        return AssetManager::GetAssetAs<T>(Handle);
    }

    template<typename T>
    AssetHandle AssetField<T>::GetHandle() const {
        return Handle;
    }

    template<typename T>
    void AssetField<T>::SetHandle(AssetHandle handle) {
        Handle = handle;
    }

    template<typename T>
    void AssetField<T>::Reset() {
        Handle = NULL_ASSET_HANDLE;
    }

    template<typename T>
    bool AssetField<T>::IsValid() const {
        return HasHandle() && AssetManager::IsAssetHandleValid(Handle);
    }

    template<typename T>
    AssetField<T>::operator Ref<T>() const {
        return GetAsset();
    }

    template<typename T>
    AssetField<T>::operator AssetHandle() const {
        return GetHandle();
    }

    template<typename T>
    AssetField<T>::operator bool() const {
        return IsValid();
    }

    template<typename T>
    AssetType AssetField<T>::GetStaticType() {return T::GetStaticType(); }

    template<typename T>
    AssetType AssetField<T>::GetType() const {return GetStaticType(); }

    template<typename T>
    bool AssetField<T>::operator==(AssetField rhs) const {
        return rhs.Handle == Handle;
    }

    template<typename T>
    bool AssetField<T>::operator!=(AssetField rhs) const {
        return !(rhs.Handle == Handle);
    }
}


template<typename T>
struct std::hash<Imagine::Core::AssetField<T>> {
    std::size_t operator()(const Imagine::Core::AssetField<T> &s) const noexcept {
        return std::hash<Imagine::Core::AssetHandle>{}(s.GetHandle());
    }
};

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const Imagine::Core::AssetField<T> &value) {
    return os << value.GetHandle().string();
}

template<typename T>
struct fmt::formatter<Imagine::Core::AssetField<T>> : fmt::formatter<std::string>
{
    auto format(Imagine::Core::AssetField<T> value, format_context &ctx) const -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}", value.GetHandle().string());
    }
};
