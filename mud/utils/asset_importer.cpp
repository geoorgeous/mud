#include "asset_importer.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#include "asset_manager.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "logger.hpp"
#include "math/math.hpp"

namespace mud
{
	std::string createAssetFilenameFromAssimpMaterial(const aiMaterial * assimpMaterial)
	{
		aiString name;
		assimpMaterial->Get(AI_MATKEY_NAME, name);
		return (name.length == 0 || name.length > 32) ? "Material" : std::string(name.C_Str());
	}

	std::string createAssetFilenameFromAssimpMesh(const aiMesh * assimpMesh)
	{
		return (assimpMesh->mName.length == 0 || assimpMesh->mName.length > 32) ? "Mesh" : std::string(assimpMesh->mName.C_Str());
	}
	
	std::string stringToLower(const std::string & string)
	{
		std::string copy = string;
		for (char & c : copy)
			if (std::isalpha(static_cast<int>(c)))
				c = static_cast<char>(std::tolower(static_cast<int>(c)));
		return copy;
	}

	std::string stringToUpper(const std::string & string)
	{
		std::string copy = string;
		for (char & c : copy)
			if (std::isalpha(static_cast<int>(c)))
				c = static_cast<char>(std::toupper(static_cast<int>(c)));
		return copy;
	}

	std::string assimpShadingModeToString(aiShadingMode mode)
	{
		switch (mode)
		{
		case aiShadingMode_NoShading: return "none";
		case aiShadingMode_Flat: return "flat";
		case aiShadingMode_Gouraud: return "gourad";
		case aiShadingMode_Phong: return "phong";
		case aiShadingMode_Blinn: return "blinn";
		case aiShadingMode_Toon: return "toon";
		case aiShadingMode_OrenNayar: return "oren-nayar";
		case aiShadingMode_Minnaert: return "minnaert";
		case aiShadingMode_CookTorrance: return "cook-torrance";
		case aiShadingMode_Fresnel: return "fresnel";
		case aiShadingMode_PBR_BRDF: return "pbr";
		default: return "unknown";
		}
	}

	void printMaterialTextures(const aiMaterial * assimpMaterial, const aiTextureType assimpTextureType, const std::string & textureTypeString)
	{
		aiString texturePath;

		unsigned int textureCount = assimpMaterial->GetTextureCount(assimpTextureType);
		for (size_t textureIdx = 0; textureIdx < textureCount; ++textureIdx)
		{
			assimpMaterial->Get(AI_MATKEY_TEXTURE(assimpTextureType, textureIdx), texturePath);
			log(LogLevel::Trace, fmt::format("\tTexture.{0}[{1}]: {2}\n", textureTypeString, textureIdx, texturePath.C_Str()));
		}
	}

