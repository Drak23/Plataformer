// enemy_sprites.c
// Sistema de animación independiente para enemigos

#include "raylib.h"
#include <stdlib.h>

// ----------------------------------------------------------------------
// Animaciones globales del enemigo
// ----------------------------------------------------------------------
Animation enemyIdleAnim;
Animation enemyWalkAnim;
Animation enemyDeathAnim;

// ----------------------------------------------------------------------
// Cargar animaciones del enemigo
// ----------------------------------------------------------------------
void LoadEnemyAnimations() {

    // -------- IDLE --------
    enemyIdleAnim.texture = LoadTexture("resources/Sprites/Enemigos/Enemigo/Mushroom-Idle.png");
    enemyIdleAnim.frameCount = 7;
    enemyIdleAnim.frameSpeed = 8;
    enemyIdleAnim.currentFrame = 0;
    enemyIdleAnim.timer = 0;
    enemyIdleAnim.frameWidth  = enemyIdleAnim.texture.width / enemyIdleAnim.frameCount;
    enemyIdleAnim.frameHeight = enemyIdleAnim.texture.height;

    // -------- WALK --------
    enemyWalkAnim.texture = LoadTexture("resources/Sprites/Enemigos/Enemigo/Mushroom-Run.png");
    enemyWalkAnim.frameCount = 8;
    enemyWalkAnim.frameSpeed = 10;
    enemyWalkAnim.currentFrame = 0;
    enemyWalkAnim.timer = 0;
    enemyWalkAnim.frameWidth  = enemyWalkAnim.texture.width / enemyWalkAnim.frameCount;
    enemyWalkAnim.frameHeight = enemyWalkAnim.texture.height;

    // -------- DEATH --------
    enemyDeathAnim.texture = LoadTexture("resources/Sprites/Enemigos/Enemigo/Mushroom-Die.png");
    enemyDeathAnim.frameCount = 15;
    enemyDeathAnim.frameSpeed = 8;
    enemyDeathAnim.currentFrame = 0;
    enemyDeathAnim.timer = 0;
    enemyDeathAnim.frameWidth  = enemyDeathAnim.texture.width / enemyDeathAnim.frameCount;
    enemyDeathAnim.frameHeight = enemyDeathAnim.texture.height;
}

// ----------------------------------------------------------------------
// Liberar memoria
// ----------------------------------------------------------------------
void UnloadEnemySprites() {
    UnloadTexture(enemyIdleAnim.texture);
    UnloadTexture(enemyWalkAnim.texture);
    UnloadTexture(enemyDeathAnim.texture);
}

// ----------------------------------------------------------------------
// Actualizar animación
// ----------------------------------------------------------------------
void UpdateEnemyAnimation(Animation *anim, float dt) {

    anim->timer += dt;

    if (anim->timer >= (1.0f / anim->frameSpeed)) {
        anim->timer = 0;
        anim->currentFrame++;

        if (anim->currentFrame >= anim->frameCount)
            anim->currentFrame = 0;
    }
}

// Actualizar todas
void UpdateAllEnemyAnimations(float dt) {
    UpdateEnemyAnimation(&enemyIdleAnim, dt);
    UpdateEnemyAnimation(&enemyWalkAnim, dt);
    UpdateEnemyAnimation(&enemyDeathAnim, dt);
}

// ----------------------------------------------------------------------
// Dibujo del enemigo
// tipoAnim = 0 idle, 1 walk, 2 death
// dir = 1 derecha, -1 izquierda
// ----------------------------------------------------------------------
void DrawEnemyAnimation(Vector2 pos, int dir, int tipoAnim) {

    Animation * anim =
        (tipoAnim == 0) ? &enemyIdleAnim :
        (tipoAnim == 1) ? &enemyWalkAnim :
                          &enemyDeathAnim;

    Rectangle src = {
        anim->frameWidth * anim->currentFrame,
        0,
        anim->frameWidth * dir,  // flip horizontal si dir = -1
        anim->frameHeight
    };

    Rectangle dst = {
        pos.x - anim->frameWidth / 2,
        pos.y - anim->frameHeight,
        anim->frameWidth,
        anim->frameHeight
    };

    DrawTexturePro(anim->texture, src, dst, (Vector2){0,0}, 0, WHITE);
}