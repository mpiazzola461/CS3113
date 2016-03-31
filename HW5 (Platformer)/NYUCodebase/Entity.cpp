#include "Entity.h"
Entity::Entity(){}

float Entity::lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}

void Entity::Update(float elapsed){
	resetFlags();

	velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction);
	//velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
	velocity_x += acceleration_x * FIXED_TIMESTEP;
	velocity_y += acceleration_y * FIXED_TIMESTEP;
	x += velocity_x * FIXED_TIMESTEP;
	y += velocity_y * FIXED_TIMESTEP;
}

void Entity::Render(ShaderProgram* program, Matrix& modelMatrix){
	sprite->Draw(7, 8, 4, program, modelMatrix, x, y);
}

bool Entity::collides(Entity *entity){
	if (entity->solid){
		float left = x - (width / 2.0);
		float right = x + (width / 2.0);
		float top = y + (height / 2.0);
		float bot = y - (height / 2.0);
		float tileLeft = entity->x - (entity->width / 2.0);
		float tileRight = entity->x + (entity->width / 2.0);
		float tileTop = entity->y + (entity->height / 2.0);
		float tileBot = entity->y - (entity->height / 2.0);
		if (bot < tileTop){
			float penetration = fabs(bot - tileTop);
			y += penetration + 0.001;
			collidedBottom = true;
			return true;
		}
		else if (top > tileBot){
			float penetration = fabs(top - tileBot);
			y -= penetration + 0.001;
			collidedBottom = true;
			return true;
		}
		if (left < tileRight){
			float penetration = fabs(left - tileRight);
			x += penetration + 0.001;
			collidedLeft = true;
			return true;
		}
		else if (right > tileLeft){
			float penetration = fabs(right - tileLeft);
			x -= penetration + 0.001;
			collidedRight = true;
			return true;
		}
		return false;
	}
}

void Entity::resetFlags(){
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;
}