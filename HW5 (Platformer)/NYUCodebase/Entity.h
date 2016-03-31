#pragma once
#include "SheetSprite.h"
#include <vector>
#define FIXED_TIMESTEP 0.03333f
#define MAX_TIMESTEPS 6

class Entity{
public:
	Entity();
	void Render(ShaderProgram* program, Matrix& modelMatrix);
	void Update(float elapsed);
	bool collides(Entity *entity);
	float lerp(float v0, float v1, float t);
	void resetFlags();

	float x;
	float y;
	float height;
	float width;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	float friction;
	SheetSprite* sprite;
	bool isStatic;
	bool solid;
	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;
};

