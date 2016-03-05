#include "Bullet.h"

Bullet::Bullet() {};
Bullet::Bullet(ShaderProgram* program, SheetSprite sprite) : program(program), sprite(sprite), xPos(), yPos(), moveSpeed(2.0),
direction(0.0f), alive(false) {};
void Bullet::Update(float elapsed) {
	program->setModelMatrix(modelMatrix);
	modelMatrix.identity();
	modelMatrix.Translate(xPos, yPos, 0.0f);
	if (alive) {
		yPos += elapsed * moveSpeed * direction;
		sprite.Draw();
	}
	if ((direction == 1 && yPos > 1.5f) || (direction == -1 && yPos < -1.5f)) alive = false;
}