	std::vector<Asset<Material> *> processAssimpMaterials(AssetManager & assetManager, const std::string & filepath, const aiScene * assimpScene)
	{
		std::vector<Asset<Material> *> materialAssets;

		for (size_t materialIdx = 0; materialIdx < assimpScene->mNumMaterials; ++materialIdx)
		{
			//log(LogLevel::Trace, fmt::format("=== Material[{0}] '{1}' ===\n", materialIdx, material->GetName().C_Str()));

			const aiMaterial * assimpMaterial = assimpScene->mMaterials[materialIdx];

			aiString name;
			aiShadingMode shadingModel;
			aiColor3D diffuse;
			aiColor3D specular;
			aiColor3D ambient;
			aiColor3D emissive;
			aiColor3D transparent;
			aiColor3D reflective;
			int shouldUseWireframe;
			int isTwoSided;
			float opacity;
			float shininess;
			float shininessStrength;
			float refractionIndex;
			float reflectivity;

			assimpMaterial->Get(AI_MATKEY_NAME, name);
			assimpMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
			assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			assimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
			assimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
			assimpMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);
			assimpMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, shouldUseWireframe);
			assimpMaterial->Get(AI_MATKEY_TWOSIDED, isTwoSided);
			assimpMaterial->Get(AI_MATKEY_OPACITY, opacity);
			assimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
			assimpMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			assimpMaterial->Get(AI_MATKEY_REFRACTI, refractionIndex);
			assimpMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);

			log(LogLevel::Trace, fmt::format("3D scene Material found: '{0}'\n", name.C_Str()));
			log(LogLevel::Trace, fmt::format("\tShading Model: '{0}'\n", assimpShadingModeToString(shadingModel)));
			log(LogLevel::Trace, fmt::format("\tColor.Diffuse: {0}, {1}, {2}\n", diffuse.r, diffuse.g, diffuse.b));
			log(LogLevel::Trace, fmt::format("\tColor.Specular: {0}, {1}, {2}\n", specular.r, specular.g, specular.b));
			log(LogLevel::Trace, fmt::format("\tColor.Ambient: {0}, {1}, {2}\n", ambient.r, ambient.g, ambient.b));
			log(LogLevel::Trace, fmt::format("\tColor.Emissive: {0}, {1}, {2}\n", emissive.r, emissive.g, emissive.b));
			log(LogLevel::Trace, fmt::format("\tColor.Transparent: {0}, {1}, {2}\n", transparent.r, transparent.g, transparent.b));
			log(LogLevel::Trace, fmt::format("\tColor.Reflective: {0}, {1}, {2}\n", reflective.r, reflective.g, reflective.b));
			log(LogLevel::Trace, fmt::format("\tWireframe: {0}\n", shouldUseWireframe == 0 ? "false" : "true"));
			log(LogLevel::Trace, fmt::format("\tTwo-sided: {0}\n", isTwoSided == 0 ? "false" : "true"));
			log(LogLevel::Trace, fmt::format("\tOpacity: {0}\n", opacity));
			log(LogLevel::Trace, fmt::format("\tShininess: {0}, {1}\n", shininess, shininessStrength));
			log(LogLevel::Trace, fmt::format("\tRefration Index: {0}\n", refractionIndex));
			log(LogLevel::Trace, fmt::format("\tReflectivity: {0}\n", reflectivity));

			aiColor3D pbrBaseColor;
			int pbrUseMetallicMap;
			int pbrUseRoughnessMap;
			float pbrMetallicFactor;
			float pbrRoughnessFactor;
			float pbrSpecularFactor;
			float pbrGlossinessFactor;

			assimpMaterial->Get(AI_MATKEY_BASE_COLOR, pbrBaseColor);
			assimpMaterial->Get(AI_MATKEY_USE_METALLIC_MAP, pbrUseMetallicMap);
			assimpMaterial->Get(AI_MATKEY_USE_ROUGHNESS_MAP, pbrUseRoughnessMap);
			assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, pbrMetallicFactor);
			assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, pbrRoughnessFactor);
			assimpMaterial->Get(AI_MATKEY_SPECULAR_FACTOR, pbrSpecularFactor);
			assimpMaterial->Get(AI_MATKEY_GLOSSINESS_FACTOR, pbrGlossinessFactor);

			log(LogLevel::Trace, fmt::format("\tPBR.BaseColor: {0}, {1}, {2}\n", pbrBaseColor.r, pbrBaseColor.g, pbrBaseColor.b));
			log(LogLevel::Trace, fmt::format("\tPBR.Use Metallic Map: {0}\n", pbrUseMetallicMap == 0 ? "false" : "true"));
			log(LogLevel::Trace, fmt::format("\tPBR.Use Roughness Map: {0}\n", pbrUseRoughnessMap == 0 ? "false" : "true"));
			log(LogLevel::Trace, fmt::format("\tPBR.Metallic Factor: {0}\n", pbrMetallicFactor));
			log(LogLevel::Trace, fmt::format("\tPBR.Roughness Factor: {0}\n", pbrRoughnessFactor));
			log(LogLevel::Trace, fmt::format("\tPBR.Specular Factor: {0}\n", pbrSpecularFactor));
			log(LogLevel::Trace, fmt::format("\tPBR.Glossiness Factor: {0}\n", pbrGlossinessFactor));

			printMaterialTextures(assimpMaterial, aiTextureType_DIFFUSE, "Diffuse");
			printMaterialTextures(assimpMaterial, aiTextureType_SPECULAR, "Specular");
			printMaterialTextures(assimpMaterial, aiTextureType_AMBIENT, "Ambient");
			printMaterialTextures(assimpMaterial, aiTextureType_EMISSIVE, "Emissive");
			printMaterialTextures(assimpMaterial, aiTextureType_HEIGHT, "Height");
			printMaterialTextures(assimpMaterial, aiTextureType_NORMALS, "Normals");
			printMaterialTextures(assimpMaterial, aiTextureType_SHININESS, "Shininess");
			printMaterialTextures(assimpMaterial, aiTextureType_OPACITY, "Opacity");
			printMaterialTextures(assimpMaterial, aiTextureType_DISPLACEMENT, "Displacement");
			printMaterialTextures(assimpMaterial, aiTextureType_LIGHTMAP, "Lightmap");
			printMaterialTextures(assimpMaterial, aiTextureType_REFLECTION, "Reflection");
			printMaterialTextures(assimpMaterial, aiTextureType_BASE_COLOR, "PBR_Base");
			printMaterialTextures(assimpMaterial, aiTextureType_NORMAL_CAMERA, "PBR_NormalCamera");
			printMaterialTextures(assimpMaterial, aiTextureType_EMISSION_COLOR, "PBR_Emission");
			printMaterialTextures(assimpMaterial, aiTextureType_METALNESS, "PBR_Metalness");
			printMaterialTextures(assimpMaterial, aiTextureType_DIFFUSE_ROUGHNESS, "PBR_DiffuseRoughness");
			printMaterialTextures(assimpMaterial, aiTextureType_AMBIENT_OCCLUSION, "PBR_AmbientOcclusion");

			Asset<Material> * newMaterialAsset = assetManager.newAsset<Material>();
			newMaterialAsset->allocateObject();

			switch (shadingModel)
			{
			case aiShadingMode::aiShadingMode_PBR_BRDF:
				newMaterialAsset->get()->baseColor = Vector4(pbrBaseColor.r, pbrBaseColor.g, pbrBaseColor.b, 1.0f);
				break;

			default:
				newMaterialAsset->get()->baseColor = Vector4(diffuse.r, diffuse.g, diffuse.b, 1.0f);
				break;
			}

			const std::string importDirectory = std::filesystem::path(filepath).parent_path().string() + "/";
			const std::string assetDirectory = std::filesystem::path(filepath).filename().string() + "/";
			const std::string assetTexturesDirectory = assetDirectory + "Textures/";
			const std::string assetMaterialsDirectory = assetDirectory + "Materials/";

			aiString texturePath;

			assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, 0), texturePath);
			if (texturePath.length == 0)
				assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texturePath);

			if (texturePath.length != 0)
				newMaterialAsset->get()->diffuseMap = AssetManager::getInstance().importAsset<Texture>(importDirectory + texturePath.C_Str(), AssetManager::createAssetFilepath(assetTexturesDirectory + texturePath.C_Str()));

			if (newMaterialAsset->get()->diffuseMap == nullptr)
				newMaterialAsset->get()->diffuseMap = AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/white.png");

			assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), texturePath);

			if (texturePath.length != 0)
				newMaterialAsset->get()->normalMap = AssetManager::getInstance().importAsset<Texture>(importDirectory + texturePath.C_Str(), AssetManager::createAssetFilepath(assetTexturesDirectory + texturePath.C_Str()));

			if (newMaterialAsset->get()->normalMap == nullptr)
				newMaterialAsset->get()->normalMap = AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/black.png");

			assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, 0), texturePath);

			if (texturePath.length != 0)
				newMaterialAsset->get()->metalnessMap = AssetManager::getInstance().importAsset<Texture>(importDirectory + texturePath.C_Str(), AssetManager::createAssetFilepath(assetTexturesDirectory + texturePath.C_Str()));

			if (newMaterialAsset->get()->metalnessMap == nullptr)
				newMaterialAsset->get()->metalnessMap = AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/black.png");

			assimpMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, 0), texturePath);

			if (texturePath.length != 0)
				newMaterialAsset->get()->roughnessMap = AssetManager::getInstance().importAsset<Texture>(importDirectory + texturePath.C_Str(), AssetManager::createAssetFilepath(assetTexturesDirectory + texturePath.C_Str()));

			if (newMaterialAsset->get()->roughnessMap == nullptr)
				newMaterialAsset->get()->roughnessMap = AssetManager::getInstance().importAsset<Texture>("C:/Users/George/Desktop/mud/res/images/black.png");

			newMaterialAsset->save(assetManager.createUniqueAssetFilepath(assetMaterialsDirectory + createAssetFilenameFromAssimpMaterial(assimpMaterial)));
			newMaterialAsset->unload();

			materialAssets.emplace_back(newMaterialAsset);
		}

		return materialAssets;
	}

	void processAssimpMesh(const aiMesh * mesh, std::vector<MeshVertex> & vertices, std::vector<uint32_t> & indices)
	{
		MeshVertex * vertex;
		Vector3 * vPositions;
		Vector3 * vNormals;
		Vector4 * vColors;
		Vector3 * vTextureCoordinates;

		vertices.resize(mesh->mNumVertices);
		vertex = vertices.data();

		vPositions = reinterpret_cast<Vector3 *>(mesh->mVertices);

		vNormals = reinterpret_cast<Vector3 *>(mesh->mNormals);

		vColors = mesh->HasVertexColors(0) ? reinterpret_cast<Vector4 *>(mesh->mColors[0]) : nullptr;

		vTextureCoordinates = mesh->HasTextureCoords(0) ? reinterpret_cast<Vector3 *>(mesh->mTextureCoords[0]) : nullptr;

		for (size_t vIdx = 0; vIdx < mesh->mNumVertices; ++vIdx)
		{
			vertex->position = vPositions[vIdx];

			vertex->normal = vNormals[vIdx];

			vertex->colour = (vColors == nullptr) ? Vector4(1.0f) : vColors[vIdx];

#if defined(MUD_USE_VULKAN)
			if (vTextureCoordinates) vertex->textureCoordinates = { vTextureCoordinates[vIdx].x, 1.0f - vTextureCoordinates[vIdx].y };
#elif defined(MUD_USE_OPENGL)
			if (vTextureCoordinates) vertex->textureCoordinates = textureCoordinates[vIdx];
#endif

			vertex++;
		}

		if (mesh->HasFaces())
		{
			indices.resize(mesh->mNumFaces * 3);

			for (size_t fIdx = 0; fIdx < mesh->mNumFaces; ++fIdx)
			{
				aiFace face = mesh->mFaces[fIdx];

				indices[fIdx * 3 + 0] = face.mIndices[0];
				indices[fIdx * 3 + 1] = face.mIndices[1];
				indices[fIdx * 3 + 2] = face.mIndices[2];
			}
		}
		else
			indices.clear();
	}

	std::vector<Asset<Mesh> *> processAssimpMeshes(AssetManager & assetManager, const std::string & filepath, const aiScene * assimpScene)
	{
		std::vector<Asset<Mesh> *> meshAssets;

		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;

		const std::string assetMeshesDirectory = std::filesystem::path(filepath).filename().string() + "/Meshes/";

		for (size_t meshIdx = 0; meshIdx < assimpScene->mNumMeshes; ++meshIdx)
		{
			aiMesh * assimpMesh = assimpScene->mMeshes[meshIdx];
			processAssimpMesh(assimpMesh, vertices, indices);

			Asset<Mesh> * newMeshAsset = assetManager.newAsset<Mesh>();
			newMeshAsset->allocateObject();
			newMeshAsset->get()->setData(vertices, indices);

			newMeshAsset->save(assetManager.createUniqueAssetFilepath(assetMeshesDirectory + createAssetFilenameFromAssimpMesh(assimpMesh)));
			newMeshAsset->unload();

			meshAssets.emplace_back(newMeshAsset);
		}

		return meshAssets;
	}

	SceneGraph::Node * processAssimpNode(SceneGraph & sceneGraph, const std::vector<Asset<Material> *> & materialAssets, const std::vector<Asset<Mesh> *> & meshAssets, const std::string & filepath, const aiNode * assimpNode, const aiScene * assimpScene)
	{
		SceneGraph::Node * newSceneGraphNode = sceneGraph.newNode();

		aiMatrix4x4 assimpNodeTransform = assimpNode->mTransformation;
		assimpNodeTransform.Transpose();
		newSceneGraphNode->data.transform = *reinterpret_cast<Matrix4 *>(&assimpNodeTransform);

		for (size_t meshIdx = 0; meshIdx < assimpNode->mNumMeshes; ++meshIdx)
		{
			aiMesh * assimpMesh = assimpScene->mMeshes[assimpNode->mMeshes[meshIdx]];

			newSceneGraphNode->data.materialMeshPairs.emplace_back(
				materialAssets[assimpMesh->mMaterialIndex],
				meshAssets[assimpNode->mMeshes[meshIdx]]);
		}

		for (size_t lightIdx = 0; lightIdx < assimpScene->mNumLights; ++lightIdx)
		{
			const aiLight * assimpLight = assimpScene->mLights[lightIdx];

			if (assimpNode->mName.length != 0 && assimpNode->mName == assimpLight->mName)
			{
				switch (assimpLight->mType)
				{
				case aiLightSource_AMBIENT:
					log(LogLevel::Trace, fmt::format("3D scene Light found (Ambient): {0}\n", assimpNode->mName.C_Str()));
					break;

				case aiLightSource_AREA:
					log(LogLevel::Trace, fmt::format("3D scene Light found (Area): {0}\n", assimpNode->mName.C_Str()));
					break;

				case aiLightSource_DIRECTIONAL:
					log(LogLevel::Trace, fmt::format("3D scene Light found (Directional): {0}\n", assimpNode->mName.C_Str()));
					break;

				case aiLightSource_POINT:
				{
					PointLight pointLight;
					aiVector3D lightPosition = assimpLight->mPosition + aiVector3D(assimpNode->mTransformation.a4, assimpNode->mTransformation.b4, assimpNode->mTransformation.c4);
					pointLight.position = Vector4(*reinterpret_cast<const Vector3 *>(&lightPosition), 1.0f);
					pointLight.color = Color(*reinterpret_cast<const Vector3 *>(&assimpLight->mColorDiffuse), 1.0f);
					if (pointLight.color.r == 0 && pointLight.color.g == 0 && pointLight.color.b == 0)
						pointLight.color = Color::white;
					pointLight.attenuationK = assimpLight->mAttenuationConstant;
					pointLight.attenuationL = assimpLight->mAttenuationLinear;
					pointLight.attenuationQ = assimpLight->mAttenuationQuadratic;
					newSceneGraphNode->data.pointLights.push_back(pointLight);
					log(LogLevel::Trace, fmt::format("3D scene Light found (Point): {0}\n", assimpNode->mName.C_Str()));
					break;
				}

				case aiLightSource_SPOT:
					log(LogLevel::Trace, fmt::format("3D scene Light found (Spot): {0}\n", assimpNode->mName.C_Str()));
					break;

				case aiLightSource_UNDEFINED:
					log(LogLevel::Trace, fmt::format("3D scene Light found (Undefined): {0}\n", assimpNode->mName.C_Str()));
					break;
				}

				log(LogLevel::Trace, fmt::format("\tPosition: [{0}, {1}, {2}]\n", assimpNode->mTransformation.a4, assimpNode->mTransformation.b4, assimpNode->mTransformation.c4));
				log(LogLevel::Trace, fmt::format("\tColor (ambient): r={0}, g={1}, b={2}\n", assimpLight->mColorAmbient.r, assimpLight->mColorAmbient.g, assimpLight->mColorAmbient.b));
				log(LogLevel::Trace, fmt::format("\tColor (diffuse): r={0}, g={1}, b={2}\n", assimpLight->mColorDiffuse.r, assimpLight->mColorDiffuse.g, assimpLight->mColorDiffuse.b));
				log(LogLevel::Trace, fmt::format("\tColor (specular): r={0}, g={1}, b={2}\n", assimpLight->mColorSpecular.r, assimpLight->mColorSpecular.g, assimpLight->mColorSpecular.b));
				log(LogLevel::Trace, fmt::format("\tAttenuation: k={0}, l={1}, q={2}\n", assimpLight->mAttenuationConstant, assimpLight->mAttenuationLinear, assimpLight->mAttenuationQuadratic));
				log(LogLevel::Trace, fmt::format("\tDirection: [{0}, {1}, {2}]\n", assimpLight->mDirection.x, assimpLight->mDirection.y, assimpLight->mDirection.z));
				log(LogLevel::Trace, fmt::format("\tCone: inner angle={0}, outer angle={1}\n", assimpLight->mAngleInnerCone, assimpLight->mAngleOuterCone));
				log(LogLevel::Trace, fmt::format("\tArea size: [{0}, {1}]\n", assimpLight->mSize.x, assimpLight->mSize.y));
			}
		}

		for (size_t childIdx = 0; childIdx < assimpNode->mNumChildren; childIdx++)
		{
			const aiNode * assimpChildNode = assimpNode->mChildren[childIdx];
			SceneGraph::Node * childNode = processAssimpNode(sceneGraph, materialAssets, meshAssets, filepath, assimpChildNode, assimpScene);
			sceneGraph.setNodeParent(childNode, newSceneGraphNode);
		}

		return newSceneGraphNode;
	}
}

