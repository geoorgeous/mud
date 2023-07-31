#include "application.hpp"

#include "graphics/camera.hpp"
#include "graphics/forward_renderer.hpp"
#include "graphics/mesh.hpp"
#include "graphics/mesh_factory.hpp"
#include "graphics/scene_graph.hpp"
#include "graphics/texture.hpp"
#include "graphics/window_graphics_context.hpp"
#include "math/math.hpp"
#include "platform/ui.hpp"
#include "platform/window.hpp"
#include "scene.hpp"
#include "utils/asset_importer.hpp"
#include "utils/asset_manager.hpp"
#include "utils/asset.hpp"
#include "utils/console.hpp"
#include "utils/logger.hpp"
#include "utils/stopwatch.hpp"
#include "utils/text_input_buffer.hpp"

#include <chrono>

// todo
// wireframe
// normals, depth, location, 
// debug shapes, lines, gizmos
// transparency
// deferred lighting
// object outline
// ui
//  view assets
//  view scene
// add meshes to scene
// scene save/load
// commands
// dynamic meshes

#define MUD_RESOLUTION_X 1920
#define MUD_RESOLUTION_Y 1080
#define MUD_FOV 90
#define MUD_FOV_RESOLUTION_RATIO ((static_cast<float>(MUD_RESOLUTION_X) / static_cast<float>(MUD_RESOLUTION_Y)) / static_cast<float>(MUD_FOV))
#define MUD_CAMERA_LOOK_SENSITIVITY 0.001
#define MUD_CAMERA_MOVE_SPEED 5
#define MUD_FPS_MAX 360
#define MUD_FPS_BACKGROUND 4

namespace mud
{
    std::string setWindowMode()
    {
        return "Done";
    }

    Application::~Application()
    {
        console::destroy();
        AssetManager::getInstance().unloadAssets();
        delete m_applicationGraphicsContext;
    }

