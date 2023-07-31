#ifndef SCENE_HPP
#define SCENE_HPP

#include "graphics/forward_renderer.hpp"
#include "graphics/scene_graph.hpp"

namespace mud
{
    class Scene
    {
    public:

        Scene();

        const SceneGraph & getGraph() const;

        SceneGraph & getGraph();

        SceneGraph::Node * rayCastQuery(const Vector2 & normalisedDeviceCoordinates, Camera & camera);

        void render(ForwardRenderer & renderer, const Camera & camera);

    private:

        SceneGraph m_graph;
    };
}

#endif