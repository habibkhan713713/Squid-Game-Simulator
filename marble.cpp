#include <raylib.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

bool isOdd(int num) { return num % 2 != 0; }

struct Button {
    Rectangle rect;
    string text;
    bool hovered = false;
    float lastClickTime = 0;
    const float clickDelay = 0.15f;

    Button(float x, float y, float w, float h, const string& t)
        : rect{x, y, w, h}, text(t) {}

    void UpdateHover() {
        Vector2 mouse = GetMousePosition();
        hovered = CheckCollisionPointRec(mouse, rect);
    }

    bool IsClicked() {
        float now = GetTime();
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
            (now - lastClickTime) > clickDelay) {
            lastClickTime = now;
            return true;
        }
        return false;
    }

    void Draw() const {
        Color col = hovered ? DARKGRAY : LIGHTGRAY;
        DrawRectangleRec(rect, col);
        int textWidth = MeasureText(text.c_str(), 22);
        DrawText(text.c_str(),
                 (int)(rect.x + rect.width / 2 - textWidth / 2),
                 (int)(rect.y + rect.height / 2 - 12), 22, BLACK);
    }
};

struct PlayerAI {
    int marbles;
    bool specialUsed;
    vector<int> playerPutsHistory;

    PlayerAI(int start) : marbles(start), specialUsed(false) {}

    bool guessOdd() {
        if (playerPutsHistory.empty()) return rand() % 2 == 0;
        int oddCount = count_if(playerPutsHistory.begin(), playerPutsHistory.end(), [](int n){ return n % 2 != 0; });
        int evenCount = (int)playerPutsHistory.size() - oddCount;
        return oddCount >= evenCount;
    }

    int putMarbles() {
        return min(marbles, (rand() % 5) + 1);
    }
};

void DrawBar(float x, float y, float width, float height, int value, int maxVal, Color fill) {
    DrawRectangle((int)x, (int)y, (int)width, (int)height, GRAY);
    float fillWidth = (value / (float)maxVal) * width;
    DrawRectangle((int)x, (int)y, (int)fillWidth, (int)height, fill);
    DrawRectangleLines((int)x, (int)y, (int)width, (int)height, BLACK);
}

void CreateBetButtons(vector<Button> &betButtons, int maxBet, int maxAllowed, int yPos, float w = 80, float h = 50) {
    betButtons.clear();
    for (int i = 1; i <= maxBet && i <= maxAllowed; i++) {
        betButtons.emplace_back(100 + (i - 1) * (w + 10), yPos, w, h, to_string(i));
    }
}

