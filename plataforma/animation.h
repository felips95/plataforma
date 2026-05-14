#pragma once
#include "raylib.h"

struct Animation
{
	Texture2D texture{};
	int framesTotal = 0;
	int frameAtual = 0;

	int frameWidth = 0;
	int frameHeight = 0;

	float frameTime = 0.0f;
	float timer = 0.0f;

	bool looping = true;
	bool finished = false;

	Rectangle frameRec{};
};

Animation CreateAnimation(const char* file, int frames, float frameTime, bool looping = true);
Rectangle GetSourceRect(const Animation& anim, bool facingRight);
void UpdateAnimation(Animation& anim, float dt, bool facingRight);
void UnloadAnimation(Animation& anim);