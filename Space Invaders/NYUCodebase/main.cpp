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
#include "Matrix.h"
#include "SheetSprite.h"
#include "Ship.h"
#include <vector>

enum GameState {INTRO, GAME};
int gameState;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Window* displayWindow;


void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	ShaderProgram* program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glViewport(0, 0, 500, 500);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Matrix modelMatrixMain;
	Matrix modelMatrixOver;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-2.0, 2.0, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint textSheet = LoadTexture(RESOURCE_FOLDER"font1.png");
	GLuint spriteSheet = LoadTexture(RESOURCE_FOLDER"sheet.png");
	SheetSprite playerSprite(program, spriteSheet, 112.0f / 1024.0f, 866.0f / 1024.0f, 112.0f / 1024.0f, 75.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite playerShotSprite(program, spriteSheet,	809.0f / 1024.0f, 437.0f / 1024.0f, 19.0f / 1024.0f, 30.0f / 1024.0f, 0.1, GL_CLAMP);
	SheetSprite enemySprite(program, spriteSheet, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.15, GL_CLAMP);
	SheetSprite enemyShotSprite(program, spriteSheet, 856.0f / 1024.0f, 421.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 0.1, GL_CLAMP);
	
	Ship player(program, playerSprite, playerShotSprite, 0.0f, -0.95f, 1.0f, false);
	std::vector<Ship> enemies;
	for (int row = 0; row < 4; row++){
		for (int col = 0; col < 8; col++) {
			if (row == 0){
				Ship enemy(program, enemySprite, enemyShotSprite, -1.5f + (col * 0.4f), 1.5f, 0.0, true);
				enemies.push_back(enemy);
			}
			else if (row == 1){
				Ship enemy(program, enemySprite, enemyShotSprite, -1.5f + (col * 0.4f), 1.0, 0.0, true);
				enemies.push_back(enemy);
			}
			else if (row == 2){
				Ship enemy(program, enemySprite, enemyShotSprite, -1.5f + (col * 0.4f), 0.5f, 0.0, true);
				enemies.push_back(enemy);
			}
			else if (row = 3){
				Ship enemy(program, enemySprite, enemyShotSprite, -1.5f + (col * 0.4f), 0.0f, 0.0, true);
				enemies.push_back(enemy);
			}
		}
	}
	int enemiesAlive = enemies.size();

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program->setViewMatrix(viewMatrix);
		program->setProjectionMatrix(projectionMatrix);
		glUseProgram(program->programID);

		switch (gameState)
		{
		case INTRO:
			program->setModelMatrix(modelMatrixMain);
			modelMatrixMain.identity();
			modelMatrixMain.Translate(-1.4f, -0.5, 0.0f);
			DrawText(program, textSheet, "Press Enter To Start", 0.15, 0.0005f);
			program->setModelMatrix(modelMatrixOver);
			modelMatrixOver.identity();
			modelMatrixOver.Translate(-1.625f, 0.5, 0.0f);
			DrawText(program, textSheet, "SPACE INVADERS", 0.25, 0.0005f);
			if (keys[SDL_SCANCODE_RETURN]) { 
				gameState = GAME;
			}
			break;
		case GAME:
			if (player.alive && enemiesAlive > 0){
				player.Update(player.xPos, player.sprite, player.bullet, player.alive, enemiesAlive);
				for (int i = 0; i < 32; i++) enemies[i].Update(player.xPos, player.sprite, player.bullet, player.alive, enemiesAlive);
			}
			
			break;
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}