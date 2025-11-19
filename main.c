 
#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Camera camera = { 0 };
Vector3 cubePosition = { 0 };

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void Update(void);          // Update and draw one frame
static void ProcessInput(void);
static void DrawFrame(void);

Texture2D character_sheet;
typedef struct
{
    char* name[32];
    Rectangle fame[32];
    float fps;
    int frameCount;
    int current;
} AnimationClip;

AnimationClip animations[3];

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    //Texture2D scarfy = LoadTexture("/reource/Free Street Animal Pixel Art/1 Dog/Idle.png");
    InitWindow(screenWidth, screenHeight, "raylib");
    character_sheet = LoadTexture("resources/Free Street Animal Pixel Art/1 Dog/Idle.png");

    //animations[0];
    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        ProcessInput();
        Update();
        DrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
void Update(void)
{
    
}

void ProcessInput(void)
{

}

// Update and draw game frame
static void DrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_ORBITAL);
    //----------------------------------------------------------------------------------

    //if (IsKeyDown(KEY_W)) Player1Position.y -= 2.0f;
    //if (IsKeyDown(KEY_S)) Player1Position.y += 2.0f;
    //if (IsKeyDown(KEY_UP)) Player2Position.y -= 2.0f;
    //if (IsKeyDown(KEY_DOWN)) Player2Position.y += 2.0f;

    
     

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        
        ClearBackground(BEIGE);

        DrawTextureRec(character_sheet,(Rectangle){0,0,48,48},(Vector2){400,300},WHITE);

    

    EndDrawing();
    //----------------------------------------------------------------------------------
}

