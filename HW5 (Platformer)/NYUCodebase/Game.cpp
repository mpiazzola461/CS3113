#include "Game.h"
#define TILE_SIZE 1.5f

Game::Game(){
	init();
	tileID = LoadTexture("arne_sprites.png");
	buildPlayer();
	buildWorld();
}

Game::~Game() { 
	SDL_Quit(); 
}

GLuint Game::LoadTexture(const char* image_path){
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

void Game::init(){
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
	projectionMatrix.setOrthoProjection(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f);

	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	glUseProgram(program->programID);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool Game::isSolid(int a){
	for (int i = 0; i < blocks.size(); i++){
		if (blocks[i]->solid)
			return true;
	}
	return false;
}


void Game::buildPlayer(){
	playerID = LoadTexture("characters_3.png");
	player = new Entity();
	player->x = 10.0;
	player->y = -10.0;
	player->height = 1 * TILE_SIZE;
	player->width = 0.75 * TILE_SIZE;
	player->sprite = new SheetSprite(playerID, player->width, player->height, 4.0);
	player->isStatic = false;
	player->acceleration_y = -20.0;
	player->friction = 1.0;
	player->solid = true;
}

void Game::buildWorld(){
	std::ifstream tileStream("map4.txt");
	std::string line;
	while (getline(tileStream, line)){
		if (line == "[header]"){
			if (!readHeader(tileStream)) {
				return;
			}
		}
		else if (line == "[layer]"){
			readLayerData(tileStream);
		}
	}
	tileStream.close();
	for (int y = 0; y < mapHeight; y++){
		for (int x = 0; x < mapWidth; x++){
			if (levelData[y][x] != 12){
				blocks.push_back(new Entity());
				blocks.back()->x = (TILE_SIZE * x) + (TILE_SIZE / 2.0);
				blocks.back()->y = (-TILE_SIZE * y) - (TILE_SIZE / 2.0);
				blocks.back()->height = TILE_SIZE;
				blocks.back()->width = TILE_SIZE;
				blocks.back()->isStatic = true;
				blocks.back()->solid = true;
			}
		}
	}
}

bool Game::readHeader(std::ifstream &stream){
	std::string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
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
	else { 
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool Game::readLayerData(std::ifstream &stream) {
	std::string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
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

void Game::renderTiles(){
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	int SPRITE_COUNT_X = 16;
	int SPRITE_COUNT_Y = 8;
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
	glBindTexture(GL_TEXTURE_2D, tileID);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 32 * 32);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Game::scroll(){
	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0.0);
	program->setViewMatrix(viewMatrix);
}


void Game::ProcessEvents(float elapsed){
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_RIGHT]){
			player->acceleration_x = 20.0;
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			player->acceleration_x = -20.0;
		}
		else{
			player->acceleration_x = 0.0;
		}
		if (keys[SDL_SCANCODE_SPACE]){
			if (player->collidedBottom)
				player->velocity_y = 15.0;
		}
		if (keys[SDL_SCANCODE_0]){ done = true; }
	}
}

void Game::playerCollisions(){
	for (int i = 0; i < blocks.size(); i++){
		player->collides(blocks[i]);
	}
}


void Game::Update(float elapsed){
	ProcessEvents(elapsed);
	player->Update(elapsed);
	playerCollisions();
	scroll();
}

void Game::Render(){
	glClear(GL_COLOR_BUFFER_BIT);
	renderTiles();
	player->Render(program, modelMatrix);
	SDL_GL_SwapWindow(displayWindow);
}

bool Game::UpdateAndRender(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		Update(FIXED_TIMESTEP);
	}

	Update(fixedElapsed);
	Render();
	return done;
}