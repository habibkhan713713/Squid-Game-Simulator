// Super Squid Tournament - Image Player + Dalgona Integrated
// Complete file with improved single-PNG Dalgona mini-game
#include <raylib.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

struct Player {
    std::string name;
    Color color;
    Vector2 pos;
    KeyboardKey moveKey;
    bool alive = true;
    bool finished = false;
    int score = 0;
    float finishTime = -1;
    float glowPhase = 0.f;
    int textureIndex = 0; // Index for player image
};

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

enum GameState { MENU, CONTROLS, REDLIGHT, DALGONA, WINNERS, GAMEOVER };
GameState currentState = MENU;

enum Phase { PHASE_GREEN, PHASE_RED };

Font gameFont;
Sound hitSound, winSound, dollPoem;
Texture2D dollFront, dollBack;
std::vector<Texture2D> playerTextures;

std::vector<Player> players;

float gameTimer = 30.0f;
Phase currentPhase = PHASE_GREEN;
float phaseTimer = 0.0f;
float greenDuration = 0.0f;
float redDuration = 2.0f;

// Dalgona resources
Texture2D dalgonaShapes[4];
Sound crackSound, scratchSound;

void ResetPlayers() {
    int startX = 80;
    int startY = 200;
    int gapY = 100;
    for (size_t i = 0; i < players.size(); i++) {
        players[i].pos = {(float)startX, (float)(startY + gapY * i)};
        players[i].alive = true;
        players[i].finished = false;
        players[i].finishTime = -1;
        players[i].glowPhase = 0;
    }
}

bool AreAllPlayersDeadOrFinished() {
    for (auto &p : players) if (p.alive && !p.finished) return false;
    return true;
}

void DrawTextOutlined(const char *text, int x, int y, int fontSize, Color textColor, Color outlineColor) {
    for (int ox = -2; ox <= 2; ox++) {
        for (int oy = -2; oy <= 2; oy++) {
            if (ox == 0 && oy == 0) continue;
            DrawText(text, x + ox, y + oy, fontSize, outlineColor);
        }
    }
    DrawText(text, x, y, fontSize, textColor);
}

void DrawPlayer(const Player &p) {
    if (!p.alive) return;

    float glow = (sinf(p.glowPhase) + 1.0f) / 2.0f;
    Color glowColor = {
        (unsigned char)(p.color.r + (255 - p.color.r) * glow),
        (unsigned char)(p.color.g + (255 - p.color.g) * glow),
        (unsigned char)(p.color.b + (255 - p.color.b) * glow),
        200};
    DrawCircleV(p.pos, 38, glowColor); // Glow behind

    Texture2D tex = playerTextures[p.textureIndex];

    // Special scale for player 456 (index 0) to visually match others
    float scale = (p.textureIndex == 0) ? 0.38f : 0.5f;

    float width = tex.width * scale;
    float height = tex.height * scale;
    Vector2 drawPos = { p.pos.x - width / 2, p.pos.y - height / 2 };
    DrawTextureEx(tex, drawPos, 0.0f, scale, WHITE);

    DrawTextEx(gameFont, p.name.c_str(), {(float)p.pos.x - 28, (float)p.pos.y - 50}, 24, 1, WHITE);
}

void DrawBackground() {
    ClearBackground((Color){237, 27, 118, 255});
    int gridSize = 100;
    for (int x = 0; x < SCREEN_WIDTH; x += gridSize)
        DrawLine(x, 0, x, SCREEN_HEIGHT, (Color){255, 105, 180, 80});
    for (int y = 0; y < SCREEN_HEIGHT; y += gridSize)
        DrawLine(0, y, SCREEN_WIDTH, y, (Color){255, 105, 180, 80});
}

#include <cstdlib>  // for std::system

