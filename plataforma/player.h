#pragma once
#include "raylib.h"
#include "animation.h"
#include "tilemap.h"

enum class PlayerState
{
	Idle,
	Run,
	Jump,
	Fall,
	Hit,
	DoubleJump
};

class Player
{
public:
	Player(Vector2 startPos);
	~Player();

	int jumpsLeft;
	int maxJumps = 2;

	void Update(float dt, TileMap& map);
	void Draw();
	void DrawCollisionDebug();
	PlayerState GetState() const;
	
private:
	Vector2 position;
	Vector2 velocity;

	Vector2 hitboxSize;
	Vector2 spriteOffset;

	float moveInput = 0.0f;
	float acceleration = 200.0f;
	float gravity = 1200.0f;
	float jumpForce = 500.0f;
	

	float fallMultiplier = 1.8f;
	float jumpCutMultiplier = 0.5f;

	float coyoteTime = 0.1f;
	float coyoteTimer = 0.0f;

	float jumpBufferTime = 0.12f;
	float jumpBufferTimer = 0.0f;

	bool facingRight = true;
	bool isGrounded = false;

	bool jumpPressed = false;
	bool jumpReleased = false;
	bool attackPressed = false;

	PlayerState state = PlayerState::Idle;

	Animation idleAnim{};
	Animation runAnim{};
	Animation jumpAnim{};
	Animation fallAnim{};
	Animation hitAnim{};
	Animation doubleJumpAnim{};
	Animation* currentAnim = nullptr;

private:
	
	void HandleControls();
	void HandleJump(float dt);
	void ApplyGravity(float dt);
	void ApplyMovement(float dt, TileMap& map);
	
	void UpdateState();
	bool IsStateLocked() const;
	void ChangeState(PlayerState newState);
	Rectangle GetHitbox() const;
};