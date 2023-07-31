#include "scene.hpp"

#include "graphics/camera.hpp"
#include "math/intersection_test.hpp"

namespace mud
{
    void renderSceneGraphNode(const SceneGraph::Node & sceneGraphNode, ForwardRenderer & renderer, const Matrix4 & parentTransform = Matrix4::identity)
    {
        const Matrix4 transform = parentTransform * sceneGraphNode.data.transform;

        bool shouldRender = !sceneGraphNode.data.isHidden;

        // frustrum culling

        if (shouldRender)
        {
            for (const auto & pair : sceneGraphNode.data.materialMeshPairs)
            {
                renderer.submit(RenderCommand{
                    pair.second->get(),
                    transform,
                    pair.first->get()
                });
            }

            for (const PointLight & light : sceneGraphNode.data.pointLights)
                renderer.submit(light);
        }

        for (const auto & childNode : sceneGraphNode.children)
            renderSceneGraphNode(*childNode, renderer, transform);
    }

    Scene::Scene()
    { }

    const SceneGraph & Scene::getGraph() const
    {
        return m_graph;
    }

    SceneGraph & Scene::getGraph()
    {
        return m_graph;
    }

    bool rayCastQueryMesh(const Vector3 & rayOrigin, const Vector3 & rayDirection, const Mesh * mesh, float & hitDistance)
    {
        const RayCastResult rayCastOBBResult = intersection_test::rayCastOBB(rayOrigin, rayDirection, mesh->getBoundingBox(), Matrix4::identity);         
        if (rayCastOBBResult.isIntersection && (hitDistance == 0 || rayCastOBBResult.distance < hitDistance))
        {
            const std::vector<MeshVertex> & meshVertices = mesh->getVertices();
            const std::vector<uint32_t> meshIndices = mesh->getIndices();

            if (meshIndices.size() == 0)
            {
                for (size_t idx = 0; idx < meshVertices.size(); idx += 3)
                {
                    const Vector3 & p0 = meshVertices[idx + 0].position;
                    const Vector3 & p1 = meshVertices[idx + 1].position;
                    const Vector3 & p2 = meshVertices[idx + 2].position;

                    if ((p0 - p1).cross(p0 - p2).dot(rayDirection) >= 0)
                        continue;
                        
                    RayCastResult rayCastTriangleResult = intersection_test::rayCastTriangle(rayOrigin, rayDirection, p0, p1, p2);
                    if (rayCastTriangleResult.isIntersection && (hitDistance == 0 || rayCastTriangleResult.distance < hitDistance))
                    {
                        hitDistance = rayCastTriangleResult.distance;
                        return true;
                    }
                }
            }
            else
            {
                for (size_t idx = 0; idx < meshIndices.size(); idx += 3)
                {
                    const Vector3 & p0 = meshVertices[meshIndices[idx + 0]].position;
                    const Vector3 & p1 = meshVertices[meshIndices[idx + 1]].position;
                    const Vector3 & p2 = meshVertices[meshIndices[idx + 2]].position;

                    if ((p0 - p1).cross(p0 - p2).dot(rayDirection) >= 0)
                        continue;

                    RayCastResult rayCastTriangleResult = intersection_test::rayCastTriangle(rayOrigin, rayDirection, p0, p1, p2);
                    if (rayCastTriangleResult.isIntersection && (hitDistance == 0 || rayCastTriangleResult.distance < hitDistance))
                    {
                        hitDistance = rayCastTriangleResult.distance;
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void rayCastQueryNode(SceneGraph::Node * node, const Matrix4 & parentTransform, const Vector3 & rayOrigin, const Vector3 & rayTarget, SceneGraph::Node *& selectedNode, float & hitDistance)
    {
        const Matrix4 nodeTransform = parentTransform * node->data.transform;

        if (!node->data.isHidden)
        {
            const Matrix4 inverseNodeTransform = nodeTransform.inverse();
            const Vector3 rayOriginMeshSpace = inverseNodeTransform * Vector4(rayOrigin, 1.0f);
            const Vector3 rayTargetMeshSpace = inverseNodeTransform * Vector4(rayTarget, 1.0f);
            const Vector3 rayDirectionMeshSpace = Vector3(rayTargetMeshSpace - rayOriginMeshSpace).normal();

            for (auto & materialMeshPair : node->data.materialMeshPairs)
                if (rayCastQueryMesh(rayOriginMeshSpace, rayDirectionMeshSpace, materialMeshPair.second->get(), hitDistance))
                    selectedNode = node;
        }

        for (SceneGraph::Node * child : node->children)
            rayCastQueryNode(child, nodeTransform, rayOrigin, rayTarget, selectedNode, hitDistance);
    }

    SceneGraph::Node * Scene::rayCastQuery(const Vector2 & normalisedDeviceCoordinates, Camera & camera)
    {
        Vector4 worldSpacePosition = camera.getProjectionViewMatrix().inverse() * Vector4(normalisedDeviceCoordinates.x, -normalisedDeviceCoordinates.y, -1, 1);
        worldSpacePosition /= worldSpacePosition.w;

        SceneGraph::Node * selectedNode = nullptr;
        float hitDistance = 0;

        for (SceneGraph::Node * rootNode : m_graph.getRootNodes())
            rayCastQueryNode(rootNode, Matrix4::identity, camera.getPosition(), worldSpacePosition, selectedNode, hitDistance);

        return selectedNode;
    }

    void Scene::render(ForwardRenderer & renderer, const Camera & camera)
    {
        for (const auto & node : m_graph.getRootNodes())
            renderSceneGraphNode(*node, renderer);

        renderer.draw(camera);
    }
}