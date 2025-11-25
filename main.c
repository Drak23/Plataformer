#include "raylib.h"
#include "animation.c"
#include "playercamera.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define NUM_SHOOTS 50

// --- Datos del juego ---
typedef struct Player {
    Vector2 pos;
    Vector2 vel;
    Rectangle box;
    bool onGround;
    int lives;
    int score;
} Player;

typedef struct Shoot{
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef struct Platform {
    Rectangle box;
} Platform;

typedef struct Enemy {
    Vector2 pos;
    Vector2 vel;
    Rectangle box;
    int dir;
    bool active;
} Enemy;

typedef struct Collectible {
    Vector2 pos;
    Rectangle box;
    bool taken;
} Collectible;

// ----------------------------------------------------------------
static bool gameOver = false;
static bool pauseGame = false;
static bool victory = false;
static Player player = { 0 };
static Shoot shoot[NUM_SHOOTS] = { 0 };
static int shootRate = 0;

#define MAX_PLATFORMS 64
#define MAX_ENEMIES 16
#define MAX_COLLECTIBLES 32

static Platform platforms[MAX_PLATFORMS];
static int platformCount = 0;

static Enemy enemies[MAX_ENEMIES];
static int enemyCount = 0;

static Collectible collectibles[MAX_COLLECTIBLES];
static int collectibleCount = 0;

static Camera2D camera;

const float GRAVITY = 1200.0f;
const float MOVE_SPEED = 240.0f;
const float JUMP_SPEED = 520.0f;
const float SHOOT_SPEED = 800.0f;

// -------------------- UTILIDADES -------------------------
Rectangle GetPlayerBox(Player *p) {
    return (Rectangle){p->pos.x - p->box.width/2.0f, p->pos.y - p->box.height, p->box.width, p->box.height};
}

bool CheckCollisionRectangles(Rectangle a, Rectangle b) {
    return !(a.x + a.width < b.x || a.x > b.x + b.width ||
             a.y + a.height < b.y || a.y > b.y + b.height);
}

void AddPlatform(float x, float y, float w, float h) {
    if (platformCount >= MAX_PLATFORMS) return;
    platforms[platformCount++].box = (Rectangle){x,y,w,h};
}

void AddEnemy(float x, float y) {
    if (enemyCount >= MAX_ENEMIES) return;

    enemies[enemyCount].pos = (Vector2){x,y};
    enemies[enemyCount].vel = (Vector2){40.0f,0};
    enemies[enemyCount].dir = -1;
    enemies[enemyCount].active = true;
    enemies[enemyCount].box = (Rectangle){x-16,y-32,32,32};
    enemyCount++;
}

void AddCollectible(float x, float y) {
    if (collectibleCount >= MAX_COLLECTIBLES) return;

    collectibles[collectibleCount].pos = (Vector2){x,y};
    collectibles[collectibleCount].box = (Rectangle){x-10,y-10,20,20};
    collectibles[collectibleCount].taken = false;
    collectibleCount++;
}

void InitShoots() {
    for (int i = 0; i < NUM_SHOOTS; i++) {
        shoot[i].rec = (Rectangle){0,0,10,5};
        shoot[i].speed = (Vector2){SHOOT_SPEED, 0};
        shoot[i].active = false;
        shoot[i].color = MAROON;
    }
}

// -------------------- RESET LEVEL -------------------------
void ResetLevel() {

    player.pos = (Vector2){120, 300};
    player.vel = (Vector2){0,0};
    player.box = (Rectangle){0,0,28,64};
    player.onGround = false;
    player.lives = 3;
    player.score = 0;

    camera.target = player.pos;
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    camera.rotation = 0;
    camera.zoom = 1.0f;

    platformCount = 0;
    enemyCount = 0;
    collectibleCount = 0;

    AddPlatform(-500, 420, 2000, 160);
    AddPlatform(400, 340, 200, 24);
    AddPlatform(700, 280, 160, 24);
    AddPlatform(920, 220, 120, 24);
    AddPlatform(1200, 300, 280, 24);
    AddPlatform(1560, 360, 100, 24);
    AddPlatform(1700, 260, 200, 24);
    AddPlatform(1960, 360, 240, 24);

    AddEnemy(680, 248);
    AddEnemy(1500, 232);
    AddEnemy(2000, 328);

    AddCollectible(460, 300);
    AddCollectible(730, 240);
    AddCollectible(1240, 260);
    AddCollectible(1720, 220);

    InitShoots();
    shootRate = 0;

    gameOver = false;
    victory = false;
    pauseGame = false;
}

// ----------------------- UPDATE -------------------------
void UpdatePlayer(float dt) {

    float move = 0;
    if (IsKeyDown(KEY_D)) move += 1;
    if (IsKeyDown(KEY_A)) move -= 1;

    player.vel.x = move * MOVE_SPEED;

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) && player.onGround) {
        player.vel.y = -JUMP_SPEED;
        player.onGround = false;
    }

    if (IsKeyDown(KEY_X)) {
        shootRate++;
        if (shootRate % 12 == 0) {
            for (int i=0; i<NUM_SHOOTS; i++) {
                if (!shoot[i].active) {
                    Rectangle box = GetPlayerBox(&player);
                    shoot[i].rec.x = box.x + box.width;
                    shoot[i].rec.y = box.y + box.height/2;
                    shoot[i].active = true;
                    break;
                }
            }
        }
    } else shootRate = 0;

    player.vel.y += GRAVITY * dt;

    player.pos.x += player.vel.x * dt;
    player.pos.y += player.vel.y * dt;

    player.box = GetPlayerBox(&player);

    player.onGround = false;

    for (int i=0;i<platformCount;i++) {

        Rectangle p = platforms[i].box;
        if (CheckCollisionRectangles(player.box, p)) {

            float prevY = player.pos.y - player.vel.y * dt;

            Rectangle prev = { player.box.x, prevY - player.box.height, player.box.width, player.box.height};

            if (prev.y + prev.height <= p.y + 1) {
                player.pos.y = p.y;
                player.vel.y = 0;
                player.onGround = true;
            }
        }
    }

    if (player.pos.y > 1400) {
        player.lives--;
        player.pos = (Vector2){120,300};
    }

    if (player.lives <= 0) gameOver = true;
}

