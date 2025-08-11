#include <raylib.h>
#include <string>

int main() {
    const int screenWidth = 900;
    const int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "Squid Game - Tug of War 2 Player");
    SetTargetFPS(60);

    // Load rope image
    Image ropeImg = LoadImage("rope.png");
    Texture2D ropeTex = LoadTextureFromImage(ropeImg);
    UnloadImage(ropeImg);

    // Load player images (replace with your own PNGs)
    Image p1Img = LoadImage("456.png");
    Image p2Img = LoadImage("333.png");

    // If no images found, use colored rectangles instead
    bool p1ImgLoaded = (p1Img.width > 0);
    bool p2ImgLoaded = (p2Img.width > 0);

    Texture2D p1Tex, p2Tex;
    if (p1ImgLoaded) {
        p1Tex = LoadTextureFromImage(p1Img);
        UnloadImage(p1Img);
    }
    if (p2ImgLoaded) {
        p2Tex = LoadTextureFromImage(p2Img);
        UnloadImage(p2Img);
    }

    // Rope properties
    float ropeX = screenWidth / 2.0f;
    const float ropeWidth = (float)ropeTex.width;
    const float ropeHeight = (float)ropeTex.height;
    const float ropeY = screenHeight / 2.0f - ropeHeight / 2.0f;

    // Player properties
    const float playerY = ropeY + ropeHeight / 2.0f - 50; // center vertically
    const float playerWidth = 60;
    const float playerHeight = 100;

    // Pull counts and speed
    int player1Pulls = 0;
    int player2Pulls = 0;
    float ropeSpeed = 0.0f;
    const float pullForce = 2.5f;

    float pullResetTimer = 0.0f;
    bool gameOver = false;
    std::string winnerText = "";

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {
            pullResetTimer += dt;

            // Reset pulls every 0.5 seconds
            if (pullResetTimer >= 0.5f) {
                pullResetTimer = 0;
                player1Pulls = 0;
                player2Pulls = 0;
            }

            if (IsKeyPressed(KEY_A)) player1Pulls++;
            if (IsKeyPressed(KEY_L)) player2Pulls++;

            int diff = player2Pulls - player1Pulls;
            ropeSpeed = diff * pullForce;
            ropeX += ropeSpeed;

            // Check game over
            if (ropeX < ropeWidth / 2) {
                ropeX = ropeWidth / 2;
                gameOver = true;
                winnerText = "Player 1 Wins!";
            } else if (ropeX > screenWidth - ropeWidth / 2) {
                ropeX = screenWidth - ropeWidth / 2;
                gameOver = true;
                winnerText = "Player 2 Wins!";
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Player areas
        DrawRectangle(0, 0, screenWidth / 2, screenHeight, LIGHTGRAY);
        DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, LIGHTGRAY);

        // Player 1 label
        DrawText("Player 1", 50, 30, 30, RED);
        DrawText("Press 'A' rapidly!", 20, 70, 20, DARKGRAY);

        // Player 2 label
        DrawText("Player 2", screenWidth / 2 + 50, 30, 30, BLUE);
        DrawText("Press 'L' rapidly!", screenWidth / 2 + 20, 70, 20, DARKGRAY);

        // Center divider
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);

        // Draw rope
        DrawTexture(ropeTex, (int)(ropeX - ropeWidth / 2), (int)ropeY, WHITE);

        // Draw players
        float p1X = ropeX - ropeWidth / 2 - playerWidth - 10;
        float p2X = ropeX + ropeWidth / 2 + 10;

        if (p1ImgLoaded) {
            DrawTexturePro(p1Tex, {0, 0, (float)p1Tex.width, (float)p1Tex.height},
                           {p1X, playerY, playerWidth, playerHeight}, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangle(p1X, playerY, playerWidth, playerHeight, RED);
        }

        if (p2ImgLoaded) {
            DrawTexturePro(p2Tex, {0, 0, (float)p2Tex.width, (float)p2Tex.height},
                           {p2X, playerY, playerWidth, playerHeight}, {0, 0}, 0.0f, WHITE);
        } else {
            DrawRectangle(p2X, playerY, playerWidth, playerHeight, BLUE);
        }

        // Winner text
        if (gameOver) {
            int textWidth = MeasureText(winnerText.c_str(), 40);
            DrawText(winnerText.c_str(), screenWidth / 2 - textWidth / 2, screenHeight / 2 + 100, 40, BLACK);
            DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 150, 25, DARKGRAY);

            if (IsKeyPressed(KEY_R)) {
                ropeX = screenWidth / 2.0f;
                player1Pulls = 0;
                player2Pulls = 0;
                gameOver = false;
                winnerText = "";
                pullResetTimer = 0;
            }
        }

        EndDrawing();
    }

    UnloadTexture(ropeTex);
    if (p1ImgLoaded) UnloadTexture(p1Tex);
    if (p2ImgLoaded) UnloadTexture(p2Tex);

    CloseWindow();
    return 0;
}