void ShowMenu() {
    BeginDrawing();
    DrawBackground();
    DrawTextOutlined("Super Squid Tournament", SCREEN_WIDTH / 2 - 300, 100, 60, DARKPURPLE, BLACK);

    DrawTextOutlined("1) Start Game - Red Light, Green Light", SCREEN_WIDTH / 2 - 300, 300, 30, WHITE, BLACK);
    DrawTextOutlined("2) Tug of War", SCREEN_WIDTH / 2 - 300, 350, 30, WHITE, BLACK);
    DrawTextOutlined("3) Dalgona Candy (Mini-Game)", SCREEN_WIDTH / 2 - 300, 400, 30, WHITE, BLACK);
    DrawTextOutlined("4) Bridge game", SCREEN_WIDTH / 2 - 300, 450, 30, WHITE, BLACK);
    DrawTextOutlined("5) Marble", SCREEN_WIDTH / 2 - 300, 500, 30, WHITE, BLACK);
    DrawTextOutlined("6) Controls", SCREEN_WIDTH / 2 - 300, 550, 30, WHITE, BLACK);
    DrawTextOutlined("Q) Quit", SCREEN_WIDTH / 2 - 300, 600, 30, WHITE, BLACK);
    EndDrawing();

    if (IsKeyPressed(KEY_ONE)) {
        printf("Pressed 1 - Starting Red Light Green Light\n");
        currentState = REDLIGHT;
    }
    else if (IsKeyPressed(KEY_TWO)) {
        printf("Pressed 2 - Launching rope.exe\n");
        std::system("./rope");  // Update path if needed
    }
    else if (IsKeyPressed(KEY_THREE)) {
        printf("Pressed 3 - Starting Dalgona\n");
        currentState = DALGONA;
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        printf("Pressed 4 - Launching bridge.exe\n");
        std::system("./bridge");  // Update path if needed
    }
    else if (IsKeyPressed(KEY_FIVE)) {
        printf("Pressed 5 - Launching marble.exe\n");
        std::system("./marble");  // Update path if needed
    }
    else if (IsKeyPressed(KEY_SIX)) {
        printf("Pressed 6 - Showing Controls\n");
        currentState = CONTROLS;
    }
    else if (IsKeyPressed(KEY_Q)) {
        printf("Pressed Q - Exiting\n");
        currentState = GAMEOVER;
    }
}


void ShowControls() {
    BeginDrawing();
    DrawBackground();
    DrawTextOutlined("Controls", SCREEN_WIDTH / 2 - 60, 100, 50, WHITE, BLACK);
    DrawTextOutlined("Blue (456): → key", 200, 200, 30, BLUE, BLACK);
    DrawTextOutlined("Red (222): D key", 200, 250, 30, RED, BLACK);
    DrawTextOutlined("Green (333): E key", 200, 300, 30, GREEN, BLACK);
    DrawTextOutlined("Yellow (388): S key", 200, 350, 30, GOLD, BLACK);
    DrawTextOutlined("Press B to go back", SCREEN_WIDTH / 2 - 100, 600, 30, WHITE, BLACK);
    EndDrawing();

    if (IsKeyPressed(KEY_B)) currentState = MENU;
    if (IsKeyPressed(KEY_Q)) currentState = GAMEOVER;
}

void ShowWinners() {
    BeginDrawing();
    DrawBackground();
    DrawTextOutlined("Winners", SCREEN_WIDTH / 2 - 80, 50, 50, WHITE, BLACK);

    std::vector<Player> sortedPlayers = players;
    std::sort(sortedPlayers.begin(), sortedPlayers.end(),
              [](const Player &a, const Player &b) {
                  if (a.finishTime < 0 && b.finishTime < 0) return false;
                  if (a.finishTime < 0) return false;
                  if (b.finishTime < 0) return true;
                  return a.finishTime < b.finishTime;
              });

    int y = 150;
    int rank = 1;
    for (auto &p : sortedPlayers) {
        if (p.finished && p.alive) {
            DrawTextOutlined(TextFormat("%d) %s", rank, p.name.c_str()), 400, y, 40, p.color, BLACK);
            y += 50;
            rank++;
        }
    }

    DrawTextOutlined("Press M to go to Menu", SCREEN_WIDTH / 2 - 150, 600, 30, WHITE, BLACK);
    EndDrawing();

    if (IsKeyPressed(KEY_M)) currentState = MENU;
}

