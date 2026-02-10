#pragma once
#include "raylib.h"

enum class PlayerState
{
	Idle,
	Run
};

struct Animation
{
	//animação
	int frameWidth;
	int frameHeight;

	int framesTotal;
	int frameAtual = 0;

	float frameTime;
	float timer = 0.0f;

	Rectangle frameRec;
	Texture2D texture;
};

class Player
{
public:
	Player(Vector2 startPos);
	~Player();

	void draw();
	void update(float deltaTime, Rectangle collision);

private:
	//moviment
	Vector2 velocity;
	float acceleration = 500.0f;
	float gravity = 1200.0f;
	bool isGrounded = false;

	//position
	Vector2 position;
	Vector2 size;
	
	//player state
	PlayerState state = PlayerState::Idle;
	PlayerState previosState = PlayerState::Idle;

	Animation idleAnim;
	Animation walkAnim;
	Animation* currentAnim = nullptr;

	void UpdateAnimation();
	void ChangeState(PlayerState newState);
	void HandleControls();
};