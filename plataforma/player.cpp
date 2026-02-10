#include "player.h"

Animation CreateAnimation(const char* file, int frames, float frameTime)
{
	Animation anim;
	anim.texture = LoadTexture(file);
	anim.framesTotal = frames;
	anim.frameTime = frameTime;

	anim.frameWidth = anim.texture.width / frames;
	anim.frameHeight = anim.texture.height;

	anim.frameRec = {
		0.0f,
		0.0f,
		(float)anim.frameWidth,
		(float)anim.frameHeight
	};
	return anim;
}

Player::Player(Vector2 startPos)
{
	idleAnim = CreateAnimation("assets/idle2.png", 11, 0.1f);
	walkAnim = CreateAnimation("assets/run2.png", 12, 0.07f);

	currentAnim = &idleAnim;

	size = {
		(float)currentAnim->frameWidth,
		(float)currentAnim->frameHeight
	};

	position = { 
		startPos.x - (float)currentAnim->frameWidth,
		startPos.y - (float)currentAnim->frameHeight
	};
}

Player::~Player()
{
	UnloadTexture(idleAnim.texture);
	UnloadTexture(walkAnim.texture);
}

void Player::ChangeState(PlayerState newState)
{
	if (state == newState)
		return;

	state = newState;

	switch (state)
	{
	case PlayerState::Idle:
		currentAnim = &idleAnim;
		break;
	case PlayerState::Run:
		currentAnim = &walkAnim;
		break;
	}

	currentAnim->frameAtual = 0;
	currentAnim->timer = 0.0f;
	currentAnim->frameRec.y = 0.0f;
}

void Player::UpdateAnimation()
{
	currentAnim->timer += GetFrameTime();

	if (currentAnim->timer >= currentAnim->frameTime)
	{
		currentAnim->timer = 0.0f;
		currentAnim->frameAtual++;
	}
	if (currentAnim->frameAtual >= currentAnim->framesTotal)
	{
		currentAnim->frameAtual = 0;
	}

	currentAnim->frameRec.x = currentAnim->frameAtual * currentAnim->frameHeight;
}

void Player::HandleControls()
{
	velocity.x = 0;

	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) velocity.x = acceleration;
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) velocity.x = -acceleration;

}

void Player::update(float deltaTime, Rectangle ground)
{
	HandleControls();

	//movimento horizontal
	position.x += velocity.x * deltaTime;

	//gravidade
	velocity.y += gravity * deltaTime;
	position.y += velocity.y * deltaTime;

	//collision
	int collisionOffsetX = 10;
	Vector2 collisionSize{ size.x - collisionOffsetX, size.y };
	Vector2 collisionPos{ position.x + collisionOffsetX / 2, position.y };

	Rectangle playerCollision = { collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y };
	isGrounded = CheckCollisionRecs(playerCollision, ground);

	if (isGrounded)
	{
		velocity.y = 0;
		position.y = ground.y - size.y;
	}

	DrawRectangleLines(playerCollision.x, playerCollision.y, playerCollision.width, playerCollision.height, RED);

	//animation
	UpdateAnimation();

	if (velocity.x != 0)
	{
		ChangeState(PlayerState::Run);
	}
	else
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::draw()
{
	DrawTextureRec(currentAnim->texture, currentAnim->frameRec, position, WHITE);
}