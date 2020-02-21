#pragma once

#include "Transform.h"

class Mesh;
class Camera;
class Material;
class Transform;

struct ID3D11DeviceContext;

class Entity 
{
public:
	Entity(class Mesh* incomingMesh, class Material* incomingMaterial);
	~Entity() = default;

	class Mesh* GetMesh() const;
	class Transform* GetTransform();

	void Draw(struct ID3D11DeviceContext* context, class Camera* mainCamera);

private:
	class Transform transform;
	class Mesh* mesh;
	class Material* material;
};