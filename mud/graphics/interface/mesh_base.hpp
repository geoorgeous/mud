#ifndef MESH_BASE_HPP
#define MESH_BASE_HPP

#include <string>
#include <vector>

#include "math/aabb.hpp"
#include "math/vector.hpp"
#include "utils/asset_object.hpp"

namespace mud
{
    struct MeshVertex
    {
        Vector3 position;
        Vector3 normal;
        Vector4 colour;
        Vector2 textureCoordinates;
    };

    class MeshBase : public AssetObject<MeshBase>
    {
    public:

        MeshBase();

        MeshBase(const std::vector<MeshVertex> & vertices, const std::vector<uint32_t> & indices = {});

        const std::vector<MeshVertex> & getVertices() const;

        const std::vector<uint32_t> & getIndices() const;

        const AABB & getBoundingBox() const;

        void setData(const std::vector<MeshVertex> & vertices, const std::vector<uint32_t> & indices = {});

        virtual bool deserialize(std::ifstream & file) override;

        virtual bool serialize(std::ofstream & file) const override;

    protected:

        std::vector<MeshVertex> m_vertices;

        std::vector<uint32_t> m_indices;

        AABB m_boundingBox;

        virtual void onSetData() = 0;

        void recalculateBoundingBox();
    };

    template<>
    inline AssetObject<MeshBase>::AssetObject()
        : AssetObjectBase(AssetObjectType::Mesh)
    { }
}

#endif