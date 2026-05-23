#pragma once

#include "raylib.h"
#include "tinyxml2.h"

#include <vector>
#include <string>

struct MapLayer
{
	std::string name;
	std::vector<int> tiles;
};

class TileMap
{
public:
	bool Load(const std::string& file);
	void Draw(Texture2D tileset);
	int GetCollisionTile(int gridX, int gridY);
	bool CheckCollision(Rectangle hitbox);

private:
	int mapWidth, mapHeight;
	int tileWidth, tileHeight;

	std::vector<MapLayer> layers;
};