namespace mud
{
	template<>
	bool asset_importer::import<FontFamily>(const std::string & filepath, Asset<FontFamily> * asset, const std::string & assetFilepath)
	{
		asset->allocateObject();
		if (!asset->get()->fromFile(filepath))
			return false;
		asset->setImportFilepath(filepath);
		asset->save(assetFilepath);
		asset->unload();
		return true;
	}

	template<>
	bool asset_importer::import<SceneGraph>(const std::string & filepath, Asset<SceneGraph> * asset, const std::string & assetFilepath)
	{
		Assimp::Importer importer;

		const aiScene * assimpScene = importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_MaxQuality);

		if (!assimpScene)
		{
			log(LogLevel::Error, fmt::format("Failed to read mesh file: {0}\n", importer.GetErrorString()));
			return false;
		}

		log(LogLevel::Trace, "Processing materials...\n");

		std::vector<Asset<Material> *> materialAssets = processAssimpMaterials(AssetManager::getInstance(), filepath, assimpScene);

		log(LogLevel::Trace, "Processing meshes...\n");

		std::vector<Asset<Mesh> *> meshAssets = processAssimpMeshes(AssetManager::getInstance(), filepath, assimpScene);

		log(LogLevel::Trace, "Processing scene graph...\n");
		
		asset->allocateObject();

