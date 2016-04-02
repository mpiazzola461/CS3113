#pragma once
#include "SheetSprite.h"
#include <vector>
#define FIXED_TIMESTEP 0.03333f
#define MAX_TIMESTEPS 6
#define friction_x 1
#define friction_y 1


enum EntityType {Player, Enemy};

class Entity{
public:
	Entity();

	void Render(ShaderProgram* program, Matrix& modelMatrix);
	void Update(float elapsed);
	float lerp(float v0, float v1, float t);

	SheetSprite* sprite;

	float x;
	float y;
	float height;
	float width;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;

	EntityType entityType;
	
	bool isStatic;
	bool isSolid;

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;
};

