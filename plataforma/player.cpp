#include "player.h"

Player::Player(Vector2 startPos)
{
	idleAnim = CreateAnimation("assets/idle.png", 11, 0.1f);
	runAnim = CreateAnimation("assets/run.png", 12, 0.07f);
	jumpAnim = CreateAnimation("assets/jump.png", 1, 0.1f);
	fallAnim = CreateAnimation("assets/fall.png", 1, 0.1f);
	hitAnim = CreateAnimation("assets/hit.png", 7, 0.03f, false);
	doubleJumpAnim = CreateAnimation("assets/doubleJump.png", 6, 0.03f);

	currentAnim = &idleAnim;
	
	hitboxSize = {16.0f, 30.0f};

	position = { 
		startPos.x,
		startPos.y
	};

	spriteOffset = { (hitboxSize.x - (float)currentAnim->frameWidth) / 2.0f, hitboxSize.y - (float)currentAnim->frameHeight};

	velocity = { 0.0f, 0.0f };
	jumpsLeft = maxJumps;
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
	case PlayerState::DoubleJump: currentAnim = &doubleJumpAnim; break;
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

	if (state == PlayerState::DoubleJump)
	{
		if (velocity.y > 0.0f) ChangeState(PlayerState::Fall);
		else return;
	}

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

	//logica de pulo
	if (jumpBufferTimer > 0.0f)
	{
		if (coyoteTimer > 0.0f)
		{
			velocity.y = -jumpForce;
			isGrounded = false;
			jumpsLeft--;
			jumpBufferTimer = 0.0f;
			coyoteTimer = 0.0f;
			ChangeState(PlayerState::Jump);
		}
		else if (jumpsLeft > 0)
		{
			velocity.y = -jumpForce;
			jumpsLeft--;
			jumpBufferTimer = 0.0f;

			ChangeState(PlayerState::DoubleJump);
		}
	}

	if (!isGrounded && coyoteTimer <= 0.0f && jumpsLeft == maxJumps)
	{
		jumpsLeft = maxJumps - 1;
	}

	//Variavel da altura de pulo (jump cut)
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

void Player::ApplyMovement(float dt, TileMap& map)
{
	// HORIZONTAL
	velocity.x = moveInput * acceleration;
	float oldX = position.x;
	position.x += velocity.x * dt;

	if (map.CheckCollision(GetHitbox()))
	{
		position.x = oldX;
		velocity.x = 0.0f;
	}

	//VERTICAL
	float oldY = position.y;
	position.y += velocity.y * dt;

	if (map.CheckCollision(GetHitbox()))
	{
		position.y = oldY;
		velocity.y = 0.0f;
	}

	Rectangle groundSensor = GetHitbox();
	groundSensor.y += 0.5f;

	if (map.CheckCollision(groundSensor) && velocity.y >= 0.0f)
	{
		isGrounded = true;
		jumpsLeft = maxJumps;
		velocity.y = 0.0f;
	}
	else
	{
		isGrounded = false;
	}
}

Rectangle Player::GetHitbox() const
{
	return { position.x, position.y, hitboxSize.x, hitboxSize.y };
}

void Player::Update(float dt, TileMap& map)
{

	// INPUT
	HandleControls();
	HandleJump(dt);

	// AÇÕES
	if (attackPressed && !IsStateLocked())
	{
		ChangeState(PlayerState::Hit);
	}
	
	// FÍSICA
	ApplyGravity(dt);
	ApplyMovement(dt, map);

	// ESTADO E ANIMAÇÃO
	UpdateState();
	UpdateAnimation(*currentAnim, dt, facingRight);
}

void Player::Draw()
{
	Vector2 drawPos = {
		position.x + spriteOffset.x,
		position.y + spriteOffset.y
	};

	DrawTextureRec(currentAnim->texture, currentAnim->frameRec, drawPos, WHITE);
}

void Player::DrawCollisionDebug()
{
	Rectangle col = GetHitbox();
	DrawRectangleLines((int)col.x, (int)col.y, (int)col.width, (int)col.height, RED);

	// Dica visual extra: Desenhe um pequeno ponto verde indicando a origem do player (0,0 da hitbox)
	DrawCircle((int)position.x, (int)position.y, 2.0f, GREEN);
}

PlayerState Player::GetState() const
{
	return state;
}