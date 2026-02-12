#include "raylib.h"

const int maxNebulae = 4;
const int maxAsteroids = 3;

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};
struct NebulaSystem
{
    Texture2D texture;
    AnimData items[maxNebulae];
    int count;
    int velocity;     // nebula X velocity (pixels/second)
};

struct AsteroidData {
    AnimData data;
    bool destroyed = false;
};

struct AsteroidSystem {
    Texture2D texture;
    AsteroidData items[maxAsteroids];
    int count;
    int velocity;
};

struct Scarfy
{
    Texture2D texture;
    AnimData data;
    int velocity;     
    bool isInAir;
    int jumpVel;
    int jumpCount = 0;
    int maxJumps = 2;
};

enum class GameState
{
    Menu,
    Playing,
    GameOver,
    Win
};

struct ParallaxLayer
{
    Texture2D texture;
    float x;
    float speed;
    float scale;
};
struct Game 
{
    GameState state;

    int windowWidth;
    int windowHeight;

    Scarfy scarfy;
    NebulaSystem nebulae;
    AsteroidSystem asteroid;

    ParallaxLayer background;
    ParallaxLayer midGround;
    ParallaxLayer foreground;

    Music menuMusic;
    Music gameMusic;

    bool collision;
};

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;

    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}

void InitNebulaSystem(Game& game)
{
    NebulaSystem& nebula = game.nebulae;
    float spacing = 500.0f; // Increased spacing between nebulas
    nebula.count = 4; // for sake of testing 3 it is
    nebula.velocity = -200;
    nebula.texture = LoadTexture("textures/12_nebula_spritesheet.png");

    for (int i = 0; i < nebula.count; ++i)
    {
        nebula.items[i].rec.x = 0.0;
        nebula.items[i].rec.y = 0.0;
        nebula.items[i].rec.width = nebula.texture.width/8;
        nebula.items[i].rec.height = nebula.texture.height/8;
        nebula.items[i].pos.y = game.windowHeight - nebula.texture.height/8;
        nebula.items[i].frame = 0;
        nebula.items[i].runningTime = 0.0;
        nebula.items[i].updateTime = 1.0 / 16.0;
        nebula.items[i].pos.x = game.windowWidth + i * spacing;
    }
}

void DrawNebulae(const Game& game)
{
    for (int i = 0; i < game.nebulae.count; ++i)
    {
        DrawTextureRec(
            game.nebulae.texture,
            game.nebulae.items[i].rec,
            game.nebulae.items[i].pos,
            WHITE
        );
    }
}

void UpdateNebulaePos(NebulaSystem& nebulae, float dt)
{
    for (int i = 0; i < nebulae.count; ++i)
    {
        nebulae.items[i].pos.x += nebulae.velocity * dt;
    }
}

void UpdateNebulaAnimations(NebulaSystem& nebulae, float dt)
{
    for (int i = 0; i < nebulae.count; ++i)
    {
        nebulae.items[i] = updateAnimData(nebulae.items[i], dt, 7);
    }
}

void InitAsteroidSystem(Game& game) {
    AsteroidSystem& asteroid = game.asteroid;
    NebulaSystem& nebula = game.nebulae;
    const float baseGap = 300.0f; // Gap between nebula and asteroid

    asteroid.texture = LoadTexture("textures/Meteor_01.png");;
    asteroid.count = 3;
    asteroid.velocity = -200;
    for (int i = 0; i < asteroid.count; ++i) {
        float gap = baseGap + i * 400.0f;

        asteroid.items[i].data.rec.x = 0.0f;
        asteroid.items[i].data.rec.y = 0.0f;
        asteroid.items[i].data.rec.width = asteroid.texture.width; 
        asteroid.items[i].data.rec.height = asteroid.texture.height;
        asteroid.items[i].data.frame = 0;
        asteroid.items[i].data.updateTime = 1.0f / 12.0f;
        asteroid.items[i].data.runningTime = 0.0f;
        asteroid.items[i].destroyed = false;

        // Place asteroid after nebula with a gap
        asteroid.items[i].data.pos.x = nebula.items[2].pos.x + gap;
        asteroid.items[i].data.pos.y = game.windowHeight - asteroid.texture.height; // Adjust vertical position as needed
    }
}

