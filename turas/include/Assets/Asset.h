//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "lvk/Mesh.h"
#include "Rendering/VertexLayouts.h"

namespace turas {
    enum class AssetType : uint32_t
    {
        Mesh,
        Model,
        Texture,
        Audio,
        Text,
        Binary
    };

    union AssetHandle {
        uint64_t _Data;
        struct {
            uint32_t    m_Hash;
            AssetType   m_Type;
        };

        AssetHandle(const uint32_t& hash, const AssetType& type) : m_Hash(hash), m_Type(type)
        {

        }

        bool operator==(const AssetHandle &o) const {
            return _Data == o._Data;
        }

        bool operator<(const AssetHandle &o) const {
            return _Data < o._Data;
        }
    };

    class Asset {
    public:
        Asset(const String &path, const AssetHandle &handle) : m_Path(path), m_Handle(handle)
        {

        }
        const String        m_Path;
        const AssetHandle   m_Handle;
    };

    class MeshAsset : public Asset {
    public:
        lvk::Mesh       m_GPUMesh;
        VertexLayout    m_VertexLayout;

    };

    class ModelAsset : public Asset {
    public:
        ModelAsset(const String &path, const AssetHandle &handle) : Asset(path, handle) {

        }
    };

    class TextureAsset : public Asset {};

    class AudioAsset : public Asset {};

    class TextAsset : public Asset {};

    class BinaryAsset : public Asset {};


}

/* required to hash a container */
template<> struct std::hash<turas::AssetHandle> {
    std::size_t operator()(const turas::AssetHandle& ah) const {
        return std::hash<uint64_t>()(ah._Data);
    }
};
