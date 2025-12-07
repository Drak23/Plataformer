// menu_sprites.c
// Sprites usados en la pantalla de inicio

#include "raylib.h"

// Texturas globales del menú
Texture2D texLogo = {0};
Texture2D texButtonIdle = {0};
Texture2D texButtonSelected = {0};

// Cargar todos los sprites del menú
void LoadMenuSprites() {
    texLogo = LoadTexture("resources/Sprites/Menu/Logo.png");
    texButtonIdle = LoadTexture("resources/Sprites/Menu/PixelGUI/PixelGUI/layer3.png");
    texButtonSelected = LoadTexture("resources/Sprites/Menu/PixelGUI/PixelGUI/layer4.png");
}

// Descargar sprites del menú
void UnloadMenuSprites() {
    UnloadTexture(texLogo);
    UnloadTexture(texButtonIdle);
    UnloadTexture(texButtonSelected);
}

