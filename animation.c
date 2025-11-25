//#include "animation.c"
#include "raylib.h"


Texture2D playerSprite;

void LoadPlayerSprite() {
    playerSprite = LoadTexture("resources/Free Street Animal Pixel Art/1 Dog/Idle.png");
}

void UnloadPlayerSprite() {
    UnloadTexture(playerSprite);
}

void DrawPlayerSprite(Rectangle dest) {
    Rectangle src = { 0, 0, 48, 48, (float)playerSprite.width, (float)playerSprite.height };
    DrawTexturePro(playerSprite, src, dest, (Vector2){0,0}, 0, WHITE);
}   
    