void RedLightGreenLight() {
    ResetPlayers();
    const int finishX = SCREEN_WIDTH - 150;
    gameTimer = 30.0f;
    currentPhase = PHASE_GREEN;
    phaseTimer = 0.0f;
    PlaySound(dollPoem);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        gameTimer -= dt;
        phaseTimer += dt;

        if (currentPhase == PHASE_GREEN && phaseTimer >= greenDuration) {
            currentPhase = PHASE_RED;
            phaseTimer = 0;
        } else if (currentPhase == PHASE_RED && phaseTimer >= redDuration) {
            currentPhase = PHASE_GREEN;
            phaseTimer = 0;
            PlaySound(dollPoem);
        }

        BeginDrawing();
        DrawBackground();
        DrawTextOutlined(TextFormat("Time Left: %.1f", gameTimer), SCREEN_WIDTH - 300, 50, 40, WHITE, BLACK);

        Texture2D doll = (currentPhase == PHASE_GREEN) ? dollBack : dollFront;
        float dollScale = 0.4f;
        float dollWidth = doll.width * dollScale;
        float dollHeight = doll.height * dollScale;
        DrawTexturePro(doll, Rectangle{0, 0, (float)doll.width, (float)doll.height},
                       Rectangle{SCREEN_WIDTH / 2 - dollWidth / 2, 50, dollWidth, dollHeight},
                       Vector2{0, 0}, 0.0f, WHITE);

        DrawTextOutlined(currentPhase == PHASE_GREEN ? "GREEN LIGHT" : "RED LIGHT",
                         SCREEN_WIDTH / 2 - 150, 20, 60,
                         currentPhase == PHASE_GREEN ? GREEN : RED, BLACK);
        DrawLine(finishX, 0, finishX, SCREEN_HEIGHT, YELLOW);

        for (auto &p : players) {
            if (!p.alive || p.finished) continue;

            if (IsKeyDown(p.moveKey)) {
                if (currentPhase == PHASE_GREEN) {
                    p.pos.x += 4;
                } else {
                    p.alive = false;
                    PlaySound(hitSound);
                }
            }

            if (p.pos.x >= finishX && p.alive && !p.finished) {
                p.finished = true;
                p.finishTime = 30.0f - gameTimer;
                PlaySound(winSound);
            }

            DrawPlayer(p);
            p.glowPhase += 0.05f;
        }

        EndDrawing();

        if (gameTimer <= 0 || AreAllPlayersDeadOrFinished()) break;
    }

    currentState = WINNERS;
}

// ---------------------------
// Dalgona (single PNG) helper + game
// ---------------------------

// Configuration (tweak these to make it easier/harder)
const int MAX_CRACKS_ALLOWED = 30;
const float CRACK_COOLDOWN = 0.35f; // seconds between counting cracks
const float PROGRESS_GOAL = 0.60f;  // 60% of outline pixels
const int OUTLINE_DARKNESS_THRESHOLD = 110; // lower -> more strict outline detection (0-255)
const int ALLOWED_RADIUS_INSIDE = 2;  // how far inside outline we allow (in image pixels)
const int ALLOWED_RADIUS_OUTSIDE = 2; // how far outside outline we allow (in image pixels)

static inline bool IsDarkOutline(const Color &c) {
    // Use luminance to detect dark outline pixels; require visible alpha
    if (c.a == 0) return false;
    float luminance = 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
    return (luminance <= OUTLINE_DARKNESS_THRESHOLD);
}
static inline bool IsVisibleCookiePixel(const Color &c) {
    return (c.a > 0);
}

