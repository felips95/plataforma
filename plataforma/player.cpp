#include "player.h"

Player::Player(Vector2 startPos)
{
	idleAnim = CreateAnimation("assets/idle.png", 11, 0.1f);
	runAnim = CreateAnimation("assets/run.png", 12, 0.07f);
	jumpAnim = CreateAnimation("assets/jump.png", 1, 0.1f);
	fallAnim = CreateAnimation("assets/fall.png", 1, 0.1f);
	hitAnim = CreateAnimation("assets/hit.png", 7, 0.03f, false);

	currentAnim = &idleAnim;
	
	size = {
		(float)currentAnim->frameWidth,
		(float)currentAnim->frameHeight
	};

	position = { 
		startPos.x - (float)currentAnim->frameWidth,
		startPos.y - (float)currentAnim->frameHeight
	};

	velocity = { 0.0f, 0.0f };
}

Player::~Player()
{
	UnloadAnimation(idleAnim);
	UnloadAnimation(runAnim);
	UnloadAnimation(hitAnim);
	UnloadAnimation(jumpAnim);
	UnloadAnimation(fallAnim);
}

void Player::ChangeState(PlayerState newState)
{
	if (state == newState) return;

	state = newState;

	switch (state)
	{
	case PlayerState::Idle: currentAnim = &idleAnim; break;
	case PlayerState::Run: currentAnim = &runAnim; break;
	case PlayerState::Hit: currentAnim = &hitAnim; break;
	case PlayerState::Jump: currentAnim = &jumpAnim; break;
	case PlayerState::Fall: currentAnim = &fallAnim; break;
	}

	currentAnim->frameAtual = 0;
	currentAnim->timer = 0.0f;
	currentAnim->finished = false;
}

bool Player::IsStateLocked() const
{
	switch (state)
	{
	case PlayerState::Hit:
		return true;

	default:
		return false;
	}
}

void Player::UpdateState()
{
	// Se o estado atual estiver travado,
	// espera a animação terminar
	if (IsStateLocked() && !currentAnim->finished) return;


	if (!isGrounded)
	{
		ChangeState(velocity.y <= 0.0f ? PlayerState::Jump : PlayerState::Fall);
	}
	else
	{
		ChangeState(moveInput != 0.0f ? PlayerState::Run : PlayerState::Idle);
	}
}

void Player::HandleControls()
{
	moveInput = 0;
	jumpPressed = false;
	jumpReleased = false;

	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		moveInput += 1.0f;

	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) 
		moveInput -= 1.0f;

	if (moveInput > 0.0f) facingRight = true;
	else if (moveInput < 0.0f) facingRight = false;

	jumpPressed = IsKeyPressed(KEY_SPACE);
	jumpReleased = IsKeyReleased(KEY_SPACE);

	attackPressed = IsKeyPressed(KEY_F);
}

void Player::HandleJump(float dt)
{
	//jump buffer
	if (jumpPressed)
		jumpBufferTimer = jumpBufferTime;
	else if (jumpBufferTimer > 0.0f)
		jumpBufferTimer -= dt;

	//Coyote Time
	if (isGrounded)
		coyoteTimer = coyoteTime;
	else if (coyoteTimer > 0.0f)
		coyoteTimer -= dt;

	if (jumpBufferTimer > 0.0f && coyoteTimer > 0.0f)
	{
		velocity.y = -jumpForce;
		isGrounded = false;
		jumpBufferTimer = 0.0f;
		coyoteTimer = 0.0f;
	}

	if (jumpReleased && velocity.y < 0.0f)
		velocity.y *= jumpCutMultiplier;
}

void Player::ApplyGravity(float dt)
{
	if (velocity.y < 0.0f)
		velocity.y += gravity * dt;
	else
		velocity.y += gravity * fallMultiplier * dt;
}

void Player::ApplyMovement(float dt)
{
	// HORIZONTAL
	velocity.x = moveInput * acceleration;
	position.x += velocity.x * dt;

	//VERTICAL
	position.y += velocity.y * dt;
}

Rectangle Player::GetCollisionRect() const
{
	float collisionOffsetX = 10.0f;
	Vector2 collisionSize{ size.x - collisionOffsetX, size.y };
	Vector2 collisionPos{ position.x + collisionOffsetX / 2.0f, position.y };

	return { collisionPos.x, collisionPos.y, collisionSize.x, collisionSize.y };
}

void Player::GroundCollision(Rectangle ground)
{
	Rectangle col = GetCollisionRect();

	if (CheckCollisionRecs(col, ground) && velocity.y >= 0.0f)
	{
		isGrounded = true;
		velocity.y = 0.0f;
		position.y = ground.y - size.y;
	}
}

void Player::Update(float dt, Rectangle ground)
{

	// INPUT
	HandleControls();
	HandleJump(dt);

	// AÇÕES
	if (attackPressed && !IsStateLocked())
	{
		ChangeState(PlayerState::Hit);
	}
	
	// MOVIMENTO
	ApplyGravity(dt);
	ApplyMovement(dt);
	
	// COLISÃO
	isGrounded = false;
	GroundCollision(ground);

	// ESTADO
	UpdateState();

	// ANIMAÇÃO
	UpdateAnimation(*currentAnim, dt, facingRight);
}

void Player::Draw()
{
	DrawTextureRec(currentAnim->texture, currentAnim->frameRec, position, WHITE);
}

void Player::DrawCollisionDebug()
{
	Rectangle col = GetCollisionRect();
	DrawRectangleLines((int)col.x, (int)col.y, (int)col.width, (int)col.height, RED);
}

PlayerState Player::GetState() const
{
	return state;
}