int main() {
    srand(time(nullptr));

    const int screenWidth = 900;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Squid Game - Marbles Challenge");
    SetTargetFPS(60);

    const int startMarbles = 10;
    const int maxBet = 5;

    PlayerAI ai(startMarbles);
    int playerMarbles = startMarbles;
    bool playerTurn = true;
    bool roundOver = false;
    bool specialMoveActive = false;
    bool playerUsedSpecial = false;

    int currentBet = 0;
    bool betPlaced = false;
    bool guessMade = false;
    bool aiTurnWait = false;
    bool aiTurnResultShow = false;

    enum GuessChoice { NONE, ODD, EVEN };
    GuessChoice playerGuess = NONE;

    int playerPutMarbles = 0;

    string narration = "Your turn! Guess if AI's marbles are Odd or Even.";

    Button oddBtn(100, 450, 160, 55, "Guess Odd");
    Button evenBtn(300, 450, 160, 55, "Guess Even");
    Button specialBtn(550, 450, 220, 55, "Use Double Bet (Once)");

    vector<Button> betButtons;
    int maxPossibleBet = maxBet;

    while (!WindowShouldClose()) {
        // --- INPUT HANDLING ---
        oddBtn.UpdateHover();
        evenBtn.UpdateHover();
        specialBtn.UpdateHover();
        for (auto& b : betButtons) b.UpdateHover();

        if (!roundOver) {
            if (playerTurn) {
                if (playerGuess == NONE) {
                    if (oddBtn.IsClicked()) {
                        playerGuess = ODD;
                        narration = "You guessed ODD. Now place your bet.";
                        maxPossibleBet = playerUsedSpecial ? maxBet * 2 : maxBet;
                        CreateBetButtons(betButtons, maxPossibleBet, playerMarbles, 530);
                    } else if (evenBtn.IsClicked()) {
                        playerGuess = EVEN;
                        narration = "You guessed EVEN. Now place your bet.";
                        maxPossibleBet = playerUsedSpecial ? maxBet * 2 : maxBet;
                        CreateBetButtons(betButtons, maxPossibleBet, playerMarbles, 530);
                    } else if (specialBtn.IsClicked() && !playerUsedSpecial) {
                        playerUsedSpecial = true;
                        specialMoveActive = true;
                        narration = "Special move activated! Bet will be doubled this turn.";
                    }
                } else if (!betPlaced) {
                    for (auto& b : betButtons) {
                        if (b.IsClicked()) {
                            currentBet = stoi(b.text);
                            if (specialMoveActive) currentBet *= 2;
                            betPlaced = true;
                            narration = "Bet placed: " + to_string(currentBet) + ". Waiting for AI's move...";
                            aiTurnWait = true;
                            specialMoveActive = false;
                            betButtons.clear();
                        }
                    }
                }
            } else {
                if (!guessMade) {
                    if (betButtons.empty()) {
                        maxPossibleBet = min(maxBet, playerMarbles);
                        CreateBetButtons(betButtons, maxPossibleBet, playerMarbles, 460);
                    }
                    for (auto& b : betButtons) {
                        if (b.IsClicked()) {
                            playerPutMarbles = stoi(b.text);
                            guessMade = true;
                            narration = "You placed " + to_string(playerPutMarbles) + " marbles. AI is guessing...";
                            betButtons.clear();
                        }
                    }
                }
            }
        } else if (IsKeyPressed(KEY_R)) {
            // Reset game
            playerMarbles = startMarbles;
            ai.marbles = startMarbles;
            playerTurn = true;
            roundOver = false;
            playerGuess = NONE;
            betPlaced = false;
            guessMade = false;
            narration = "Your turn! Guess if AI's marbles are Odd or Even.";
            playerUsedSpecial = false;
            ai.playerPutsHistory.clear();
            currentBet = 0;
            aiTurnWait = false;
            aiTurnResultShow = false;
            betButtons.clear();
        }

        // --- GAME LOGIC ---
        if (aiTurnWait) {
            static int timer = 0;
            timer++;
            if (timer > 90) {
                timer = 0;
                int aiPut = ai.putMarbles();
                narration = "AI put " + to_string(aiPut) + " marbles.";

                bool playerGuessOdd = (playerGuess == ODD);
                if (playerGuessOdd == isOdd(aiPut)) {
                    narration += " You guessed right! You win marbles.";
                    playerMarbles += currentBet;
                    ai.marbles -= currentBet;
                } else {
                    narration += " Wrong guess! You lose marbles.";
                    playerMarbles -= currentBet;
                    ai.marbles += currentBet;
                }

                betPlaced = false;
                playerGuess = NONE;
                aiTurnWait = false;
                playerTurn = false;

                if (playerMarbles <= 0 || ai.marbles <= 0) roundOver = true;
            }
        }

        if (!playerTurn && guessMade && !aiTurnResultShow) {
            int aiGuessOdd = ai.guessOdd();
            narration = "AI guesses your marbles are " + string(aiGuessOdd ? "Odd." : "Even.");

            if (aiGuessOdd == isOdd(playerPutMarbles)) {
                narration += " AI guessed right! It wins marbles.";
                playerMarbles -= playerPutMarbles;
                ai.marbles += playerPutMarbles;
            } else {
                narration += " AI guessed wrong! You win marbles.";
                playerMarbles += playerPutMarbles;
                ai.marbles -= playerPutMarbles;
            }

            ai.playerPutsHistory.push_back(playerPutMarbles);
            guessMade = false;
            aiTurnResultShow = true;

            if (playerMarbles <= 0 || ai.marbles <= 0) roundOver = true;
        }

        if (aiTurnResultShow) {
            static int timer2 = 0;
            timer2++;
            if (timer2 > 120) {
                timer2 = 0;
                aiTurnResultShow = false;
                playerTurn = true;
                narration = "Your turn! Guess if AI's marbles are Odd or Even.";
            }
        }

        // --- DRAW ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Your Marbles", 100, 50, 24, BLACK);
        DrawBar(100, 85, 250, 35, playerMarbles, startMarbles, BLUE);

        DrawText("AI Marbles", 500, 50, 24, BLACK);
        DrawBar(500, 85, 250, 35, ai.marbles, startMarbles, RED);

        DrawText(narration.c_str(), 50, 150, 22, DARKGRAY);

        if (!roundOver) {
            if (playerTurn) {
                if (playerGuess == NONE) {
                    DrawText("Select your guess:", 100, 400, 22, BLACK);
                    oddBtn.Draw();
                    evenBtn.Draw();
                    if (!playerUsedSpecial) specialBtn.Draw();
                } else if (!betPlaced) {
                    DrawText("Select your bet:", 100, 500, 22, BLACK);
                    for (auto& b : betButtons) b.Draw();
                }
            } else if (!guessMade) {
                DrawText("Choose how many marbles to put forward:", 100, 400, 22, BLACK);
                for (auto& b : betButtons) b.Draw();
            }
        } else {
            DrawText("Round Over!", 350, 300, 40, DARKGREEN);
            DrawText(narration.c_str(), 200, 350, 25, BLACK);
            DrawText("Press R to restart.", 300, 400, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