void DrawAsteroid(const Game& game)
{
    for (int i = 0; i < game.asteroid.count; ++i)
    {
        DrawTextureRec(
            game.asteroid.texture,
            game.asteroid.items[i].data.rec,
            game.asteroid.items[i].data.pos,
            WHITE
        );
    }
}

void UpdateAsteroidPos(AsteroidSystem& asteroid, float dt)
{
    for (int i = 0; i < asteroid.count; ++i)
    {
        asteroid.items[i].data.pos.x += asteroid.velocity * dt;
    }
}


void OnGameStateChanged(Game& game, GameState newState)
{
    if (game.state == newState) return;

    StopMusicStream(game.menuMusic);
    StopMusicStream(game.gameMusic);

    game.state = newState;

    switch (newState)
    {
        case GameState::Menu:
        case GameState::GameOver:
        case GameState::Win:
            PlayMusicStream(game.menuMusic);
            break;

        case GameState::Playing:
            PlayMusicStream(game.gameMusic);
            break;
    }
}

void CheckNebulaCollisions(Game& game)
{
    const float pad = 50.0f;

    Rectangle scarfyRec{
        game.scarfy.data.pos.x,
        game.scarfy.data.pos.y,
        game.scarfy.data.rec.width,
        game.scarfy.data.rec.height
    };

    for (int i = 0; i < game.nebulae.count; ++i)
    {
        AnimData& nebula = game.nebulae.items[i];

        Rectangle nebRec{
            nebula.pos.x + pad,
            nebula.pos.y + pad,
            nebula.rec.width - 2 * pad,
            nebula.rec.height - 2 * pad
        };

        if (CheckCollisionRecs(nebRec, scarfyRec))
        {
            game.collision = true;
            OnGameStateChanged(game, GameState::GameOver);

            return;
        }
    }
}

void CheckAsteroidCollisions(Game& game)
{
    const float pad = 50.0f;

    Rectangle scarfyRec{
        game.scarfy.data.pos.x,
        game.scarfy.data.pos.y,
        game.scarfy.data.rec.width,
        game.scarfy.data.rec.height
    };

    for (int i = 0; i < game.asteroid.count; ++i)
    {
        AnimData& asteroid = game.asteroid.items[i].data;

        Rectangle nebRec{
            asteroid.pos.x + pad,
            asteroid.pos.y + pad,
            asteroid.rec.width - 2 * pad,
            asteroid.rec.height - 2 * pad
        };

        if (CheckCollisionRecs(nebRec, scarfyRec))
        {
            game.collision = true;
            OnGameStateChanged(game, GameState::GameOver);

            return;
        }
    }
}



Scarfy CreateScarfy(const Game& game)
{
    Scarfy scarfy;
    scarfy.isInAir = false;
    scarfy.jumpVel = -600;
    scarfy.texture = LoadTexture("textures/scarfy.png");

    scarfy.data.rec.x = 0.0f;
    scarfy.data.rec.y = 0.0f;
    scarfy.data.rec.width  = scarfy.texture.width / 6.0f;
    scarfy.data.rec.height = scarfy.texture.height;

    scarfy.data.pos.x = game.windowWidth / 2.0f - scarfy.data.rec.width / 2.0f;
    scarfy.data.pos.y = game.windowHeight - scarfy.data.rec.height;

    scarfy.data.frame = 0;
    scarfy.data.runningTime = 0.0f;
    scarfy.data.updateTime  = 1.0f / 12.0f;

    return scarfy;
}

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

