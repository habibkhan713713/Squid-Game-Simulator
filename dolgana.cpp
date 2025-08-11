#include <raylib.h>
#include <cstdio>

#ifndef DARKRED
#define DARKRED CLITERAL(Color){139, 0, 0, 255}
#endif

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Only consider nearly-black pixels as "dark"
bool IsDark(Color c, int threshold = 10) {
    int avg = (c.r + c.g + c.b) / 3;
    return avg < threshold;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Trace the Boundary");
    SetTargetFPS(60);

    // Load images
    Image boundaryImg = LoadImage("triangle_boundary.png"); // big black boundary
    Image innerImg    = LoadImage("triangle.png");          // smaller bright triangle

    if (!boundaryImg.data || !innerImg.data) {
        printf("Error: could not load images.\n");
        CloseWindow();
        return -1;
    }

    // Convert images to a known pixel format before pixel access
    ImageFormat(&boundaryImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageFormat(&innerImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    int imgWidth = boundaryImg.width;
    int imgHeight = boundaryImg.height;

    // Get pointer to boundary pixels
    Color *boundaryPixels = LoadImageColors(boundaryImg);

    // Allocate pixelVisited dynamically on the heap, initialized to false
    bool* pixelVisited = new bool[imgWidth * imgHeight]();

    // Textures for drawing
    Texture2D boundaryTex = LoadTextureFromImage(boundaryImg);
    Texture2D innerTex    = LoadTextureFromImage(innerImg);

    // Position images centered
    Vector2 boundaryPos = {
        (SCREEN_WIDTH - imgWidth) / 2.0f,
        (SCREEN_HEIGHT - imgHeight) / 2.0f
    };
    Vector2 innerPos = {
        (SCREEN_WIDTH - innerImg.width) / 2.0f,
        (SCREEN_HEIGHT - innerImg.height) / 2.0f
    };

    int progressCount = 0;
    int crackCount = 0;
    const int maxCracks = 10;

    while (!WindowShouldClose()) {
        bool isPressing = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        Vector2 mouse = GetMousePosition();

        if (isPressing) {
            // Mouse relative to boundary image position
            int px = (int)(mouse.x - boundaryPos.x);
            int py = (int)(mouse.y - boundaryPos.y);

            if (px >= 0 && px < imgWidth &&
                py >= 0 && py < imgHeight) {

                int idx = py * imgWidth + px;
                if (!pixelVisited[idx]) {
                    pixelVisited[idx] = true;
                    Color underPixel = boundaryPixels[idx];
                    if (IsDark(underPixel)) {
                        progressCount++;
                    } else {
                        crackCount++;
                    }
                }
            } else {
                crackCount++;
            }
        }

        float progress = (float)progressCount / 200.0f; // adjust difficulty

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(boundaryTex, (int)boundaryPos.x, (int)boundaryPos.y, WHITE); // dark boundary
        DrawTexture(innerTex, (int)innerPos.x, (int)innerPos.y, WHITE);          // bright inner triangle

        DrawText(TextFormat("Progress: %.1f%%", progress * 100.0f), 10, 10, 20, DARKGREEN);
        DrawText(TextFormat("Cracks: %d / %d", crackCount, maxCracks), 10, 40, 20, RED);

        if (progress >= 1.0f) {
            DrawText("You Win!", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, 40, DARKGREEN);
        } else if (crackCount >= maxCracks) {
            DrawText("You Lost!", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, 40, DARKRED);
        }

        EndDrawing();

        if (progress >= 1.0f || crackCount >= maxCracks)
            break;
    }

    // End screen
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if ((float)progressCount / 200.0f >= 1.0f)
            DrawText("You Win!", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, 40, DARKGREEN);
        else
            DrawText("You Lost!", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, 40, DARKRED);
        EndDrawing();
    }

    // Cleanup
    delete[] pixelVisited;
    UnloadImageColors(boundaryPixels);
    UnloadTexture(boundaryTex);
    UnloadTexture(innerTex);
    UnloadImage(boundaryImg);
    UnloadImage(innerImg);
    CloseWindow();

    return 0;
}
