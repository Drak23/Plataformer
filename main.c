/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
//Camera camera = { 0 };
//Vector3 cubePosition = { 0 };
//Vector2 ballPosition;
//Vector2 ballSpeed;

//Vector2 Player1Position;
//Vector2 Player1Size;
//Vector2 Player2Position;
//Vector2 Player2Size;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    

    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
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
        //ClearBackground(BLACK);

        //DrawCircleV(ballPosition, 15, RED);
        //DrawRectangleV(Player1Position, Player1Size, BLUE);
        //DrawRectangleV(Player2Position, Player2Size, GREEN);

        //DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