void UpdateScarfy(Scarfy& scarfy, int windowHeight, float dt)
{
    // perform ground check
    if (isOnGround(scarfy.data, windowHeight)) 
    {
    //    rectangle is on the ground
        scarfy.velocity = 0;
        scarfy.isInAir = false;
        scarfy.jumpCount = 0;
    }
    else 
    {
        // rectangle is in the air
        scarfy.velocity += 1'000 * dt; //gravity (pixels/s/s)
        scarfy.isInAir = true;
    }
    // jump check
    if (IsKeyPressed(KEY_SPACE) && scarfy.jumpCount < scarfy.maxJumps)
    {
        scarfy.velocity = scarfy.jumpVel;
        scarfy.isInAir = true;
        scarfy.jumpCount++;
    }
    // update scarfy position 
    scarfy.data.pos.y += scarfy.velocity * dt;

    if (!scarfy.isInAir)
    {
        scarfy.data = updateAnimData(scarfy.data, dt, 5);
    }
}

Game InitGame()
{
    Game game{};

    game.windowWidth  = 512;
    game.windowHeight = 380;

    game.state = GameState::Menu;
    game.collision = false;

    // Parallax layers (loaded once)
    game.background = {
        LoadTexture("textures/far-buildings.png"),
        0.0f,
        20.0f,
        2.0f
    };

    game.midGround = {
        LoadTexture("textures/back-buildings.png"),
        0.0f,
        40.0f,
        2.0f
    };

    game.foreground = {
        LoadTexture("textures/foreground.png"),
        0.0f,
        80.0f,
        2.0f
    };

    game.menuMusic = LoadMusicStream("audio/where_it_leads.mp3");
    game.gameMusic = LoadMusicStream("audio/vei.mp3");

    game.menuMusic.looping = true;
    game.gameMusic.looping = true;
    PlayMusicStream(game.menuMusic);

    return game;
}

void StartGame(Game& game)
{
    game.collision = false;
    game.scarfy = CreateScarfy(game);
    InitNebulaSystem(game);
    InitAsteroidSystem(game);
}

void UpdateParallax(ParallaxLayer& layer, float dt)
{
    layer.x -= layer.speed * dt;

    if (layer.x <= -layer.texture.width * layer.scale)
        layer.x = 0.0f;
}


void DrawParallax(const ParallaxLayer& layer)
{
    Vector2 pos1{ layer.x, 0.0f };
    Vector2 pos2{ layer.x + layer.texture.width * layer.scale, 0.0f };

    DrawTextureEx(layer.texture, pos1, 0.0f, layer.scale, WHITE);
    DrawTextureEx(layer.texture, pos2, 0.0f, layer.scale, WHITE);
}



void InitScarfy(AnimData& scarfy, int windowWidth, int windowHeight, Texture2D scarfyTexture)
{
    scarfy.rec.x = 0.0f;
    scarfy.rec.y = 0.0f;

    scarfy.rec.width  = scarfyTexture.width / 6.0f;
    scarfy.rec.height = scarfyTexture.height;

    scarfy.pos.x = windowWidth / 2.0f - scarfy.rec.width / 2.0f;
    scarfy.pos.y = windowHeight - scarfy.rec.height;

    scarfy.frame = 0;
    scarfy.runningTime = 0.0f;
    scarfy.updateTime  = 1.0f / 12.0f;
}

void UpdateAndDrawLayer(Texture2D texture, float& x, float speed, float dt, float scale = 2.0f)
{
    x -= speed * dt;

    if (x <= -texture.width * scale)
    {
        x = 0.0f;
    }

    Vector2 pos1{ x, 0.0f };
    Vector2 pos2{ x + texture.width * scale, 0.0f };

    DrawTextureEx(texture, pos1, 0.0f, scale, WHITE);
    DrawTextureEx(texture, pos2, 0.0f, scale, WHITE);
}

float GetFinishLineX(const AsteroidSystem& asteroid)
{
    const AsteroidData& last = asteroid.items[asteroid.count - 1];
    return last.data.pos.x + last.data.rec.width;
}

