#include "player.h"

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "meu joguinho");

	Player player({ (float)(windowWidth / 2), (float)(windowHeight / 2) });
	Rectangle ground = { 0.0f, (float)windowHeight - 100.0f, (float)windowWidth - 500.0f, 100.0f };

	while(!WindowShouldClose()) 
	{
		float deltaTime = GetFrameTime();
		player.Update(deltaTime, ground);

		BeginDrawing();
		ClearBackground(WHITE);

		DrawRectangleRec(ground, BROWN);
		player.Draw();
		player.DrawCollisionDebug();

		EndDrawing();
	}
	CloseWindow();
}