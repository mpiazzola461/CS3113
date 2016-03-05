#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "ShaderProgram.h"
#include "SheetSprite.h"
#include "Bullet.h"


class Ship {
public:
	Ship();
	Ship(ShaderProgram* program, SheetSprite sprite, SheetSprite shotSprite, float xPos, float yPos, float moveSpeed, bool enemy);
	void Update(const float & player_xPos, const SheetSprite & player_sprite, Bullet & player_bullet, bool & player_alive, int & enemiesAlive);
	
	const Uint8 *keys;
	ShaderProgram* program;
	Matrix modelMatrix;
	SheetSprite sprite;
	SheetSprite shotSprite;
	float lastFrameTicks;
	float xPos;
	float yPos;
	float moveSpeed;
	bool alive;
	bool enemy;
	int bulletIndex;
	Bullet bullet;
};