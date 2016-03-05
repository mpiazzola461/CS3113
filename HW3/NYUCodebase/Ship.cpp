#include "Ship.h"

Ship::Ship() : xPos(0.0f), yPos(0.0f), moveSpeed(0.0f) {};
Ship::Ship(ShaderProgram* program, SheetSprite sprite, SheetSprite shotSprite, float xPos, float yPos, float moveSpeed, bool enemy) : 
	  program(program), keys(SDL_GetKeyboardState(NULL)), sprite(sprite), shotSprite(shotSprite), xPos(xPos), yPos(yPos), moveSpeed(moveSpeed),
	  bullet(Bullet(program, shotSprite)), alive(true), enemy(enemy)
{};

void Ship::Update(const float & player_xPos, const SheetSprite & player_sprite, Bullet & player_bullet, bool & player_alive, int & enemies_alive) {
	if (enemy){
		if (alive){
			program->setModelMatrix(modelMatrix);
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;

			modelMatrix.identity();
			modelMatrix.Translate(xPos, yPos, 0.0f);
			int trigger = rand() % 4000 + 1;
			if (!bullet.alive && trigger == 1 && player_alive){
				bullet.xPos = xPos;
				bullet.yPos = yPos - this->sprite.height * 2.0f;
				bullet.direction = -1.0f;
				bullet.alive = true;
			}
			if (yPos - sprite.height < player_bullet.yPos + player_bullet.sprite.height &&
				yPos + sprite.height > player_bullet.yPos - player_bullet.sprite.height &&
				xPos + sprite.width > player_bullet.xPos + player_bullet.sprite.width &&
				xPos - sprite.width < player_bullet.xPos - player_bullet.sprite.width){
				alive = false;
				player_bullet.alive = false;
				enemies_alive--;
			}
			if (-0.95f + player_sprite.height > bullet.yPos - bullet.sprite.height &&
				-0.95f - player_sprite.height < bullet.yPos + bullet.sprite.height &&
				player_xPos + player_sprite.width > bullet.xPos + bullet.sprite.width &&
				player_xPos - player_sprite.width < bullet.xPos - bullet.sprite.width){
				bullet.alive = false;
				player_alive = false;
			}

			sprite.Draw();
			bullet.Update(elapsed);
		}
	}
	else{
		if (alive){
			program->setModelMatrix(modelMatrix);
			float ticks = (float)SDL_GetTicks() / 1000.0f;
			float elapsed = ticks - lastFrameTicks;
			lastFrameTicks = ticks;

			modelMatrix.identity();
			modelMatrix.Translate(xPos, yPos, 0.0f);

			if (keys[SDL_SCANCODE_RIGHT] && xPos + sprite.width / 2 < 1.95f){
				xPos += elapsed * moveSpeed;
			}
			else if (keys[SDL_SCANCODE_LEFT] && xPos - sprite.width / 2 > -1.95f){
				xPos -= elapsed * moveSpeed;
			}

			if (keys[SDL_SCANCODE_SPACE]) {
				if (!bullet.alive){
					bullet.xPos = xPos;
					bullet.yPos = yPos + this->sprite.height * 2.0f;
					bullet.direction = 1.0f;
					bullet.alive = true;
				}

			}
			sprite.Draw();
			bullet.Update(elapsed);
		}
	}
}