		for (size_t childIdx = 0; childIdx < assimpScene->mRootNode->mNumChildren; ++childIdx)
		{
			const aiNode * assimpChildNode = assimpScene->mRootNode->mChildren[childIdx];
			processAssimpNode(*asset->get(), materialAssets, meshAssets, filepath, assimpChildNode, assimpScene);
		}

		asset->setImportFilepath(filepath);
		asset->save(AssetManager::createAssetFilepath(std::filesystem::path(filepath).filename().string() + "/" + std::filesystem::path(filepath).filename().string()));
		asset->unload();

		return true;
	}

	template<>
	bool asset_importer::import<Texture>(const std::string & filepath, Asset<Texture> * asset, const std::string & assetFilepath)
	{
		int width, height, channels;

		stbi_uc * pixels = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels)
		{
			const char * stbiFailureReason = stbi_failure_reason();
			if (stbiFailureReason)
			{
				log(LogLevel::Error, fmt::format("Failed to load image from file '{0}': {1}\n", filepath, stbiFailureReason), "Texture");
				return false;
			}
			log(LogLevel::Error, fmt::format("Failed to load image from file '{0}'\n", filepath), "Texture");
			return false;
		}

		asset->allocateObject();

		asset->get()->setData(
			reinterpret_cast<uint8_t *>(pixels),
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height),
			static_cast<uint32_t>(STBI_rgb_alpha));

		asset->setImportFilepath(filepath);
		asset->save(assetFilepath);
		asset->unload();

		return true;
	}
}