void ResetToMenuGame(Game& game)
{
    game.state = GameState::Menu;
    game.collision = false;
}

void UpdateGame(Game& game, float dt)
{
    UpdateMusicStream(game.menuMusic);
    UpdateMusicStream(game.gameMusic);

    switch (game.state)
    {
        case GameState::Menu:
            if (IsKeyPressed(KEY_SPACE))
            {
                StartGame(game);
                OnGameStateChanged(game, GameState::Playing);
            }
            break;

        case GameState::Playing:

            UpdateParallax(game.background, dt);
            UpdateParallax(game.midGround, dt);
            UpdateParallax(game.foreground, dt);

            UpdateScarfy(game.scarfy, game.windowHeight, dt);
            UpdateNebulaePos(game.nebulae, dt);
            UpdateNebulaAnimations(game.nebulae, dt);
            UpdateAsteroidPos(game.asteroid, dt);
            CheckNebulaCollisions(game);
            CheckAsteroidCollisions(game);
            
            if (game.scarfy.data.pos.x >= GetFinishLineX(game.asteroid)) {
                OnGameStateChanged(game, GameState::Win);
            }
            break;

        case GameState::GameOver:
        case GameState::Win:
            if (IsKeyPressed(KEY_SPACE))
            {
                ResetToMenuGame(game);
                OnGameStateChanged(game, GameState::Menu);
            }
            break;
    }
}

void DrawGame(const Game& game)
{
    Rectangle overlay{
        80,    // x
        180,   // y
        350,   // width
        100    // height
    };

    Rectangle overlayMenu{
        80,    // x
        75,   // y
        375,   // width
        150    // height
    };

    Color overlayColor = { 0, 0, 0, 200 };

    switch (game.state)
    {
        case GameState::Menu:
            DrawParallax(game.background);
            DrawParallax(game.midGround);
            DrawParallax(game.foreground);
            DrawRectangleRec(overlayMenu, overlayColor);
            DrawText("DAPPER DASHER", 90, 120, 40, DARKGRAY);
            DrawText("Press SPACE to start", 110, 180, 20, GRAY);
            break;

        case GameState::Playing:
            DrawParallax(game.background);
            DrawParallax(game.midGround);
            DrawParallax(game.foreground);
            DrawNebulae(game);
            DrawAsteroid(game);
            DrawTextureRec(game.scarfy.texture, game.scarfy.data.rec, game.scarfy.data.pos, WHITE);
            break;

        case GameState::GameOver:
            DrawParallax(game.background);
            DrawParallax(game.midGround);
            DrawParallax(game.foreground);
            DrawRectangleRec(overlay, overlayColor);
            DrawText("Game Over!", 120, 190, 40, RED);
            DrawText("Press SPACE to go to menu", 100, 240, 20, GRAY);
            break;

        case GameState::Win:
            DrawParallax(game.background);
            DrawParallax(game.midGround);
            DrawParallax(game.foreground);
            DrawRectangleRec(overlay, overlayColor);
            DrawText("You Win!", 140, 190, 40, GREEN);
            DrawText("Press SPACE to go to menu", 100, 240, 20, GRAY);
            break;
    }
}

int main()

{
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");
    InitAudioDevice();
    
    Game game = InitGame();
    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        float dT{ GetFrameTime() };

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);
        UpdateGame(game, dT);
    
        DrawGame(game);

        // stop drawing; 
        EndDrawing();
    }
    UnloadTexture(game.scarfy.texture);
    UnloadTexture(game.nebulae.texture);
    UnloadTexture(game.background.texture);
    UnloadTexture(game.midGround.texture);
    UnloadTexture(game.foreground.texture);

    StopMusicStream(game.menuMusic);
    StopMusicStream(game.gameMusic);
    UnloadMusicStream(game.menuMusic);
    UnloadMusicStream(game.gameMusic);
    CloseAudioDevice();
    CloseWindow();
}