// main.c
// Juego plataforma 2D con menú, música, sprites separados y animación de personaje.
// ARREGLADO + Animación independiente de enemigos

#include "raylib.h"

// ------------------------------
// INCLUSIÓN DE MÓDULOS .C
// ------------------------------
#include "animation.c"          // Animación del jugador
#include "enemy_sprites.c"    // Animación independiente de enemigos
#include "playercamera.c"

#include "menu_sprites.c"       // Sprites del menú
#include "music.c"              // Música del menú y juego

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(PLATFORM_DESKTOP)
   
#endif

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define NUM_SHOOTS 50
#define MAX_PLATFORMS 64
#define MAX_ENEMIES 16
#define MAX_COLLECTIBLES 32

// ------------------------------
// Estructuras de datos
// ------------------------------
typedef struct Player {
    Vector2 pos;
    Vector2 vel;
    Rectangle box;
    bool onGround;
    int lives;
    int score;
} Player;

typedef struct Shoot {
    Rectangle rec;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef struct Platform {
    Rectangle box;
    int type;      // 0..2 plataformas, 3 = suelo
} Platform;

typedef struct Enemy {
    Vector2 pos;
    Vector2 vel;
    Rectangle box;
    int dir;        // 1 derecha, -1 izquierda
    bool active;
} Enemy;

typedef struct Collectible {
    Vector2 pos;
    Rectangle box;
    bool taken;
} Collectible;

// ------------------------------
// Variables Globales
// ------------------------------
static Player player;
static Shoot shoot[NUM_SHOOTS] = { 0 };
static Platform platforms[MAX_PLATFORMS];
static Enemy enemies[MAX_ENEMIES];
static Collectible collectibles[MAX_COLLECTIBLES];
static int shootRate = 0;

static int platformCount = 0;
static int enemyCount = 0;
static int collectibleCount = 0;

static Camera2D camera;

static bool gameOver = false;
static bool victory = false;
static bool pauseGame = false;

const float GRAVITY = 1200.0f;
const float MOVE_SPEED = 240.0f;
const float JUMP_SPEED = 520.0f;
const float SHOOT_SPEED = 800.0f;

// Texturas de plataformas y suelo
static Texture2D platformTex[3];
static Texture2D groundTex;

// ------------------------------
// Menú (pantallas)
// ------------------------------
typedef enum { LOGO = 0, TITLE, CREDITS, CONTROLES, GAMEPLAY, ENDING } GameScreen;
static int currentScreen = TITLE;

static int optionSelect = 0;


// ------------------------------
// Prototipos
// ------------------------------
Rectangle GetPlayerBox(Player *p);
bool CheckCollisionRectangles(Rectangle a, Rectangle b);
void AddPlatform(float x, float y, float w, float h, int type);
void AddEnemy(float x, float y);
void AddCollectible(float x, float y);
void InitShoots(void);
void ResetLevel(void);

void UpdatePlayer(float dt);
void UpdateEnemies(float dt);
void UpdateCollectibles(void);
void UpdateShoots(float dt);

void LoadResources(void);
void UnloadResources(void);


// ------------------------------
// Utilidades
// ------------------------------
Rectangle GetPlayerBox(Player *p) {
    return (Rectangle){
        p->pos.x - p->box.width/2,
        p->pos.y - p->box.height,
        p->box.width,
        p->box.height
    };
}

bool CheckCollisionRectangles(Rectangle a, Rectangle b) {
    return !(a.x + a.width < b.x ||
             a.x > b.x + b.width ||
             a.y + a.height < b.y ||
             a.y > b.y + b.height);
}

void AddPlatform(float x, float y, float w, float h, int type) {
    platforms[platformCount].box = (Rectangle){x,y,w,h};
    platforms[platformCount].type = type;
    platformCount++;
}

void AddEnemy(float x, float y) {
    enemies[enemyCount].pos = (Vector2){x,y};
    enemies[enemyCount].vel = (Vector2){40,0};
    enemies[enemyCount].dir = -1;
    enemies[enemyCount].box = (Rectangle){x-16,y-32,32,32};
    enemies[enemyCount].active = true;
    enemyCount++;
}

void AddCollectible(float x, float y) {
    collectibles[collectibleCount].pos = (Vector2){x,y};
    collectibles[collectibleCount].box = (Rectangle){x-8,y-8,16,16};
    collectibles[collectibleCount].taken = false;
    collectibleCount++;
}

void InitShoots() {
    for (int i=0;i<NUM_SHOOTS;i++) {
        shoot[i].rec = (Rectangle){0,0,10,5};
        shoot[i].speed = (Vector2){SHOOT_SPEED,0};
        shoot[i].active = false;
        shoot[i].color = MAROON;
    }
}


// ------------------------------
// Cargar todos los recursos
// ------------------------------
void LoadResources() {

    LoadMenuSprites();
    LoadEnemyAnimations();

    LoadMusicTracks();
    PlayMenuMusic();

    platformTex[0] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[1] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[2] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[3] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[4] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[5] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[6] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[7] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[8] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[9] = LoadTexture("resources/Sprites/Plataformas/suelo.png");
    platformTex[10] = LoadTexture("resources/Sprites/Plataformas/suelo.png");

    //Fondo nivel
    groundTex = LoadTexture("resources/Sprites/Plataformas/cielo.png");

    LoadPlayerAnimations();
    LoadEnemyAnimations(); // ← NUEVO
}


// ------------------------------
// Descargar recursos
// ------------------------------
void UnloadResources() {
    UnloadMenuSprites();
    LoadEnemyAnimations();
    UnloadMusicTracks();

    UnloadTexture(platformTex[0]);
    UnloadTexture(platformTex[1]);
    UnloadTexture(platformTex[2]);
    UnloadTexture(platformTex[3]);
    UnloadTexture(platformTex[4]);
    UnloadTexture(platformTex[5]);
    UnloadTexture(platformTex[6]);
    UnloadTexture(platformTex[7]);
    UnloadTexture(platformTex[8]);
    UnloadTexture(platformTex[9]);
    UnloadTexture(platformTex[10]);

    UnloadTexture(groundTex);

    UnloadPlayerSprites();
    UnloadEnemySprites(); // ← NUEVO
}


// ------------------------------
// Reset del nivel
// ------------------------------
void ResetLevel() {

    player.pos = (Vector2){120,300};
    player.vel = (Vector2){0,0};
    player.box = (Rectangle){0,0,28,64};
    player.lives = 3;
    player.score = 0;
    player.onGround = false;

    camera.target = player.pos;
    camera.offset = (Vector2){SCREEN_WIDTH/2, 450};
    camera.zoom = 1.5f;

    platformCount = 0;
    enemyCount = 0;
    collectibleCount = 0;

    //suelo
    AddPlatform(10,420,900,1600,4);
    AddPlatform(1620,420,600,1600,4);
    AddPlatform(2620,420,600,1600,4); 
    AddPlatform(3920,420,600,1600,4); 

    //AddPlatform(340,420,20,30,5);

    //Plataformas(x,y,z,ancho,numero de plataforma)
    AddPlatform(400,340,200,24,0);  //1
    AddPlatform(700,280,160,24,1);  //2
    AddPlatform(920,220,120,24,2);  //3
    AddPlatform(1217,290,125,24,3); //4
    AddPlatform(1520,230,120,24,5); //5
    AddPlatform(2020,330,120,24,6); //6
    AddPlatform(2300,250,120,24,7); //7
    AddPlatform(3300,350,120,24,8); //8
    AddPlatform(3500,260,120,24,9); //9
    AddPlatform(3700,160,120,24,10); //10

    //Enemigos
    AddEnemy(1620,420);  // suelo 2
    AddEnemy(1690,420);  // suelo 2
    AddEnemy(2640,420);  // suelo 3
    AddEnemy(2690,420);  // suelo 3
    AddEnemy(3920,420);  // suelo 4
    AddEnemy(3990,420);  // suelo 4
    AddEnemy(780,280);   // Enemigo 1 (plataforma 2)
    AddEnemy(980,220);   // Enemigo 2 (plataforma 3)
    AddEnemy(1280,290);  // Enemigo 3(plataforma 4)

    AddCollectible(460,300);
    AddCollectible(730,240);

    InitShoots();
    shootRate = 0;

    gameOver = false;
    victory = false;
    pauseGame = false;
}


// ------------------------------
// Actualizar jugador
// ------------------------------
void UpdatePlayer(float dt) {

    bool controller = IsGamepadAvailable(0);

    float move = 0;

    // MOVIMIENTO → ANIMACIÓN
    if (IsKeyDown(KEY_D)) {
        move += 1;
        facingRight = true;
        playerState = PLAYER_WALK;
    }
    else if (IsKeyDown(KEY_A)) {
        move -= 1;
        facingRight = false;
        playerState = PLAYER_WALK;
    }
    else {
        playerState = PLAYER_IDLE;
    }
        // --- Movimiento con Gamepad ---
    if (controller) {
        float axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);

            if (axis > 0.2f) {
            move = 1;
            facingRight = true;
        }
        else if (axis < -0.2f) {
            move = -1;
            facingRight = false;
        }
    }

    // ---------------------
    // DEFINIR ESTADO DE ANIMACIÓN
    // ---------------------
    if (player.lives <= 0) {
        playerState = PLAYER_DEATH;
    }
    else if (move != 0) {
        playerState = PLAYER_WALK;
    }
    else {
        playerState = PLAYER_IDLE;
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z) ||
        (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
        && player.onGround) 
    {
        player.vel.y = -JUMP_SPEED;
        player.onGround = false;
    }


    // Actualizar animación según estado
    switch (playerState) {
        case PLAYER_IDLE:  UpdateAnimation(&idleAnim, dt);  break;
        case PLAYER_WALK:  UpdateAnimation(&walkAnim, dt);  break;
        case PLAYER_DEATH: UpdateAnimation(&deathAnim, dt); break;
    }

    player.vel.x = move * MOVE_SPEED;

     // Si muere
    if (player.lives <= 0 && playerState != PLAYER_DEATH) {
        playerState = PLAYER_DEATH;
        ResetAnimation(&deathAnim);
    }

      // Disparar
    if (IsKeyPressed(KEY_RIGHT) ||
        (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)))
    {
        for (int i=0;i<NUM_SHOOTS;i++) {
            if (!shoot[i].active) {
                Rectangle box = GetPlayerBox(&player);
                shoot[i].rec.x = box.x + box.width;
                shoot[i].rec.y = box.y + box.height/2;
                shoot[i].active = true;
                break;
            }
        }
    } else shootRate = 0;
    

    // FÍSICAS
    player.vel.y += GRAVITY * dt;
    player.pos.x += player.vel.x * dt;
    player.pos.y += player.vel.y * dt;

    player.box = GetPlayerBox(&player);
    player.onGround = false;

    // COLISIONES
    for (int i=0;i<platformCount;i++) {

        Rectangle p = platforms[i].box;

        if (CheckCollisionRectangles(player.box, p)) {
            float prevY = player.pos.y - player.vel.y * dt;

            if (prevY <= p.y) {
                player.pos.y = p.y;
                player.vel.y = 0;
                player.onGround = true;
            }
        }
    }

    // Caer al vacío
    if (player.pos.y > 1400) {
        player.lives--;
        player.pos = (Vector2){120,300};
    }

    if (player.lives <= 0) {
        gameOver = true;
    }
    
    // GAME OVER al terminar animación de muerte
    if (player.lives <= 0) {
        playerState = PLAYER_DEATH;

        if (IsAnimationFinished(&deathAnim)) {
            gameOver = true;
        }
    }
}


