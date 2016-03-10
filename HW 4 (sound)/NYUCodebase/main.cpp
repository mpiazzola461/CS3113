/*
Marcus Piazzola
CS 3113 Ivan Safrin Intro to Game Programming
Assignment 2
*/

#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Marcus Piazzola Scene", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;
	bool done = false;

	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	GLuint padL = LoadTexture("pad1.png");
	GLuint padR = LoadTexture("pad2.png");
	GLuint ball = LoadTexture("ball.png");

	//Initialize SDL mixer
	int mix = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	
	//Load a sound
	Mix_Chunk *aSound;
	aSound = Mix_LoadWAV("eddieMurphy.wav");
	
	//Load Music
	Mix_Music *music;
	music = Mix_LoadMUS("sorry.mp3");

	Mix_PlayMusic(music, -1);

	int count = 0;
	bool scored = false;
	//left paddle attribs
	float TopL = 0.5;
	float BotL = -0.5;
	float yPosL = 0.0;
	float xPosL = -3.0;

	//right paddle attribs
	float TopR = 0.5;
	float BotR = -0.5;
	float yPosR = 0.0;
	float xPosR = 3.0;

	//ball attribs
	bool hitR = false;
	bool hitL = true;
	bool hitT = false;
	bool hitB = true;
	float ballX = 0.0;
	float ballY = 0.0;
	float ballTop = 0.25;
	float ballBot = -0.25;
	float ballL = -0.25;
	float ballR = 0.25;


	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	float lastFrameTicks = 0.0f;
	glUseProgram(program.programID);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		program.setModelMatrix(modelMatrix);

		//Left Paddle
		if (keys[SDL_SCANCODE_W] && TopL <= 2.0){
			yPosL += elapsed;
			TopL += elapsed;
			BotL += elapsed;
		}
		else if (keys[SDL_SCANCODE_S] && BotL >= -2.0){
			yPosL -= elapsed;
			BotL -= elapsed;
			TopL -= elapsed;
		}
		modelMatrix.Translate(0.0, yPosL, 0.0);
		program.setModelMatrix(modelMatrix);
		modelMatrix.identity();
		if (scored){
			scored = false;

			//left paddle attribs
			TopL = 0.5;
			BotL = -0.5;
			yPosL = 0.0;
			xPosL = -3.0;

			//right paddle attribs
			TopR = 0.5;
			BotR = -0.5;
			yPosR = 0.0;
			xPosR = 3.0;

			//ball attribs
			hitR = true;
			hitL = false;
			hitT = true;
			hitB = false;
			ballX = 0.0;
			ballY = 0.0;
			ballTop = 0.25;
			ballBot = -0.25;
			ballL = -0.25;
			ballR = 0.25;
			//done = true;
		}
		float vertices1[] = { -3.55, -0.5, -3.0, -0.5, -3.0, 0.5, -3.55, -0.5, -3.0, 0.5, -3.55, 0.5 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, padL);
		glDrawArrays(GL_TRIANGLES, 0, 6);	
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		//Ball
		program.setModelMatrix(modelMatrix);
		if (hitB && hitL){
			ballL += elapsed;
			ballR += elapsed;
			ballBot += elapsed;
			ballTop += elapsed;
			ballX += elapsed;
			ballY += elapsed;
		}
		else if (hitB && hitR){
			ballL -= elapsed;
			ballR -= elapsed;
			ballBot += elapsed;
			ballTop += elapsed;
			ballX -= elapsed;
			ballY += elapsed;
		}
		else if (hitT && hitL){
			ballL += elapsed;
			ballR += elapsed;
			ballBot -= elapsed;
			ballTop -= elapsed;
			ballX += elapsed;
			ballY -= elapsed;
		}
		else if (hitT && hitR){
			ballL -= elapsed;
			ballR -= elapsed;
			ballBot -= elapsed;
			ballTop -= elapsed;
			ballX -= elapsed;
			ballY -= elapsed;
		}
		if (ballBot <= -2.0){
			hitB = true;
			hitT = false;
		}
		if (ballTop >= 2.0){
			hitT = true;
			hitB = false;
		}
		if ((ballR >= xPosR) && (ballTop <= TopR) && (ballBot >= BotR)){
			hitR = true;
			hitL = false;
		}
		if ((ballL <= xPosL) && (ballTop <= TopL) && (ballBot >= BotL)){
			hitR = false;
			hitL = true;
		}

		if (ballL <= -3.5 || ballR >= 3.5){
			scored = true;
			count++;
			Mix_PlayChannel(-1, aSound, 0);
		}

		modelMatrix.Translate(ballX, ballY, 0.0);
		program.setModelMatrix(modelMatrix);

		float vertices2[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, ball);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		//Right Paddle
		program.setModelMatrix(modelMatrix);
		if (keys[SDL_SCANCODE_UP] && TopR <= 2.0 ){
			yPosR += elapsed;
			TopR += elapsed;
			BotR += elapsed;
		}
		else if (keys[SDL_SCANCODE_DOWN] && BotR >= -2.0){
			yPosR -= elapsed;
			BotR -= elapsed;
			TopR -= elapsed;
		}
		modelMatrix.identity();
		modelMatrix.Translate(0.0, yPosR, 0.0);
		program.setModelMatrix(modelMatrix);
		modelMatrix.identity();
		float vertices3[] = { 3.0, -0.5, 3.55, -0.5, 3.55, 0.5, 3.0, -0.5, 3.55, 0.5, 3.0, 0.5 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);	
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, padR);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);

		if (count > 3){
			done = true;
		}

	}

	Mix_FreeChunk(aSound);
	Mix_FreeMusic(music);

	SDL_Quit();
	return 0;
}