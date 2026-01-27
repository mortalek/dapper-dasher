#include "raylib.h"

const int maxNebulae = 3;

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

struct Scarfy
{
    Texture2D texture;
    AnimData data;
    int velocity;     
    bool isInAir;
    int jumpVel;
};

enum class GameState
{
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

    ParallaxLayer background;
    ParallaxLayer midGround;
    ParallaxLayer foreground;

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

void InitNebulaSystem(NebulaSystem& nebula, Game& game)
{
    nebula.count = 3; // startowa trudność
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
        nebula.items[i].pos.x = game.windowWidth + i * 300;
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
            game.state = GameState::GameOver;
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
    }
    else 
    {
        // rectangle is in the air
        scarfy.velocity += 1'000 * dt; //gravity (pixels/s/s)
        scarfy.isInAir = true;
    }
    // jump check
    if (IsKeyPressed(KEY_SPACE) && !scarfy.isInAir)
    {
        scarfy.velocity += scarfy.jumpVel;
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
    Game game;
    game.windowWidth  = 512;
    game.windowHeight = 380;

    game.state = GameState::Playing;
    game.collision = false;

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

    game.scarfy = CreateScarfy(game);
    InitNebulaSystem(game.nebulae, game);

    return game;
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

float GetFinishLineX(const NebulaSystem& nebulae)
{
    const AnimData& last = nebulae.items[nebulae.count - 1];
    return last.pos.x + last.rec.width;
}


int main()

{
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");
    Game game = InitGame();

    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        float dT{ GetFrameTime() };

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        UpdateAndDrawLayer(
            game.background.texture,
            game.background.x,
            game.background.speed,
            dT,
            game.background.scale
        );

        UpdateAndDrawLayer(
            game.midGround.texture,
            game.midGround.x,
            game.midGround.speed,
            dT,
            game.midGround.scale
        );

        UpdateAndDrawLayer(
            game.foreground.texture,
            game.foreground.x,
            game.foreground.speed,
            dT,
            game.foreground.scale
        );
        
        UpdateScarfy(game.scarfy, game.windowHeight, dT);
         // update nebula position 
        UpdateNebulaePos(game.nebulae, dT);

        // update nebula animation frame 
        UpdateNebulaAnimations(game.nebulae, dT);

        CheckNebulaCollisions(game);

        if (game.collision)
        {
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        }
        else if (game.scarfy.data.pos.x >= GetFinishLineX(game.nebulae))
        {
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2, 40, GREEN);
        }
        else 
        {
            // draw nebula
            DrawNebulae(game);

            // draw scarfy
            DrawTextureRec(game.scarfy.texture, game.scarfy.data.rec, game.scarfy.data.pos, WHITE);
        }
        

        // stop drawing; 
        EndDrawing();
    }
    UnloadTexture(game.scarfy.texture);
    UnloadTexture(game.nebulae.texture);
    UnloadTexture(game.background.texture);
    UnloadTexture(game.midGround.texture);
    UnloadTexture(game.foreground.texture);

    CloseWindow();
}