// ------------------------------
// Actualizar enemigos (MOVIMIENTO + ANIMACIÓN)
// ------------------------------
void UpdateEnemies(float dt) {

    UpdateAllEnemyAnimations(dt); // ← animaciones

    for (int i = 0; i < enemyCount; i++) {
        Enemy *e = &enemies[i];
        if (!e->active) continue;

        // velocidad horizontal prevista
        float stepX = e->vel.x * e->dir * dt;
        float nextX = e->pos.x + stepX;

        // actualizar caja según pos actual (antes de mover)
        e->box.x = e->pos.x - e->box.width / 2.0f;
        e->box.y = e->pos.y - e->box.height;

        // Buscar la plataforma actual (si existe) en la que está apoyado el enemigo
        int currentPlat = -1;
        for (int j = 0; j < platformCount; j++) {
            Rectangle pb = platforms[j].box;
            // Consideramos que el enemigo está "sobre" la plataforma si su X está dentro
            // y su Y está cerca de la Y de la plataforma.
            if (e->pos.x >= pb.x && e->pos.x <= pb.x + pb.width) {
                float dy = e->pos.y - pb.y;
                if (dy >= -8.0f && dy <= 48.0f) { // tolerancia vertical razonable
                    currentPlat = j;
                    break;
                }
            }
        }

        // Si está sobre una plataforma, fijar su Y a la plataforma (evita desalineaciones)
        if (currentPlat >= 0) {
            Rectangle pb = platforms[currentPlat].box;
            e->pos.y = pb.y;
            e->box.y = e->pos.y - e->box.height;
        }

        // Decidir si avanzar o girar:
        bool shouldTurn = false;

        if (currentPlat >= 0) {
            // Si avanza, ¿se saldría del ancho de la plataforma?
            Rectangle pb = platforms[currentPlat].box;
            float projectedFeetX = nextX; // usamos la X del "centro" del enemigo
            // margen para evitar "pegado" al borde:
            const float EDGE_MARGIN = 6.0f;

            if (projectedFeetX < pb.x + EDGE_MARGIN || projectedFeetX > pb.x + pb.width - EDGE_MARGIN) {
                shouldTurn = true;
            } else {
                // además comprobamos colisiones laterales con el "volumen" de la plataforma (paredes)
                Rectangle nextBox = e->box;
                nextBox.x = nextX - e->box.width / 2.0f;

                // pared izquierda
                if (CheckCollisionRectangles(nextBox, (Rectangle){pb.x - 2.0f, pb.y - pb.height, 2.0f, pb.height})) {
                    shouldTurn = true;
                }
                // pared derecha
                if (CheckCollisionRectangles(nextBox, (Rectangle){pb.x + pb.width, pb.y - pb.height, 2.0f, pb.height})) {
                    shouldTurn = true;
                }
            }
        } else {
            // Si no está sobre ninguna plataforma: buscar si hay plataforma justo delante (paso en vacío)
            bool platformAhead = false;
            for (int j = 0; j < platformCount; j++) {
                Rectangle pb = platforms[j].box;
                // ver si la X prevista cae dentro de alguna plataforma y la Y es coherente
                if (nextX >= pb.x && nextX <= pb.x + pb.width) {
                    float dy = e->pos.y - pb.y;
                    if (dy >= -16.0f && dy <= 48.0f) {
                        platformAhead = true;
                        break;
                    }
                }
            }
            // si no hay plataforma adelante, girar
            if (!platformAhead) shouldTurn = true;
        }

        if (shouldTurn) {
            e->dir *= -1;
            // ajustar posición para evitar quedar 'pegado' fuera del borde
            e->pos.x += e->vel.x * e->dir * dt;
        } else {
            // avanzar
            e->pos.x = nextX;
        }

        // actualizar caja final
        e->box.x = e->pos.x - e->box.width / 2.0f;
        e->box.y = e->pos.y - e->box.height;

        // Colisión con jugador
        if (CheckCollisionRectangles(e->box, GetPlayerBox(&player))) {
            if (player.vel.y > 200.0f) {
                // jugador salta encima -> enemigo muere
                e->active = false;
                player.vel.y = -JUMP_SPEED * 0.4f;
                player.score += 100;
            } else {
                // daño al jugador
                player.lives--;
                player.pos = (Vector2){120,300};
                player.vel = (Vector2){0,0};
            }
        }
    }
}




