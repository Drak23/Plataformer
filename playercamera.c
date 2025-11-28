// playercamera.c
// Control simple de la cámara 2D que sigue al jugador

#include "raylib.h"

// ---------------------------------------------------------------------
// UpdatePlayerCamera
//
// Mantiene la cámara centrada en el jugador,
// aplicando pequeños límites mínimos para evitar mostrar espacio vacío.
// ---------------------------------------------------------------------
void UpdatePlayerCamera(Camera2D *camera, Vector2 playerPos) {

    // Seguir al jugador
    camera->target = playerPos;

    // Limites mínimos para evitar mostrar pantalla fuera del mundo
    if (camera->target.x < 256)
        camera->target.x = 256;

    if (camera->target.y < 192)
        camera->target.y = 192;
}
