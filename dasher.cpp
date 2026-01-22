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

struct Game 
{
    GameState state;

    int windowWidth;
    int windowHeight;

    Scarfy scarfy;
    NebulaSystem nebulae;

    bool collision;
    float finishLine;
};

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

void UpdateNebulaePos(Game& game, float dt)
{
    for (int i = 0; i < game.nebulae.count; ++i)
    {
        game.nebulae.items[i].pos.x += game.nebulae.velocity * dt;
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
        scarfy.velocity += 1'000 * dt; //gravity
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
    game.scarfy = CreateScarfy(game);
    InitNebulaSystem(game.nebulae, game);

    game.finishLine = game.nebulae.items[game.nebulae.count - 1].pos.x;

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

int main()

{
     int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher!");
    Game game = InitGame();

    // acceleration due to gravity(pixels/s/s);
    const int gravity{1'000};
 
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};

    Texture2D midGround = LoadTexture("textures/back-buildings.png");
    float mgX{};

    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        float dT{ GetFrameTime() };

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        bgX -= 20 * dT;
        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }
        // scroll the midground
        mgX -= 40 * dT;
        if (mgX <= -midGround.width*2)
        {
            mgX = 0.0;
        }
        fgX -= 80 * dT;
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }
        // draw the background
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE); 
        Vector2 bg2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        // draw the midground
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midGround, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midGround.width*2, 0.0};
        DrawTextureEx(midGround, mg2Pos, 0.0, 2.0, WHITE);

        //  draw the foreground
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);
        
        UpdateScarfy(game.scarfy, game.windowHeight, dT);
         // update nebula position 
        UpdateNebulaePos(game, dT);

        game.finishLine += game.nebulae.velocity * dT;

        // update nebula animation frame 
        for ( int i = 0; i < game.nebulae.count; i++)
        {
            game.nebulae.items[i] = updateAnimData(game.nebulae.items[i], dT, 7);
        }
        for (AnimData nebula : game.nebulae.items)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec{
                game.scarfy.data.pos.x,
                game.scarfy.data.pos.y,
                game.scarfy.data.rec.width,
                game.scarfy.data.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        }
        else if (game.scarfy.data.pos.x >= game.finishLine)
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
    UnloadTexture(background);
    UnloadTexture(midGround);
    UnloadTexture(foreground);

    CloseWindow();
}