    void Application::run()
    {
        Stopwatch mainLoopStopwatch;
        Stopwatch renderStopwatch;
        double elapsedSinceDraw;

        WindowProperties windowProperties;
        windowProperties.width = MUD_RESOLUTION_X;
        windowProperties.height = MUD_RESOLUTION_Y;
        windowProperties.title = "Mud Engine";

        m_applicationGraphicsContext = new ApplicationGraphicsContext;
        m_applicationGraphicsContext->init(windowProperties);
        Window * window = m_applicationGraphicsContext->getMainWindow();

        AssetManager::getInstance().importLocalAssets();

        window->setIcon(AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/mud_icon_x32.png")->get());

        Asset<Mesh> * cubeMesh = AssetManager::getInstance().newAsset<Mesh>();
        cubeMesh->allocateObject();
        mesh_factory::cube(*cubeMesh->get());

        Asset<Material> * cubeMaterial = AssetManager::getInstance().newAsset<Material>();
        cubeMaterial->allocateObject();
        cubeMaterial->get()->baseColor = Color::cornflowerBlue.toVector();
        cubeMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        cubeMaterial->get()->normalMap = cubeMaterial->get()->metalnessMap = cubeMaterial->get()->roughnessMap = cubeMaterial->get()->diffuseMap;

        Asset<Material> * redMaterial = AssetManager::getInstance().newAsset<Material>();
        redMaterial->allocateObject();
        redMaterial->get()->baseColor = Color::red.toVector();
        redMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        redMaterial->get()->normalMap = redMaterial->get()->metalnessMap = redMaterial->get()->roughnessMap = redMaterial->get()->diffuseMap;

        Asset<Material> * greenMaterial = AssetManager::getInstance().newAsset<Material>();
        greenMaterial->allocateObject();
        greenMaterial->get()->baseColor = Color::green.toVector();
        greenMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        greenMaterial->get()->normalMap = greenMaterial->get()->metalnessMap = greenMaterial->get()->roughnessMap = greenMaterial->get()->diffuseMap;

        Asset<Material> * blueMaterial = AssetManager::getInstance().newAsset<Material>();
        blueMaterial->allocateObject();
        blueMaterial->get()->baseColor = Color::blue.toVector();
        blueMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        blueMaterial->get()->normalMap = blueMaterial->get()->metalnessMap = blueMaterial->get()->roughnessMap = blueMaterial->get()->diffuseMap;

        Asset<Material> * selectedMeshMaterial = AssetManager::getInstance().newAsset<Material>();
        selectedMeshMaterial->allocateObject();
        selectedMeshMaterial->get()->baseColor = Color::yellow.toVector();
        selectedMeshMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        selectedMeshMaterial->get()->normalMap = selectedMeshMaterial->get()->metalnessMap = selectedMeshMaterial->get()->roughnessMap = selectedMeshMaterial->get()->diffuseMap;

        Asset<Material> * visitedMeshMaterial = AssetManager::getInstance().newAsset<Material>();
        visitedMeshMaterial->allocateObject();
        visitedMeshMaterial->get()->baseColor = Color::gold.toVector();
        visitedMeshMaterial->get()->diffuseMap = AssetManager::getDefaultAsset<Texture>();
        visitedMeshMaterial->get()->normalMap = visitedMeshMaterial->get()->metalnessMap = visitedMeshMaterial->get()->roughnessMap = visitedMeshMaterial->get()->diffuseMap;
        
        Asset<SceneGraph> * sceneGraph1 = AssetManager::getInstance().importAsset<SceneGraph>("C:/Users/George/Desktop/mud/res/models/sponza_crytek/sponza.obj");
        //Asset<SceneGraph> * sceneGraph1 = AssetManager::getInstance().importAsset<SceneGraph>("C:/Users/George/Desktop/mud/res/models/sponza_pbr/NewSponza_Main_glTF_002.gltf");
        //Asset<SceneGraph> * sceneGraph2 = AssetManager::getInstance().importAsset<SceneGraph>("C:/Users/George/Desktop/mud/res/models/sponza_pbr/NewSponza_Curtains_glTF.gltf");

        Camera camera;
        camera.setPosition(Vector3(1, 3, 5));

        Scene scene;
        SceneGraph::Node * selectedNode = nullptr;

        SceneGraph::Node * lights = scene.getGraph().newNode();
        lights->data.pointLights.push_back(PointLight{Vector3(10, 0.25, 0), Color::white, 1.0f, 0.35f, 0.44f});
        lights->data.pointLights.push_back(PointLight{Vector3(2, 7, 0), Color::orange, 1.0f, 0.35f, 0.44f});
        lights->data.pointLights.push_back(PointLight{Vector3(-5, 1, 0), Color::cornflowerBlue, 1.0f, 0.35f, 0.44f});

        SceneGraph::Node * cube1 = scene.getGraph().newNode();
        cube1->data.transform = transform_t(-2.0f, 0.0f, 0.0f);
        cube1->data.materialMeshPairs.push_back({ cubeMaterial, cubeMesh });

        SceneGraph::Node * cube2 = scene.getGraph().newNode();
        cube2->data.transform = transform_t(0.0f, 0.0f, 0.0f);
        cube2->data.materialMeshPairs.push_back({ cubeMaterial, cubeMesh });

        SceneGraph::Node * cube3 = scene.getGraph().newNode();
        cube3->data.transform = transform_t(2.0f, 0.0f, 0.0f);
        cube3->data.materialMeshPairs.push_back({ cubeMaterial, cubeMesh });

        SceneGraph::Node * cube4 = scene.getGraph().newNode();
        cube4->data.transform = transform_t(4.0f, 0.0f, 0.0f);
        cube4->data.materialMeshPairs.push_back({ cubeMaterial, cubeMesh });

        SceneGraph::Node * model1 = scene.getGraph().copyNodeTree(*sceneGraph1->get());
        model1->data.transform = transform_s(0.01f);

        //SceneGraph::Node * model2 = scene.getGraph().copyNodeTree(*sceneGraph2->get());
        //model2->data.transform = Matrix4::identity;
        
        RenderPassOptions renderPassOptions;
        renderPassOptions.clearColorBuffer = true;
        renderPassOptions.clearDepthBuffer = true;
        renderPassOptions.wireframe = false;
        renderPassOptions.faceCullMode = FaceCullMode::None;
        ForwardRenderer forwardRenderer(renderPassOptions);
        forwardRenderer.getDirectionalLight().color = Color(Vector3(0.05f));

        Scene gizmoScene;
        SceneGraph::Node * selectedGizmo = nullptr;

        SceneGraph::Node * translateGizmoX = gizmoScene.getGraph().newNode();
        translateGizmoX->data.materialMeshPairs.push_back({ redMaterial, cubeMesh });
        translateGizmoX->data.isHidden = true;

        SceneGraph::Node * translateGizmoY = gizmoScene.getGraph().newNode();
        translateGizmoY->data.materialMeshPairs.push_back({ greenMaterial, cubeMesh });
        translateGizmoY->data.isHidden = true;
        
        SceneGraph::Node * translateGizmoZ = gizmoScene.getGraph().newNode();
        translateGizmoZ->data.materialMeshPairs.push_back({ blueMaterial, cubeMesh });
        translateGizmoZ->data.isHidden = true;
        
        renderPassOptions.clearColorBuffer = false;
        renderPassOptions.clearDepthBuffer = true;
        renderPassOptions.wireframe = false;
        renderPassOptions.faceCullMode = FaceCullMode::Back;
        ForwardRenderer debugForwardRenderer(renderPassOptions);
        debugForwardRenderer.getDirectionalLight().color = Color(Vector3(0.8f));

        ui::UIContext uiContext;

        ui::Element * e1 = uiContext.newElement();
        e1->setSize(Vector2(600, 25));
        e1->setTexture(AssetManager::getDefaultAsset<Texture>()->get());
        e1->setText("Hello, World");
        e1->setColor(Color(0, 0, 0, 0.5f));
        e1->setForegroundColor(Color::white);
        e1->setIsVisible(false);

        ui::Element * fpsText = uiContext.newElement();
        fpsText->setForegroundColor(Color::green);
    
        console::init(*window);

        console::Command * newSceneNodeCommand = new console::Command("newSceneNode", "Sets the camera field of view", {}, [&](const std::vector<console::Argument *> & arguments) -> std::string {
            SceneGraph::Node * newNode = scene.getGraph().newNode();
            newNode->data.transform = Matrix4::identity;
            newNode->data.materialMeshPairs.push_back({ cubeMaterial, cubeMesh });
            return "Spawned new entity";
        });
        console::registerCommand(newSceneNodeCommand);
        
        mainLoopStopwatch.start();
        while (!window->getShouldClose())
        {
            float deltaTime = static_cast<float>(mainLoopStopwatch.stop() * 0.001);
            mainLoopStopwatch.start();

            {
                if (camera.getAspectRatio() != window->getAspectRatio())
                    camera.setPerspective(window->getAspectRatio() / MUD_FOV_RESOLUTION_RATIO / window->getAspectRatio(), window->getAspectRatio(), 0.1f, 100.0f);

                if (window->getKeyState(Key::GraveAccent).action == KeyAction::Released)
                {
                    bool enableCLI = !console::getInputBuffer()->getIsFocussed();
                    console::getInputBuffer()->setIsFocussed(enableCLI);
                    e1->setIsVisible(enableCLI);
                }

                if (window->getKeyState(Key::Num1).action == KeyAction::Released)
                    window->setMode(WindowMode::Windowed);
                if (window->getKeyState(Key::Num2).action == KeyAction::Released)
                    window->setMode(WindowMode::WindowedFullscreen);
                if (window->getKeyState(Key::Num3).action == KeyAction::Released)
                    window->setMode(WindowMode::Fullscreen);

                if (console::getInputBuffer()->getIsFocussed())
                {
                    e1->setText(" > " + console::getInputBuffer()->getContent());
                }
                else
                {
                    Vector3 cameraRotation;

                    MouseState mouseState = window->getMouseState();

                    if (mouseState.buttons[MouseButton::Right].isPressed)
                    {
                        window->setCursorInputMode(CursorInputMode::Disabled);

                        if (mouseState.cursorDelta != Vector2::zero)
                        {
                            cameraRotation.y = mouseState.cursorDelta.x * MUD_CAMERA_LOOK_SENSITIVITY;
                            cameraRotation.x = mouseState.cursorDelta.y * MUD_CAMERA_LOOK_SENSITIVITY;
                        }
                    }
                    else
                    {
                        window->setCursorInputMode(CursorInputMode::Normal);

                        if (mouseState.buttons[MouseButton::Left].action == MouseButtonAction::Pressed)
                        {
                            selectedGizmo = gizmoScene.rayCastQuery(window->getNormalisedDeviceCoordinates(mouseState.cursorPosition), camera);
                        }
                        else if (mouseState.buttons[MouseButton::Left].action == MouseButtonAction::Released)
                        {
                            if (selectedGizmo == nullptr)
                            {
                                selectedNode = scene.rayCastQuery(window->getNormalisedDeviceCoordinates(mouseState.cursorPosition), camera);

                                const bool isNodeSelected = selectedNode != nullptr;

                                translateGizmoX->data.isHidden = !isNodeSelected;
                                translateGizmoY->data.isHidden = !isNodeSelected;
                                translateGizmoZ->data.isHidden = !isNodeSelected;

                                if (isNodeSelected)
                                {
                                    const Vector3 nodePosition = SceneGraph::getNodeWorldTransform(*selectedNode)[3];

                                    translateGizmoX->data.transform = transform_t(nodePosition + Vector3::posX) * transform_s(0.25f);
                                    translateGizmoY->data.transform = transform_t(nodePosition + Vector3::posY) * transform_s(0.25f);
                                    translateGizmoZ->data.transform = transform_t(nodePosition + Vector3::posZ) * transform_s(0.25f);
                                }
                            }
                            
                            selectedGizmo = nullptr;
                        }

                        if (mouseState.buttons[MouseButton::Left].isPressed)
                        {
                            if (selectedGizmo != nullptr)
                            {
                                const Matrix4 nodeWorldTransform = SceneGraph::getNodeWorldTransform(*selectedNode);
                                
                                const Vector2 cursorPosNDC = window->getNormalisedDeviceCoordinates(mouseState.cursorPosition);
                                Vector4 worldSpacePosition = camera.getProjectionViewMatrix().inverse() * Vector4(cursorPosNDC.x, -cursorPosNDC.y, -1, 1);
                                worldSpacePosition /= worldSpacePosition.w;
                                Vector3 rayDirection = Vector3(Vector3(worldSpacePosition) - camera.getPosition()).normal();

                                if (selectedGizmo == translateGizmoX)
                                {
                                    RayCastResult result = intersection_test::rayCastPlane(camera.getPosition(), rayDirection, selectedNode->data.transform[3], Vector3::posY);
                                    Vector3 planePos = camera.getPosition() + rayDirection * result.distance;

                                    selectedNode->data.transform[3][0] = (planePos.x - 1) * (1 / nodeWorldTransform[0][0]);
                                }
                                else if (selectedGizmo == translateGizmoY)
                                {
                                    RayCastResult result = intersection_test::rayCastPlane(camera.getPosition(), rayDirection, selectedNode->data.transform[3], Vector3::posX);
                                    Vector3 planePos = camera.getPosition() + rayDirection * result.distance;

                                    selectedNode->data.transform[3][1] = (planePos.y - 1) * (1 / nodeWorldTransform[1][1]);
                                }
                                else if (selectedGizmo == translateGizmoZ)
                                {
                                    RayCastResult result = intersection_test::rayCastPlane(camera.getPosition(), rayDirection, selectedNode->data.transform[3], Vector3::posY);
                                    Vector3 planePos = camera.getPosition() + rayDirection * result.distance;

                                    selectedNode->data.transform[3][2] = (planePos.z - 1) * (1 / nodeWorldTransform[2][2]);
                                }
                                
                                const Vector3 nodePosition = nodeWorldTransform[3];
                                
                                translateGizmoX->data.transform = transform_t(nodePosition + Vector3::posX) * transform_s(0.25f);
                                translateGizmoY->data.transform = transform_t(nodePosition + Vector3::posY) * transform_s(0.25f);
                                translateGizmoZ->data.transform = transform_t(nodePosition + Vector3::posZ) * transform_s(0.25f);
                            }
                        }
                    }

                    if (window->getKeyState(Key::Q).isPressed)
                        cameraRotation.z += 1 * deltaTime;
                    if (window->getKeyState(Key::E).isPressed)
                        cameraRotation.z -= 1 * deltaTime;

                    camera.rotate(Quaternion::fromEulerAngles(cameraRotation));

                    Vector3 cameraOffset;
                    const float moveSpeed = MUD_CAMERA_MOVE_SPEED * deltaTime;

                    if (window->getKeyState(Key::A).isPressed)
                        cameraOffset.x -= moveSpeed;
                    if (window->getKeyState(Key::D).isPressed)
                        cameraOffset.x += moveSpeed;
                    if (window->getKeyState(Key::W).isPressed)
                        cameraOffset.z += moveSpeed;
                    if (window->getKeyState(Key::S).isPressed)
                        cameraOffset.z -= moveSpeed;
                    if (window->getKeyState(Key::LeftControl).isPressed)
                        cameraOffset.y -= moveSpeed;
                    if (window->getKeyState(Key::Space).isPressed)
                        cameraOffset.y += moveSpeed;

                    camera.move(cameraOffset);
                }
            }

            {
                const float maxFps = window->getIsFocused() ? MUD_FPS_MAX : MUD_FPS_BACKGROUND;
                elapsedSinceDraw += deltaTime;
                if (elapsedSinceDraw >= 1.0f / maxFps && !window->getIsMinimised() && window->getGraphicsContext().beginDraw())
                {
                    //fpsText->setText(fmt::format( "{0}", 1.0f / elapsedSinceDraw));

                    elapsedSinceDraw = 0;

                    renderStopwatch.start();

                    scene.render(forwardRenderer, camera);
                    gizmoScene.render(debugForwardRenderer, camera);

                    uiContext.render();

                    window->getGraphicsContext().endDraw();

                    renderStopwatch.stop();
                }
            }

            window->pollEvents();
        }

        window->getGraphicsContext().getSwapchain().waitUntilIdle();
    }
}