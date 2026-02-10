#include "player.h"

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "meu joguinho");

	Player player({ (float)(windowWidth / 2), (float)(windowHeight / 2) });
	Rectangle ground = { 0, windowHeight - 100, windowWidth - 500, 100 };

	while(!WindowShouldClose()) 
	{
		float deltaTime = GetFrameTime();
		player.update(deltaTime, ground);

		BeginDrawing();
		ClearBackground(WHITE);

		DrawRectangleRec(ground, BROWN);
		player.draw();

		EndDrawing();
	}
	CloseWindow();
}