// Check if any outline pixel exists within tolerance radius around (x,y)
bool FindAndMarkOutlineNearby(int imgX, int imgY, int w, int h, Color *pixels,
                              std::vector<bool> &scratchedMask, int &scratchedPixels) {
    int r = std::max(ALLOWED_RADIUS_INSIDE, ALLOWED_RADIUS_OUTSIDE);
    bool foundAny = false;

    // We'll search in a small square and if we find outline pixels we'll mark them
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            int nx = imgX + dx;
            int ny = imgY + dy;
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
            Color c = pixels[ny * w + nx];
            if (IsDarkOutline(c)) {
                int idx = ny * w + nx;
                if (!scratchedMask[idx]) {
                    scratchedMask[idx] = true;
                    scratchedPixels++;
                }
                foundAny = true;
            }
        }
    }
    return foundAny;
}

// Main Dalgona function (replaces previous)
void PlayDalgona() {
    // UI & gameplay params
    const float revealDuration = 1.2f;
    const float shapeScale = 1.0f;

    Texture2D mysteryTexture = LoadTexture("cookie_box.png"); // closed box image for selection
    Color mysteryTint = {255, 255, 255, 255};

    int selectedIndex = -1;
    int assignedShape = -1;

    // Step 1: Player picks one of 4 closed cookies (instant selection)
    while (!WindowShouldClose() && selectedIndex == -1) {
        BeginDrawing();
        DrawBackground();
        DrawTextOutlined("Pick a Cookie!", SCREEN_WIDTH / 2 - 150, 50, 50, GOLD, BLACK);

        for (int i = 0; i < 4; i++) {
            float x = 240 + i * 200;
            float y = 280;
            float scale = 0.38f;

            DrawTextureEx(mysteryTexture, {x, y}, 0.0f, scale, mysteryTint);

            Rectangle rect = {x, y, mysteryTexture.width * scale, mysteryTexture.height * scale};
            Vector2 mouse = GetMousePosition();

            if (CheckCollisionPointRec(mouse, rect)) {
                DrawRectangleLinesEx(rect, 5, YELLOW);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedIndex = i;
                }
            }
        }

        DrawTextOutlined("Click any box to choose — shape assigned randomly", SCREEN_WIDTH / 2 - 360, 380, 20, WHITE, BLACK);
        DrawTextOutlined("Press B to go back", SCREEN_WIDTH / 2 - 100, 620, 24, WHITE, BLACK);
        EndDrawing();

        if (IsKeyPressed(KEY_B)) {
            UnloadTexture(mysteryTexture);
            currentState = MENU;
            return;
        }
    }
    if (WindowShouldClose()) {
        UnloadTexture(mysteryTexture);
        return;
    }

    // Step 2: Assign random cookie shape (from the loaded dalgonaShapes textures)
    assignedShape = rand() % 4;
    Texture2D chosenShape = dalgonaShapes[assignedShape];

    // Step 3: Reveal chosen shape briefly
    float revealTimer = 0.0f;
    while (!WindowShouldClose() && revealTimer < revealDuration) {
        revealTimer += GetFrameTime();
        BeginDrawing();
        DrawBackground();
        DrawTextOutlined("Your Cookie Shape!", SCREEN_WIDTH / 2 - 160, 50, 50, GOLD, BLACK);
        Vector2 shapePos = {(SCREEN_WIDTH - chosenShape.width * shapeScale) / 2,
                            (SCREEN_HEIGHT - chosenShape.height * shapeScale) / 2};
        DrawTextureEx(chosenShape, shapePos, 0.0f, shapeScale, WHITE);
        EndDrawing();
    }
    if (WindowShouldClose()) {
        UnloadTexture(mysteryTexture);
        return;
    }

    // Step 4: Prepare tracing: load image pixels from chosen texture
    Image shapeImg = LoadImageFromTexture(chosenShape);
    Color *pixels = LoadImageColors(shapeImg);
    int imgW = shapeImg.width;
    int imgH = shapeImg.height;

    // Compute total outline pixels (we consider dark pixels as outline)
    int totalOutlinePixels = 0;
    for (int y = 0; y < imgH; y++) {
        for (int x = 0; x < imgW; x++) {
            if (IsDarkOutline(pixels[y * imgW + x])) totalOutlinePixels++;
        }
    }
    if (totalOutlinePixels <= 0) {
        // fallback: consider any visible pixel as outline (very permissive)
        for (int y = 0; y < imgH; y++)
            for (int x = 0; x < imgW; x++)
                if (IsVisibleCookiePixel(pixels[y * imgW + x])) totalOutlinePixels++;
    }

    std::vector<bool> scratchedMask(imgW * imgH, false);
    int scratchedPixels = 0;
    int crackCounter = 0;
    float crackTimer = 0.0f;
    float progress = 0.0f;

    // Compute on-screen position & scale for drawing the shape nicely centered (scale to fit)
    float maxDisplayW = SCREEN_WIDTH * 0.6f;
    float maxDisplayH = SCREEN_HEIGHT * 0.6f;
    float scaleX = maxDisplayW / imgW;
    float scaleY = maxDisplayH / imgH;
    float drawScale = std::min(scaleX, scaleY);
    if (drawScale <= 0.0f) drawScale = 1.0f;
    Vector2 shapePos = {(SCREEN_WIDTH - imgW * drawScale) / 2.0f,
                        (SCREEN_HEIGHT - imgH * drawScale) / 2.0f};

    // Step 5: Tracing loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        crackTimer -= dt;

        BeginDrawing();
        DrawBackground();
        DrawTextOutlined("Trace the Shape Carefully! Hold mouse button while tracing", SCREEN_WIDTH / 2 - 340, 20, 22, GOLD, BLACK);

        // Draw cookie
        DrawTextureEx(chosenShape, shapePos, 0.0f, drawScale, WHITE);

        Vector2 mouse = GetMousePosition();
        int imgX = (int)((mouse.x - shapePos.x) / drawScale);
        int imgY = (int)((mouse.y - shapePos.y) / drawScale);

        bool anyOutlineHitThisFrame = false;

        if (imgX >= 0 && imgX < imgW && imgY >= 0 && imgY < imgH) {
            // Only active when holding mouse button
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                // We require:
                //  - the current point to be within the cookie (visible)
                //  - and near an outline pixel within allowed radii
                Color curPx = pixels[imgY * imgW + imgX];
                bool insideCookie = IsVisibleCookiePixel(curPx);
                if (insideCookie) {
                    // Try to find any outline pixel nearby and mark it scratched
                    bool found = FindAndMarkOutlineNearby(imgX, imgY, imgW, imgH, pixels, scratchedMask, scratchedPixels);
                    if (found) {
                        anyOutlineHitThisFrame = true;
                        // small feedback sound but don't spam: play when we actually scratch new pixel
                        PlaySound(scratchSound);
                    }
                }
                // If we didn't hit any outline pixels near the cursor, this counts toward a crack (cooldown applies)
                if (!anyOutlineHitThisFrame) {
                    if (crackTimer <= 0.0f) {
                        crackCounter++;
                        crackTimer = CRACK_COOLDOWN;
                        PlaySound(crackSound);
                    }
                }
            }
        }

        // Update progress
        if (totalOutlinePixels > 0) progress = (float)scratchedPixels / (float)totalOutlinePixels;
        else progress = 0.0f;

        // Draw scratched overlay (small visualization): draw small circles on scratched outline pixels (scaled)
        // to show progress to player (optional). Keep it light-weight by sampling.
        for (int y = 0; y < imgH; y++) {
            for (int x = 0; x < imgW; x++) {
                int idx = y * imgW + x;
                if (scratchedMask[idx]) {
                    Vector2 drawAt = { shapePos.x + x * drawScale, shapePos.y + y * drawScale };
                    // small dot
                    DrawCircleV(drawAt, std::max(1.0f, drawScale * 0.6f), Fade(GRAY, 0.9f));
                }
            }
        }

        // Progress bar
        int barX = 200;
        int barW = SCREEN_WIDTH - 400;
        int barY = SCREEN_HEIGHT - 70;
        DrawRectangle(barX, barY, barW, 22, GRAY);
        DrawRectangle(barX, barY, (int)(barW * (progress / PROGRESS_GOAL)), 22, GREEN);
        DrawRectangleLines(barX, barY, barW, 22, BLACK);

        DrawTextOutlined(TextFormat("Progress: %.0f%% (goal %.0f%%)",
                                    progress * 100.0f, PROGRESS_GOAL * 100.0f),
                         barX + 10, barY - 28, 20, WHITE, BLACK);
        DrawTextOutlined(TextFormat("Cracks: %d / %d", crackCounter, MAX_CRACKS_ALLOWED),
                         barX + barW - 200, barY - 28, 20, RED, BLACK);

        EndDrawing();

        // Win / lose conditions
        if (progress >= PROGRESS_GOAL) {
            // success - mark winners state
            UnloadImageColors(pixels);
            UnloadImage(shapeImg);
            UnloadTexture(mysteryTexture);
            currentState = WINNERS;
            return;
        }
        if (crackCounter >= MAX_CRACKS_ALLOWED) {
            // failed - go to winners state as "failed" (you can change to GAMEOVER or specific screen)
            UnloadImageColors(pixels);
            UnloadImage(shapeImg);
            UnloadTexture(mysteryTexture);
            currentState = WINNERS;
            return;
        }
    }

    // Fallback cleanup
    UnloadImageColors(pixels);
    UnloadImage(shapeImg);
    UnloadTexture(mysteryTexture);
}

