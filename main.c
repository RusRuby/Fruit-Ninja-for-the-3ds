// Fruit Slice 3D - a Fruit-Ninja-style game for Nintendo 3DS
// Built with libctru + citro2d
//
// Controls:
//   - Slide your finger/stylus across the BOTTOM screen touch pad
//     to slice fruit that flies up on the TOP screen... actually since
//     the 3DS only has one touch screen (bottom), all gameplay happens
//     there, and the bottom screen mirrors the action.
//   - START = quit
//   - A     = restart after Game Over

#include <citro2d.h>
#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MAX_FRUITS   24
#define SCREEN_W     320
#define SCREEN_H     240
#define GRAVITY      0.35f
#define START_LIVES  3

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    u32   color;
    bool  alive;
    bool  isBomb;
    bool  sliced;
} Fruit;

static Fruit fruits[MAX_FRUITS];
static int   score = 0;
static int   lives = START_LIVES;
static bool  gameOver = false;

static u32 fruitColors[5];

static void resetGame(void) {
    score = 0;
    lives = START_LIVES;
    gameOver = false;
    for (int i = 0; i < MAX_FRUITS; i++) fruits[i].alive = false;
}

static void spawnFruit(void) {
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (!fruits[i].alive) {
            fruits[i].alive  = true;
            fruits[i].sliced = false;
            fruits[i].x      = 30 + (rand() % (SCREEN_W - 60));
            fruits[i].y      = SCREEN_H + 10.0f;
            fruits[i].vx     = ((rand() % 100) - 50) / 25.0f;
            fruits[i].vy     = -(6.0f + (rand() % 30) / 10.0f);
            fruits[i].radius = 14 + (rand() % 10);
            fruits[i].isBomb = (rand() % 6 == 0);
            fruits[i].color  = fruits[i].isBomb
                                  ? C2D_Color32(40, 40, 40, 255)
                                  : fruitColors[rand() % 5];
            return;
        }
    }
}

// Distance from point (px,py) to the segment (x1,y1)-(x2,y2)
static bool pointNearSegment(float px, float py,
                              float x1, float y1,
                              float x2, float y2,
                              float threshold) {
    float dx = x2 - x1, dy = y2 - y1;
    float len2 = dx * dx + dy * dy;
    float t = 0.0f;
    if (len2 > 0.0001f) {
        t = ((px - x1) * dx + (py - y1) * dy) / len2;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
    }
    float cx = x1 + t * dx, cy = y1 + t * dy;
    float ddx = px - cx, ddy = py - cy;
    return (ddx * ddx + ddy * ddy) <= threshold * threshold;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* top    = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    fruitColors[0] = C2D_Color32(220, 30, 30, 255);   // apple red
    fruitColors[1] = C2D_Color32(255, 140, 0, 255);   // orange
    fruitColors[2] = C2D_Color32(240, 220, 20, 255);  // lemon
    fruitColors[3] = C2D_Color32(40, 180, 60, 255);   // lime
    fruitColors[4] = C2D_Color32(170, 60, 200, 255);  // grape

    C2D_TextBuf textBuf = C2D_TextBufNew(4096);

    touchPosition lastTouch = {0, 0};
    bool wasTouching = false;
    int  spawnTimer = 20;

    resetGame();

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
        if (kDown & KEY_START) break;

        touchPosition touch;
        hidTouchRead(&touch);
        bool touching = (kHeld & KEY_TOUCH) != 0;

        if (gameOver) {
            if (kDown & KEY_A) resetGame();
        } else {
            spawnTimer--;
            if (spawnTimer <= 0) {
                spawnFruit();
                spawnTimer = 24 + (rand() % 18);
            }

            for (int i = 0; i < MAX_FRUITS; i++) {
                if (!fruits[i].alive) continue;

                fruits[i].vy += GRAVITY;
                fruits[i].x  += fruits[i].vx;
                fruits[i].y  += fruits[i].vy;

                if (touching && wasTouching && !fruits[i].sliced) {
                    if (pointNearSegment(fruits[i].x, fruits[i].y,
                                          (float)lastTouch.px, (float)lastTouch.py,
                                          (float)touch.px, (float)touch.py,
                                          fruits[i].radius)) {
                        fruits[i].sliced = true;
                        fruits[i].alive  = false;
                        if (fruits[i].isBomb) {
                            lives = 0;
                        } else {
                            score += 10;
                        }
                    }
                }

                if (fruits[i].y > SCREEN_H + 40.0f) {
                    fruits[i].alive = false;
                    if (!fruits[i].isBomb && !fruits[i].sliced) {
                        lives--;
                    }
                }
            }

            if (lives <= 0) gameOver = true;
        }

        if (touching) { lastTouch = touch; wasTouching = true; }
        else wasTouching = false;

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // ---- TOP SCREEN ----
        C2D_TargetClear(top, C2D_Color32(15, 15, 35, 255));
        C2D_SceneBegin(top);

        for (int i = 0; i < MAX_FRUITS; i++) {
            if (fruits[i].alive) {
                C2D_DrawCircleSolid(fruits[i].x, fruits[i].y, 0.5f,
                                     fruits[i].radius, fruits[i].color);
                if (fruits[i].isBomb) {
                    C2D_DrawCircleSolid(fruits[i].x, fruits[i].y - fruits[i].radius,
                                         0.5f, 3.0f, C2D_Color32(255, 180, 0, 255));
                }
            }
        }

        char hud[64];
        snprintf(hud, sizeof(hud), "Score: %d   Lives: %d", score, lives);
        C2D_TextBufClear(textBuf);
        C2D_Text hudText;
        C2D_TextParse(&hudText, textBuf, hud);
        C2D_TextOptimize(&hudText);
        C2D_DrawText(&hudText, C2D_WithColor, 10.0f, 8.0f, 0.5f, 0.6f, 0.6f,
                      C2D_Color32(255, 255, 255, 255));

        if (gameOver) {
            char over[64];
            snprintf(over, sizeof(over), "GAME OVER  -  Score %d", score);
            C2D_TextBufClear(textBuf);
            C2D_Text overText;
            C2D_TextParse(&overText, textBuf, over);
            C2D_TextOptimize(&overText);
            C2D_DrawText(&overText, C2D_WithColor, 40.0f, 100.0f, 0.5f, 0.65f, 0.65f,
                          C2D_Color32(255, 60, 60, 255));

            C2D_TextBufClear(textBuf);
            C2D_Text retryText;
            C2D_TextParse(&retryText, textBuf, "Press A to play again");
            C2D_TextOptimize(&retryText);
            C2D_DrawText(&retryText, C2D_WithColor, 60.0f, 130.0f, 0.5f, 0.5f, 0.5f,
                          C2D_Color32(255, 255, 255, 255));
        }

        // ---- BOTTOM SCREEN (touch pad, mirrors slicing area) ----
        C2D_TargetClear(bottom, C2D_Color32(10, 10, 25, 255));
        C2D_SceneBegin(bottom);

        C2D_TextBufClear(textBuf);
        C2D_Text hint;
        C2D_TextParse(&hint, textBuf, "Slide here to slice fruit!");
        C2D_TextOptimize(&hint);
        C2D_DrawText(&hint, C2D_WithColor, 40.0f, 110.0f, 0.5f, 0.55f, 0.55f,
                      C2D_Color32(200, 200, 255, 255));

        C3D_FrameEnd(0);
    }

    C2D_TextBufDelete(textBuf);
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
