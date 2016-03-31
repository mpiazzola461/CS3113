#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "Entity.h"
#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Game{
public:
	Game();
	~Game();

	GLuint LoadTexture(const char* image_path);

	void init();
	void buildPlayer(); 
	void buildWorld();
	void renderTiles();
	bool readHeader(std::ifstream &stream);
	bool readLayerData(std::ifstream &stream);
	void scroll();
	bool isSolid(int c);
	void playerCollisions();
	void ProcessEvents(float elapsed);
	void Update(float elapsed);
	void Render();
	bool UpdateAndRender();
	
	SDL_Window* displayWindow;
	SDL_Event event;
	bool done;
	ShaderProgram* program;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	float lastFrameTicks;
	
	int mapWidth;
	int mapHeight;
	unsigned char** levelData;
	Entity* player;
	std::vector<Entity*> blocks;
	unsigned int tileID;
	unsigned int playerID;
};