//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "STL/Memory.h"
#include "STL/Vector.h"
#include "Rendering/VertexLayouts.h"
#include "Rendering/Mesh.h"

namespace turas {
    enum class AssetType : u8
    {
        Model,
        Texture,
        Audio,
        Text,
        Binary
    };

    union AssetHandle {
        u64 _DataA;
        u64 _DataB;

        struct {
            u64    m_Hash;
            AssetType   m_Type;
        };

        AssetHandle(const u64& hash, const AssetType& type) : m_Hash(hash), m_Type(type)
        {

        }

        bool operator==(const AssetHandle &o) const {
            return _DataA == o._DataA && _DataB == o._DataB;
        }

        bool operator<(const AssetHandle &o) const {
            return _DataA < o._DataA && _DataB < o._DataB;
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

    class ModelAsset : public Asset {
    public:

        ModelAsset(const String &path, const AssetHandle &handle) : Asset(path, handle) {

        }

        TURAS_IMPL_ALLOC(ModelAsset)
        Vector<CPUMesh> m_MeshData;
    };

    class TextureAsset : public Asset {
    public:
        TURAS_IMPL_ALLOC(TextureAsset)
    };

    class AudioAsset : public Asset {
    public:
        TURAS_IMPL_ALLOC(AudioAsset)
    };

    class TextAsset : public Asset {
    public:
        TURAS_IMPL_ALLOC(TextAsset)
    };

    class BinaryAsset : public Asset {
    public:
        TURAS_IMPL_ALLOC(BinaryAsset)
    };
}

/* required to hash a container */
template<> struct std::hash<turas::AssetHandle> {
    std::size_t operator()(const turas::AssetHandle& ah) const {
        return std::hash<turas::u64>()(ah._DataA) ^ std::hash<turas::u64>()(ah._DataB);
    }
};
