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

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(ShaderProgram* program, unsigned int textureID, float u, float v, float width, float height, float size, GLint wrap_mode);
	void Draw();
	ShaderProgram* program;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float size;
	GLint wrap_mode;
};