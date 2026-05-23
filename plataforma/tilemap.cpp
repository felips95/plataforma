#include "tilemap.h"
#include <sstream>

using namespace tinyxml2;

bool TileMap::Load(const std::string& file)
{
	XMLDocument doc;
	if (doc.LoadFile(file.c_str()) != XML_SUCCESS) return false;

	XMLElement* map = doc.FirstChildElement("map");
	if (!map) return false;

	map->QueryIntAttribute("width", &mapWidth);
	map->QueryIntAttribute("height", &mapHeight);
	map->QueryIntAttribute("tilewidth", &tileWidth);
	map->QueryIntAttribute("tileheight", &tileHeight);

	XMLElement* layerNode = map->FirstChildElement("layer");

	while (layerNode)
	{
		MapLayer newLayer;

		const char* nameAttr = layerNode->Attribute("name");
		if (nameAttr) newLayer.name = nameAttr;

		XMLElement* data = layerNode->FirstChildElement("data");
		if (data)
		{
			const char* csv = data->GetText();
			if (csv)
			{
				std::stringstream ss(csv);
				std::string tile;

				while (std::getline(ss, tile, ','))
				{
					newLayer.tiles.push_back(std::stoi(tile));
				}
			}
		}

		layers.push_back(newLayer);

		layerNode = layerNode->NextSiblingElement("layer");
	}

	return true;
}

void TileMap::Draw(Texture2D tileset)
{
	int columns = tileset.width / tileWidth;

	for (const auto& layer : layers)
	{
		if (layer.name == "Colisao") continue;

		for (int y = 0; y < mapHeight; y++)
		{
			for (int x = 0; x < mapWidth; x++)
			{
				int index = x + y * mapWidth;
				int tileID = layer.tiles[index];

				if (tileID == 0) continue;

				tileID--;

				int tileX = (tileID % columns) * tileWidth;
				int tileY = (tileID / columns) * tileHeight;

				Rectangle source = {
					(float)tileX,
					(float)tileY,
					(float)tileWidth,
					(float)tileHeight
				};

				Vector2 position = {
					(float)(x * tileWidth),
					(float)(y * tileHeight)
				};

				DrawTextureRec(tileset, source, position, WHITE);
			}
		}
	}
}

int TileMap::GetCollisionTile(int gridX, int gridY)
{
	if (gridX < 0 || gridX >= mapWidth || gridY < 0 || gridY >= mapHeight)
		return 0;

	int index = gridX + gridY * mapWidth;

	// Procura a camada chamada "Colisao"
	for (const auto& layer : layers)
	{
		if (layer.name == "Colisao")
		{
			return layer.tiles[index];
		}
	}
	return 0;
}

bool TileMap::CheckCollision(Rectangle hitbox)
{
	// Converte as extremidades do retângulo do jogador (em pixels) para índices da grade (grid)
	// Subtraímos 1 pixel na direita e embaixo para evitar colisão acidental com o tile vizinho
	int startX = (int)(hitbox.x / tileWidth);
	int endX = (int)((hitbox.x + hitbox.width - 0.1f) / tileWidth);
	int startY = (int)(hitbox.y / tileHeight);
	int endY = (int)((hitbox.y + hitbox.height - 0.1f) / tileHeight);

	// Verifica apenas os tiles que estão exatamente "encostando" na hitbox
	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			int tileID = GetCollisionTile(x, y);

			// Se o ID for maior que 0, significa que tem um bloco pintado na camada de Colisão
			if (tileID > 0)
			{
				return true;
			}
		}
	}

	return false;
}