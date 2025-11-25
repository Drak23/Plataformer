//#include "playercamera.c"
#include "raylib.h"

void UpdatePlayerCamera(Camera2D *camera, Vector2 playerPos) {

    camera->target = playerPos;

    if (camera->target.x < 256) camera->target.x = 256;
    if (camera->target.y < 192) camera->target.y = 192;
}