// ---------------------------
// Main
// ---------------------------
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Super Squid Tournament");
    InitAudioDevice();
    SetTargetFPS(60);
    srand((unsigned int)time(nullptr));

    // Load assets
    hitSound = LoadSound("hit.mp3");
    winSound = LoadSound("win.mp3");
    dollPoem = LoadSound("poem.mp3");

    dollFront = LoadTexture("doll_front.png");
    dollBack = LoadTexture("doll_back.png");

    Wave poemWave = LoadWave("poem.mp3");
    greenDuration = (float)poemWave.frameCount / poemWave.sampleRate;
    UnloadWave(poemWave);

    playerTextures.push_back(LoadTexture("456.png"));
    playerTextures.push_back(LoadTexture("222.png"));
    playerTextures.push_back(LoadTexture("333.png"));
    playerTextures.push_back(LoadTexture("388.png"));

    // Your cookie PNGs (the actual cookie image for each shape)
    dalgonaShapes[0] = LoadTexture("circle.png");
    dalgonaShapes[1] = LoadTexture("triangle.png");
    dalgonaShapes[2] = LoadTexture("umbrella.png");
    dalgonaShapes[3] = LoadTexture("star.png");

    crackSound = LoadSound("crack.mp3");
    scratchSound = LoadSound("scratch.mp3");

    gameFont = GetFontDefault();

    players = {
        {"456", BLUE, {80, 200}, KEY_RIGHT, true, false, 0, -1, 0.f, 0},
        {"222", RED,  {80, 300}, KEY_D,     true, false, 0, -1, 0.f, 1},
        {"333", GREEN,{80, 400}, KEY_E,     true, false, 0, -1, 0.f, 2},
        {"388", GOLD, {80, 500}, KEY_S,     true, false, 0, -1, 0.f, 3}
    };

    while (!WindowShouldClose()) {
        switch (currentState) {
        case MENU: ShowMenu(); break;
        case CONTROLS: ShowControls(); break;
        case REDLIGHT: RedLightGreenLight(); break;
        case DALGONA: PlayDalgona(); break;
        case WINNERS: ShowWinners(); break;
        case GAMEOVER:
            CloseWindow();
            return 0;
        }
    }

    // Cleanup
    UnloadSound(hitSound);
    UnloadSound(winSound);
    UnloadSound(dollPoem);
    UnloadTexture(dollFront);
    UnloadTexture(dollBack);
    for (auto &tex : playerTextures) UnloadTexture(tex);
    for (int i = 0; i < 4; ++i) UnloadTexture(dalgonaShapes[i]);
    UnloadSound(crackSound);
    UnloadSound(scratchSound);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
