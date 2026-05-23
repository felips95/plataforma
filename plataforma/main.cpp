#include "player.h"
#include "tilemap.h"

int main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	InitWindow(windowWidth, windowHeight, "meu joguinho");
	SetTargetFPS(144);

	Player player({80,80});

	TileMap map;
	map.Load("assets/mapa.tmx");
	Texture2D tileset = LoadTexture("assets/terrain.png");

	while(!WindowShouldClose()) 
	{
		float deltaTime = GetFrameTime();
		player.Update(deltaTime, map);

		BeginDrawing();
		ClearBackground(SKYBLUE);

		map.Draw(tileset);
		player.Draw();
		player.DrawCollisionDebug();

		//Text
		DrawText(TextFormat("State: %d", (int)player.GetState()), 60, 60, 20, BLACK);
		DrawText(TextFormat("Jump: %d", player.jumpsLeft), 60, 80, 20, BLACK);

		EndDrawing();
	}
	UnloadTexture(tileset);
	CloseWindow();
	return 0;
}