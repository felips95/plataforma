#include "animation.h"

Rectangle GetSourceRect(const Animation& anim, bool facingRight)
{
	float x = (float)(anim.frameAtual * anim.frameWidth);
	float w = (float)anim.frameWidth;

	if (!facingRight) { x += w; w = -w; }

	return { x, 0.0f, w, (float)anim.frameHeight };
}

Animation CreateAnimation(const char* file, int frames, float frameTime, bool looping)
{
	Animation anim;
	anim.texture = LoadTexture(file);
	anim.framesTotal = frames;
	anim.frameTime = frameTime;

	anim.frameWidth = anim.texture.width / frames;
	anim.frameHeight = anim.texture.height;

	anim.frameAtual = 0;
	anim.timer = 0.0f;

	anim.looping = looping;
	anim.finished = false;

	anim.frameRec = {
		0.0f,
		0.0f,
		(float)anim.frameWidth,
		(float)anim.frameHeight
	};
	return anim;
}

void UpdateAnimation(Animation& anim, float dt, bool facingRight)
{
	anim.timer += dt;

	// Usar 'while' em vez de 'if' garante que, se o PC travar e o dt 
	// for muito grande, a animação vai "compensar" os frames perdidos.
	while (anim.timer >= anim.frameTime && anim.frameTime > 0.0f)
	{
		// Subtrai o tempo do frame em vez de zerar, preservando os milissegundos restantes
		anim.timer -= anim.frameTime;
		anim.frameAtual++;

		// Checa se a animação acabou dentro do loop
		if (anim.frameAtual >= anim.framesTotal)
		{
			if (anim.looping)
			{
				anim.frameAtual = 0;
			}
			else
			{
				anim.frameAtual = anim.framesTotal - 1;
				anim.finished = true;
				anim.timer = 0.0f; // Para animações que não repetem, limpamos o timer
				break; // Sai do while
			}
		}
	}

	anim.frameRec = GetSourceRect(anim, facingRight);
}

void UnloadAnimation(Animation& anim)
{
	UnloadTexture(anim.texture);
}