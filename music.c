// music.c
// Manejo totalmente separado de la música del menú y del juego

#include "raylib.h"

// Variables globales que contendrán la música cargada
Music musicMenu;
Music musicGame;

// ------------------------------------------------------
// Cargar música desde archivos
// ------------------------------------------------------
void LoadMusicTracks() {

    InitAudioDevice();   // Se inicializa el sistema de audio

    // Música del menú
    musicMenu = LoadMusicStream("resources/Audio/menu_music.mp3");
    musicMenu.looping = true;

    // Música del juego (puedes usar otro archivo o el mismo)
    musicGame = LoadMusicStream("resources/Audio/game_music.mp3");
    musicGame.looping = true;
}

// ------------------------------------------------------
// Reproducir música del menú
// ------------------------------------------------------
void PlayMenuMusic() {
    StopMusicStream(musicGame);   // Detener música del juego si suena
    PlayMusicStream(musicMenu);
}

// ------------------------------------------------------
// Reproducir música del juego
// ------------------------------------------------------
void PlayGameMusic() {
    StopMusicStream(musicMenu);
    PlayMusicStream(musicGame);
}

// ------------------------------------------------------
// Actualizar ambas pistas (DEBE LLAMARSE CADA FRAME)
// ------------------------------------------------------
void UpdateAllMusic() {
    UpdateMusicStream(musicMenu);
    UpdateMusicStream(musicGame);
}

// ------------------------------------------------------
// Descargar música al salir del juego
// ------------------------------------------------------
void UnloadMusicTracks() {
    UnloadMusicStream(musicMenu);
    UnloadMusicStream(musicGame);

    CloseAudioDevice(); // Cerrar el sistema de audio
}
