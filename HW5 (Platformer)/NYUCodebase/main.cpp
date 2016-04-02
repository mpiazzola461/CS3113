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

using namespace std;

#define TILE_SIZE 1.5f
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8 

//Globals
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
vector<Entity*> ents;
unsigned int mapID;

GLuint LoadTexture(const char* image_path){
	SDL_Surface* surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

bool readHeader(std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool readLayerData(std::ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

bool readEntityData(std::ifstream &stream) {
	string line;
	string type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE;
			//placeEntity(type, placeX, placeY);
			player = new Entity();
			player->x = placeX;
			player->y = placeY;
			player->height = TILE_SIZE;
			player->width = TILE_SIZE;
			player->isStatic = false;
			player->isSolid = true;
			player->sprite = new SheetSprite(mapID, player->width, player->height, 4.0);
			//player->entityType = type;
			ents.push_back(player);
		}
	}
	return true;
}

void readMap(){
	ifstream infile("map2.txt");
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			//readEntityData(infile);
		}
	}
}

void renderWorld(){
	vector<float> vertexData;
	vector<float> texCoordData;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
			float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
			float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
			float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
			vertexData.insert(vertexData.end(), {
				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				TILE_SIZE * x, -TILE_SIZE * y,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
			});
			texCoordData.insert(texCoordData.end(), {
				u, v,
				u, v + (spriteHeight),
				u + spriteWidth, v + (spriteHeight),
				u, v,
				u + spriteWidth, v + (spriteHeight),
				u + spriteWidth, v
			});
		}
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	glBindTexture(GL_TEXTURE_2D, mapID);
	glDrawArrays(GL_TRIANGLES, 0, 1536);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(-worldY / TILE_SIZE);
}

void update(float elapsed){

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_RIGHT]){
			//player->acceleration_x = 5.0;
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			//player->acceleration_x = -5.0;
		}
		else{
			//player->acceleration_x = 0.0;
		}
		if (keys[SDL_SCANCODE_SPACE]){
			//if (player->collidedBottom)
				//player->velocity_y = 5.0;
		}
		if (keys[SDL_SCANCODE_0]){ done = true; }
	}

	//player->Update(elapsed);
}

void checkCollisions(vector<Entity*> ents){
	//for each dynamic entity check if it is colliding with the world around it
}

void render(){
	glClear(GL_COLOR_BUFFER_BIT);
	renderWorld();
	//player->Render(program, modelMatrix);
	SDL_GL_SwapWindow(displayWindow);
}

void updateAndRender(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		//update(FIXED_TIMESTEP);
	}
	//update(fixedElapsed);
	checkCollisions(ents);
	render();
}

void init(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Marcus Piazzola Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	done = false;

	glViewport(0, 0, 640, 360);

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	projectionMatrix.setOrthoProjection(-8.0f, 8.0f, -6.0f, 6.0f, -1.0f, 1.0f);
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	glUseProgram(program->programID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	mapID = LoadTexture("arne_sprites.png");
	readMap();
}

int main(int argc, char *argv[]){
	init();
	while (!done) { //you can press 0 to exit the game (if it were to work)
		updateAndRender();
		//Scrolling
		viewMatrix.identity();
		//viewMatrix.Translate(-player->x, -player->y, 0.0);
		viewMatrix.Translate(0.0, 0.0, 0.0);
		program->setViewMatrix(viewMatrix);
	}
	return 0;
}