void UpdateEnemies(float dt) { 
    for (int i=0;i<enemyCount;i++) { 
        if (!enemies[i].active) continue; 
        Enemy *e = &enemies[i]; 
        // simple patrol 
        e->pos.x += e->vel.x * e->dir * dt; 
        e->box.x = e->pos.x - e->box.width/2.0f; 
        e->box.y = e->pos.y - e->box.height; 
        // reverse when reaching edges of platform below 
        bool onPlatform = false; 
        for (int j=0;j<platformCount;j++) { 
            Rectangle pb = platforms[j].box; 
            if (e->pos.x > pb.x && e->pos.x < pb.x + pb.width && fabs(e->pos.y - pb.y) < 48) { 
                onPlatform = true; 
                break; 
            } 
        } 
        if (!onPlatform) e->dir *= -1; 
        // Collision with player 
        if (CheckCollisionRectangles(e->box, GetPlayerBox(&player))) { 
                // If player is falling and hits the enemy from above -> kill enemy 
                if (player.vel.y > 200.0f) { 
                e->active = false; player.vel.y = -JUMP_SPEED*0.4f; 
                // bounce 
                player.score += 100; 
            } else { 
            // Player damaged 
            player.lives -= 1; 
            player.pos = (Vector2){120,300}; 
            player.vel = (Vector2){0,0}; 
            } 
        } 
    } 
}

void UpdateCollectibles() {
    for (int i=0;i<collectibleCount;i++) {
        if (!collectibles[i].taken &&
            CheckCollisionRectangles(collectibles[i].box, GetPlayerBox(&player))) {
            collectibles[i].taken = true;
            player.score += 50;
        }
    }
}

