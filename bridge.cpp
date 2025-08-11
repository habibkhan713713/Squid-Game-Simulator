#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

enum PanelType { SAFE, BREAK };

struct Panel {
    PanelType type;
    Rectangle rect;
    bool steppedOn = false;
};

int main() {
    // Windowed mode (not fullscreen)
    InitWindow(1000, 700, "Squid Game - Glass Bridge");
    SetTargetFPS(60);
    srand(time(nullptr));

    InitAudioDevice(); // Enable sound
    Sound breakSound = LoadSound("glass_break.mp3"); // Add glass_break.wav in folder

    // Load player image
    Image playerImg = LoadImage("456.png");
    if (playerImg.width == 0) {
        CloseAudioDevice();
        CloseWindow();
        printf("Failed to load 456.png. Make sure it is in the correct folder.\n");
        return -1;
    }
    Texture2D playerTexture = LoadTextureFromImage(playerImg);
    UnloadImage(playerImg);

    // Dynamic scaling for windowed resolution
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    const int rows = 5;
    float panelWidth = screenWidth * 0.15f;   // wider panels
    float panelHeight = screenHeight * 0.07f; // taller panels
    float gap = screenHeight * 0.025f;        // more gap between panels

    float titleHeight = screenHeight * 0.09f;
    float instructionHeight = screenHeight * 0.13f;
    float topMargin = titleHeight + instructionHeight + screenHeight * 0.06f; // more space above bridge
    float bottomMargin = screenHeight * 0.18f; // more space for progress bar

    // Font sizes
    int titleFontSize = screenHeight / 14;
    int instructionFontSize = screenHeight / 38;
    int messageFontSize = screenHeight / 28;
    int progressFontSize = screenHeight / 32;

    std::vector<std::vector<Panel>> bridge(rows, std::vector<Panel>(2));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 2; j++) {
            bridge[i][j].type = (rand() % 2 == 0) ? SAFE : BREAK;
            bridge[i][j].rect.x = screenWidth / 2.0f - panelWidth - gap + j * (panelWidth + gap);
            bridge[i][j].rect.y = topMargin + i * (panelHeight + gap);
            bridge[i][j].rect.width = panelWidth;
            bridge[i][j].rect.height = panelHeight;
        }
    }

    int currentRow = 0;
    bool gameOver = false;
    bool won = false;
    std::string message = "Use LEFT/RIGHT to select, SPACE/ENTER to step";
    int selectedSide = 0;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (IsKeyPressed(KEY_LEFT)) selectedSide = 0;
            if (IsKeyPressed(KEY_RIGHT)) selectedSide = 1;

            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                bridge[currentRow][selectedSide].steppedOn = true;
                if (bridge[currentRow][selectedSide].type == BREAK) {
                    PlaySound(breakSound);
                    gameOver = true;
                    won = false;
                    message = "❌ You stepped on broken glass! Game Over.";
                } else {
                    currentRow++;
                    if (currentRow >= rows) {
                        gameOver = true;
                        won = true;
                        message = "✅ You crossed safely! You Win!";
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                currentRow = 0;
                gameOver = false;
                won = false;
                message = "Use LEFT/RIGHT to select, SPACE/ENTER to step";
                selectedSide = 0;
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < 2; j++) {
                        bridge[i][j].type = (rand() % 2 == 0) ? SAFE : BREAK;
                        bridge[i][j].steppedOn = false;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground((Color){230, 240, 255, 255});

        // Title
        DrawRectangle(0, 0, screenWidth, titleHeight, (Color){50, 100, 150, 220});
        const char* titleText = "🪞 Squid Game: Glass Bridge 🪞";
        DrawText(titleText, screenWidth / 2 - MeasureText(titleText, titleFontSize) / 2, titleHeight * 0.25f, titleFontSize, RAYWHITE);

        // Instructions
        DrawRectangle(screenWidth * 0.025f, titleHeight + 5, screenWidth * 0.95f, instructionHeight, (Color){255, 255, 255, 230});
        DrawText("Use LEFT/RIGHT to select panel, SPACE/ENTER to step.", screenWidth * 0.05f, titleHeight + 15, instructionFontSize, DARKGRAY);
        DrawText("Avoid broken glass! Reach the top. Press R to restart.", screenWidth * 0.05f, titleHeight + 45, instructionFontSize, DARKGRAY);

        // Game message (separated above bridge)
        int msgWidth = MeasureText(message.c_str(), messageFontSize);
        DrawText(message.c_str(), screenWidth / 2 - msgWidth / 2, titleHeight + instructionHeight + 20, messageFontSize, (won ? GREEN : RED));

        // Bridge
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < 2; j++) {
                Color baseColor = (bridge[i][j].type == SAFE) ? (Color){200, 230, 255, 255} : (Color){255, 200, 200, 255};
                if (bridge[i][j].steppedOn) {
                    baseColor = (bridge[i][j].type == SAFE) ? (Color){50, 150, 255, 255} : (Color){255, 50, 50, 255};
                }
                DrawRectangle(bridge[i][j].rect.x + 4, bridge[i][j].rect.y + 4, panelWidth, panelHeight, (Color){0, 0, 0, 40});
                DrawRectangleRec(bridge[i][j].rect, baseColor);
                DrawRectangleLinesEx(bridge[i][j].rect, 2, (Color){80, 80, 80, 180});
            }
        }

        // Player
        if (!gameOver && currentRow < rows) {
            Rectangle panelRect = bridge[currentRow][selectedSide].rect;
            float imgWidth = panelWidth * 0.4f;
            float imgHeight = panelHeight * 1.6f;
            float imgX = panelRect.x + panelRect.width / 2.0f - imgWidth / 2.0f;
            float imgY = panelRect.y - imgHeight - 15; // more gap above panel
            DrawTexturePro(playerTexture, {0, 0, (float)playerTexture.width, (float)playerTexture.height}, {imgX, imgY, imgWidth, imgHeight}, {0, 0}, 0.0f, WHITE);
        }

        // Progress bar (moved further down)
        Rectangle progressBarBg = {screenWidth / 2.0f - screenWidth * 0.25f, screenHeight - bottomMargin + 40, screenWidth * 0.5f, 30};
        DrawRectangleRec(progressBarBg, (Color){200, 200, 200, 180});
        DrawRectangleLinesEx(progressBarBg, 2, (Color){100, 100, 100, 200});

        float progressPercent = (float)currentRow / rows;
        Rectangle progressBarFill = {progressBarBg.x + 3, progressBarBg.y + 3, (progressBarBg.width - 6) * progressPercent, progressBarBg.height - 6};
        DrawRectangleRec(progressBarFill, (Color){30, 144, 255, 220});

        // Progress text
        std::string progressText = "Progress: " + std::to_string(currentRow) + " / " + std::to_string(rows);
        DrawText(progressText.c_str(), screenWidth / 2 - MeasureText(progressText.c_str(), progressFontSize) / 2, progressBarBg.y + 5, progressFontSize, BLACK);

        // Restart hint
        if (gameOver) {
            std::string restartMsg = "Press R to Restart";
            int restartWidth = MeasureText(restartMsg.c_str(), progressFontSize);
            DrawText(restartMsg.c_str(), screenWidth / 2 - restartWidth / 2, screenHeight - bottomMargin / 2, progressFontSize, (Color){100, 100, 100, 180});
        }

        EndDrawing();
    }

    UnloadTexture(playerTexture);
    UnloadSound(breakSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
