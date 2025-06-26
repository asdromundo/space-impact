#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

// Components
struct BoundingBox {
    SDL_FRect box;
};

struct Velocity {
    float x, y, s = 0.0f;
};

struct Speed {
    float value = 0.0f;
};

// Render related
struct Sprite {
    SDL_Texture* sprite;
};

// Input
struct MovementInput2D {
    Sint16 x, y;
};

// Tamaño y forma
struct Radius {
    float value = 0.0f;
};

struct Size2D {
    float width = 0.0f;
    float height = 0.0f;
};

// Gameplay
struct Score {
    int value = 0;
};

// Etiquetas (Tags) para identificar entidades
struct PlayerTag {};
struct PaddleTag {};
struct BallTag {};
struct Player {};
struct Enemy {};
struct Bullet {};
// Relationship structs
struct Damages {};

#endif  // GAME_COMPONENTS_H
