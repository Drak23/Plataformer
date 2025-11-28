// menu_sprites.c
// Sprites usados en la pantalla de inicio

#include "raylib.h"

// Texturas globales del menú
Texture2D texLogo = {0};
Texture2D texButtonIdle = {0};
Texture2D texButtonSelected = {0};

// Cargar todos los sprites del menú
void LoadMenuSprites() {
    texLogo = LoadTexture("resources/Sprites/Menu/logo.png");
    texButtonIdle = LoadTexture("resources/Sprites/Menu/button_idle.png");
    texButtonSelected = LoadTexture("resources/Sprites/Menu/button_selected.png");
}

// Descargar sprites del menú
void UnloadMenuSprites() {
    UnloadTexture(texLogo);
    UnloadTexture(texButtonIdle);
    UnloadTexture(texButtonSelected);
}