// ------------------------------
// Actualizar objetos
// ------------------------------
void UpdateCollectibles() {
    for (int i=0;i<collectibleCount;i++) {
        if (!collectibles[i].taken &&
            CheckCollisionRectangles(collectibles[i].box, GetPlayerBox(&player))) {
            collectibles[i].taken = true;
            player.score += 50;
        }
    }
}


// ------------------------------
// Disparos
// ------------------------------
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
            }
        }
    }
}



// ------------------------------
// MAIN
// ------------------------------
int main(void) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dragon Legends");
    SetTargetFPS(60);

    LoadResources();
    ResetLevel();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        UpdateAllMusic();

        // ------------------------------
        // PANTALLA DE MENÚ
        // ------------------------------
        if (currentScreen == TITLE) {

            bool controller = IsGamepadAvailable(0);

            // ----------------------------
            // MOVER CURSOR DEL MENÚ
            // ----------------------------

            // TECLADO
            if (IsKeyPressed(KEY_DOWN)) optionSelect++;
            if (IsKeyPressed(KEY_UP)) optionSelect--;

            // GAMEPAD – D-PAD
            if (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
                optionSelect++;

            if (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
                optionSelect--;

            
            if (optionSelect < 0) optionSelect = 0;
            if (optionSelect > 3) optionSelect = 3;

            if (IsKeyPressed(KEY_ENTER) ||
                (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) 
            {

                if (optionSelect == 0) {
                    ResetLevel();  
                    currentScreen = GAMEPLAY;
                    PlayGameMusic();
                }
                else if (optionSelect == 1) {
                    currentScreen = CONTROLES;
                    PlayGameMusic();
                }
                else if (optionSelect == 2)
                    currentScreen = CREDITS;
                else if (optionSelect == 3)
                    break;
            }
        

            BeginDrawing();
            ClearBackground(WHITE);

            // Logo
            float escala = 0.45f;   // Ajusta este valor si lo quieres más grande o pequeño

            int logoAncho = texLogo.width * escala;
            int logoAlto  = texLogo.height * escala;

            int posX = (SCREEN_WIDTH - logoAncho) / 2;
            int posY = -60;

            DrawTexturePro(
                texLogo,
                (Rectangle){ 0, 0, texLogo.width, texLogo.height },    // origen
                (Rectangle){ posX, posY, logoAncho, logoAlto },        // destino escalado
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            // Botones
            int bx = SCREEN_WIDTH/2 - texButtonIdle.width/7;
            int by = 310;

            for (int i=0;i<4;i++) {

                Texture2D *btn = (i==optionSelect)
                    ? &texButtonSelected
                    : &texButtonIdle;

                DrawTexture(*btn, bx, by + i*60, WHITE);

                const char *label =
                    (i==0) ? "JUGAR" :
                    (i==1) ? "CONTROLES" :
                    (i==2) ? "CREDITOS" :
                             "SALIR";

                DrawText(label, bx + 80, by + i*60 + 20, 24, BLACK);
            }

            EndDrawing();
            continue;
        }

        // ------------------------------
        // Controles
        // ------------------------------
        if (currentScreen == CONTROLES) {

            bool controller = IsGamepadAvailable(0);

            if (IsKeyPressed(KEY_ENTER)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
            {
                currentScreen = GAMEPLAY;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("CONTROLES", 420, 80, 40, DARKBLUE);
            DrawText("Controles Mando: joysticks(Movimiento), B/0(Disparo), \nA/X(Saltar), =(Pausa), X/O(Reiniciar Nivel), Boton Vista(Menu)", 150, 180, 24, BLACK);
            DrawText("Controles Teclado: A(Izquierda), D(Derecha), ->(Disparo), \nSPACE(Saltar), P(pausa), R(Reiniciar Nivel), Tab(Menu)", 150, 280, 24, BLACK);
            if (controller){
            DrawText("Presiona A/X para continuar", SCREEN_WIDTH/2 - 220, 510, 20, GREEN);
            DrawText("Presiona B/0 para continuar", SCREEN_WIDTH/2 - 220, 540, 20, RED);
            }
            else{
            DrawText("Presiona Enter para continuar", SCREEN_WIDTH/2 - 220, 510, 20, GREEN);
            DrawText("Presiona Backspace para continuar", SCREEN_WIDTH/2 - 220, 540, 20, RED);
            }
            if (IsKeyPressed(KEY_BACKSPACE)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)))
            {
                currentScreen = TITLE;
            }
            EndDrawing();
            continue;
        }

        // ------------------------------
        // CRÉDITOS
        // ------------------------------
        if (currentScreen == CREDITS) {

            bool controller = IsGamepadAvailable(0);

            if (IsKeyPressed(KEY_ENTER)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))
            {
                currentScreen = TITLE;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("CREDITOS", 420, 80, 40, DARKBLUE);
            DrawText("Desarrollador: Diego ", 350, 180, 24, BLACK);
            if (controller){
            DrawText("Boton Vista para regresar", 360, 500, 20, GRAY);
            }
            else{
            DrawText("ENTER para regresar", 360, 500, 20, GRAY);
            }

            EndDrawing();
            continue;
        }

        // ------------------------------
        // GAMEPLAY
        // ------------------------------
        if (currentScreen == GAMEPLAY) {
            

            bool controller = IsGamepadAvailable(0);

            if (IsKeyPressed(KEY_P) || (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))
            {
                pauseGame = !pauseGame;
            }

            if (IsKeyPressed(KEY_R)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))) ResetLevel();
            

            if (!pauseGame && !gameOver && !victory) {
                UpdatePlayer(dt);
                UpdateEnemies(dt);
                UpdateCollectibles();
                UpdateShoots(dt);
            }
            if (IsKeyPressed(KEY_TAB)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)))
            {
                currentScreen = TITLE;
            }
            if (IsKeyPressed(KEY_BACKSPACE)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)))
            {
                currentScreen = GAMEPLAY;
            }

            // GAME OVER SCREEN
            if (gameOver) {
                
                bool controller = IsGamepadAvailable(0);

                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 60, 40, RED);
                if (controller){
                DrawText("Reiniciar - Presiona X para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("Boton Vista para regresar al menu", 360, 500, 20, GRAY);
                }
                else{
                DrawText("Reiniciar - Presiona R para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("ENTER para regresar al menu", 360, 500, 20, GRAY);
                }
                if (IsKeyPressed(KEY_ENTER)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))
                {
                    currentScreen = TITLE;
                }
                if (IsKeyPressed(KEY_R)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))) ResetLevel();
                EndDrawing();
                continue;
            }

            // VICTORY SCREEN
            if (victory) {

                bool controller = IsGamepadAvailable(0);

                BeginDrawing();
                ClearBackground(WHITE);
                DrawText("VICTORIA", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 60, 40, DARKGREEN);
                if (controller){
                DrawText("PAUSA - Presiona = para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("Boton Vista para regresar al menu", 360, 500, 20, GRAY);
                }
                else{
                DrawText("PAUSA - Presiona P para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("ENTER para regresar", 360, 500, 20, GRAY);
                }
                if (IsKeyPressed(KEY_ENTER)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))
                {
                    currentScreen = TITLE;
                }

                EndDrawing();
                continue;
            }

            // PAUSA
            if (pauseGame) {

                bool controller = IsGamepadAvailable(0);

                BeginDrawing();
                ClearBackground(BLACK);
                
                if (controller){
                DrawText("PAUSA - Presiona = para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("Boton Vista para regresar", 360, 500, 20, GRAY);
                }
                else{
                DrawText("PAUSA - Presiona P para continuar", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 - 10, 20, WHITE);
                DrawText("ENTER para regresar", 360, 500, 20, GRAY);
                }
                if (IsKeyPressed(KEY_ENTER)|| (controller && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)))
                {
                    currentScreen = TITLE;
                }
                EndDrawing();
                continue;
            }
            // WIN CONDITION
            if (player.pos.x >= 4300 && !victory)
            victory = true;

            UpdatePlayerCamera(&camera, player.pos);


            BeginDrawing();
            ClearBackground(SKYBLUE);

            BeginMode2D(camera);

        

            // Plataformas
            for (int i=0;i<platformCount;i++) {

                Rectangle b = platforms[i].box;

                if (platforms[i].type == 11) {
                    DrawTexturePro(
                        groundTex,
                        (Rectangle){0,0, groundTex.width, groundTex.height},
                        (Rectangle){b.x, b.y - b.height, b.width, b.height},
                        (Vector2){0,0}, 0, WHITE
                    );
                }
                else {
                    Texture2D *t = &platformTex[platforms[i].type];

                    DrawTexturePro(
                        *t,
                        (Rectangle){0,0,t->width,t->height},
                        (Rectangle){b.x, b.y - b.height/3, b.width, b.height},
                        (Vector2){0,0}, 0, WHITE
                    );
                }
            }
            //Suelos
            DrawRectangle(10,420,900,1600,BROWN);
            DrawRectangle(1620,420,600,1600,BROWN);
            DrawRectangle(2620,420,600,1600,BROWN);
            DrawRectangle(3920,420,600,1600,BROWN);


            // ------------------------------
            // DIBUJAR ENEMIGOS ANIMADOS
            // ------------------------------
            for (int i=0;i<enemyCount;i++) {
                if (!enemies[i].active) continue;

                DrawEnemyAnimation(enemies[i].pos, enemies[i].dir, 1); // 1 = caminar
            }

            // Coleccionables
            for (int i=0;i<collectibleCount;i++) {
                if (!collectibles[i].taken)
                    DrawCircleV(collectibles[i].pos, 8, GOLD);
            }

            // Proyectiles
            for (int i=0;i<NUM_SHOOTS;i++)
                if (shoot[i].active)
                    DrawRectangleRec(shoot[i].rec, MAROON);

            // Jugador
            DrawPlayer(player.pos);

            DrawRectangle(4300, 0, 8, 800, GREEN);
            DrawText("META", 4290, -20, 20, BLACK);

            EndMode2D();

            DrawText(TextFormat("Vidas: %d", player.lives), 20, 20, 24, BLACK);
            DrawText(TextFormat("Puntaje: %d", player.score), 20, 60, 24, BLACK);
            if (controller){
            DrawText("Mando conectado", 20, 520, 20, DARKGREEN);
            }
            else{
            DrawText("Mando NO detectado", 20, 520, 20, RED);
            }


            EndDrawing();
        }
    }

    UnloadResources();
    CloseWindow();
    return 0;
}