void UpdateShoots(float dt) {

    for (int i=0;i<NUM_SHOOTS;i++) {
        if (!shoot[i].active) continue;

        shoot[i].rec.x += SHOOT_SPEED * dt;

        for (int e=0;e<enemyCount;e++) {
            if (!enemies[e].active) continue;

            if (CheckCollisionRectangles(shoot[i].rec, enemies[e].box)) {
                enemies[e].active = false;
                shoot[i].active = false;
                player.score += 100;
                break;
            }
        }
    }
}

// ------------------- MAIN -------------------------------
int main(void) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dragon Legends - Nivel 1");
    SetTargetFPS(60);

    LoadPlayerSprite();
    ResetLevel();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // Entradas: toggle PAUSA y reinicio
        if (IsKeyPressed(KEY_P)) pauseGame = !pauseGame;
        if (IsKeyPressed(KEY_R)) ResetLevel();

        // Si ya perdiste -> mostrar pantalla de GAME OVER y saltar la simulación
        if (gameOver) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 40, 40, RED);
            DrawText("Presiona R para reiniciar", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 10, 20, WHITE);
            EndDrawing();
            continue;
        }

        // Si ganaste -> mostrar pantalla de VICTORIA y saltar la simulación
        if (victory) {
            BeginDrawing();
            ClearBackground(WHITE);
            DrawText("¡VICTORIA!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 40, 40, DARKGREEN);
            DrawText("Presiona R para reiniciar", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 10, 20, BLACK);
            EndDrawing();
            continue;
        }

        // Si está pausado -> mostrar pantalla de pausa y saltar la lógica
        if (pauseGame) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PAUSA - Presiona P para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
            EndDrawing();
            continue;
        }

        // Actualizar solo si no está en pausa, gameOver ni victory
        if (!pauseGame && !gameOver && !victory) {
            UpdatePlayer(dt);
            UpdateEnemies(dt);
            UpdateCollectibles();
            UpdateShoots(dt);
        }

        // WIN CONDITION: Llegar a la META (x >= 2200)
        if (player.pos.x >= 2200 && !victory) {
            victory = true;
        }

        UpdatePlayerCamera(&camera, player.pos);

        // --- DIBUJO ---
        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode2D(camera);

        // Parallax background (very simple shapes)
        DrawRectangle(-1000, 100, 3000, 300, DARKBLUE);
        DrawRectangle(200, 80, 300, 200, PURPLE);
        DrawRectangle(800, 60, 280, 220, DARKPURPLE);

        // Ground and platforms
        for (int i=0;i<platformCount;i++) {
            Rectangle b = platforms[i].box;
            DrawRectangleRec(b, BROWN);
            DrawRectangleLinesEx(b, 2, BLACK);
        }

        // coleccionables
        for (int i=0;i<collectibleCount;i++) {
            if (!collectibles[i].taken)
                DrawCircleV(collectibles[i].pos, 8, GOLD);
        }

        // enemigos
        for (int i=0;i<enemyCount;i++) {
            if (enemies[i].active)
                DrawRectangleRec(enemies[i].box, RED);
        }

        // jugador (sprite)
        DrawPlayerSprite(GetPlayerBox(&player));

        // balas
        for (int i=0;i<NUM_SHOOTS;i++)
            if (shoot[i].active)
                DrawRectangleRec(shoot[i].rec, MAROON);

        // Level end marker (goal)
        DrawRectangle(2200, 0, 8, 800, GREEN);
        DrawText("META", 2190, -20, 20, BLACK);

        EndMode2D();

        DrawText(TextFormat("Vidas: %d", player.lives), 10, 10, 20, BLACK);
        DrawText(TextFormat("Puntaje: %d", player.score), 10, 40, 20, BLACK);

        EndDrawing();
    }

    UnloadPlayerSprite();
    CloseWindow();
    return 0;
}
