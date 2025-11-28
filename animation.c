// animation.c
// Control de animaciones del jugador
// -----------------------------------
// Carga de sprites, animación por frames,
// actualización por delta time y dibujo con flip.

#include "raylib.h"
#include <stdlib.h>

// ----------------------------------------------------------------------
// ESTRUCTURA DE ANIMACIÓN
// ----------------------------------------------------------------------
typedef struct {
    Texture2D texture;      // Sprite sheet completo
    int frameCount;         // Número de frames
    int currentFrame;       // Frame actual
    float frameSpeed;       // Frames por segundo
    float timer;            // Acumulador de tiempo
    int frameWidth;         // Ancho de un frame
    int frameHeight;        // Alto de un frame
} Animation;

// Animaciones globales
Animation idleAnim;
Animation walkAnim;
Animation deathAnim;

// Estado del jugador
typedef enum {
    PLAYER_IDLE = 0,
    PLAYER_WALK,
    PLAYER_DEATH
} PlayerState;

PlayerState playerState = PLAYER_IDLE;

// Dirección del jugador (true = derecha, false = izquierda)
bool facingRight = true;

// ----------------------------------------------------------------------
// Cargar animaciones del jugador
// ----------------------------------------------------------------------
void LoadPlayerAnimations() {

    // -------- IDLE --------
    idleAnim.texture = LoadTexture("resources/Sprites/Personaje/1 Dog/Idle.png");
    idleAnim.frameCount = 4;
    idleAnim.frameSpeed = 8;
    idleAnim.currentFrame = 0;
    idleAnim.timer = 0;
    idleAnim.frameWidth = idleAnim.texture.width / idleAnim.frameCount;
    idleAnim.frameHeight = idleAnim.texture.height;

    // -------- WALK --------
    walkAnim.texture = LoadTexture("resources/Sprites/Personaje/1 Dog/Walk.png");
    walkAnim.frameCount = 6;
    walkAnim.frameSpeed = 10;
    walkAnim.currentFrame = 0;
    walkAnim.timer = 0;
    walkAnim.frameWidth = walkAnim.texture.width / walkAnim.frameCount;
    walkAnim.frameHeight = walkAnim.texture.height;

    // -------- DEATH --------
    deathAnim.texture = LoadTexture("resources/Sprites/Personaje/1 Dog/Death.png");
    deathAnim.frameCount = 4;
    deathAnim.frameSpeed = 8;
    deathAnim.currentFrame = 0;
    deathAnim.timer = 0;
    deathAnim.frameWidth = deathAnim.texture.width / deathAnim.frameCount;
    deathAnim.frameHeight = deathAnim.texture.height;
}

// ----------------------------------------------------------------------
// Liberar memoria de animaciones
// ----------------------------------------------------------------------
void UnloadPlayerSprites() {
    UnloadTexture(idleAnim.texture);
    UnloadTexture(walkAnim.texture);
    UnloadTexture(deathAnim.texture);
}

// ----------------------------------------------------------------------
// Actualizar animación
// ----------------------------------------------------------------------
void UpdateAnimation(Animation *anim, float delta) {

    anim->timer += delta;

    if (anim->timer >= 1.0f / anim->frameSpeed) {
        anim->timer = 0;
        anim->currentFrame++;

        if (anim->currentFrame >= anim->frameCount) {
            anim->currentFrame = 0;     // Loop
        }
    }
}

// ----------------------------------------------------------------------
// Reiniciar animación desde frame 0
// ----------------------------------------------------------------------
void ResetAnimation(Animation *anim) {
    anim->currentFrame = 0;
    anim->timer = 0;
}

// ----------------------------------------------------------------------
// Saber si animación terminó (para muerte)
// ----------------------------------------------------------------------
bool IsAnimationFinished(Animation *anim) {
    return (anim->currentFrame == anim->frameCount - 1);
}

// ----------------------------------------------------------------------
// Dibujo genérico de animación
// ----------------------------------------------------------------------
void DrawAnimation(Animation *anim, Vector2 pos) {

    Rectangle src = {
        anim->frameWidth * anim->currentFrame,
        0,
        anim->frameWidth,
        anim->frameHeight
    };

    // Si mira a la izquierda, dar vuelta al sprite
    if (!facingRight)
        src.width = -anim->frameWidth;

    Rectangle dest = {
        pos.x - anim->frameWidth/2,
        pos.y - anim->frameHeight,
        anim->frameWidth,
        anim->frameHeight
    };

    DrawTexturePro(anim->texture, src, dest, (Vector2){0,0}, 0, WHITE);
}

// ----------------------------------------------------------------------
// Dibujo de la animación correcta según el estado del jugador
// ----------------------------------------------------------------------
void DrawPlayer(Vector2 pos)
{
    Animation *animToDraw = NULL;

    switch (playerState) {
        case PLAYER_IDLE:  animToDraw = &idleAnim;  break;
        case PLAYER_WALK:  animToDraw = &walkAnim;  break;
        case PLAYER_DEATH: animToDraw = &deathAnim; break;
    }

    Rectangle src = {
        animToDraw->frameWidth * animToDraw->currentFrame,
        0,
        animToDraw->frameWidth,
        animToDraw->frameHeight
    };

    if (!facingRight)
        src.width = -animToDraw->frameWidth;

    Rectangle dest = {
        pos.x - animToDraw->frameWidth / 2,
        pos.y - animToDraw->frameHeight,
        animToDraw->frameWidth,
        animToDraw->frameHeight
    };

    DrawTexturePro(animToDraw->texture, src, dest, (Vector2){0,0}, 0.0f, WHITE);
}
