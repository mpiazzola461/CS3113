#include "Entity.h"
Entity::Entity(){}

float Entity::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Entity::Update(float elapsed){
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
	velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
	velocity_x += acceleration_x * FIXED_TIMESTEP;
	velocity_y += acceleration_y * FIXED_TIMESTEP;
	x += velocity_x * FIXED_TIMESTEP;
	y += velocity_y * FIXED_TIMESTEP;
}

void Entity::Render(ShaderProgram* program, Matrix& modelMatrix){
	sprite->Draw(96, 16, 8, program, modelMatrix, x, y);
}
