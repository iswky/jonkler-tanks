#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include "../SDL/SDL_render.h"

// font loading
TTF_Font* loadMainFont(App* app, int size);
TTF_Font* loadSmallFont(App* app, int size);

// text creation
RenderObject* createCenteredText(App* app, const char* text, TTF_Font* font,
                                 int y, SDL_Color color);
RenderObject* createCenteredTextWithTrigger(App* app, const char* text,
                                            TTF_Font* font, int y,
                                            SDL_Color normalColor,
                                            SDL_Color triggeredColor,
                                            enum Button buttonName);
RenderObject* createLeftAlignedText(App* app, const char* text, TTF_Font* font,
                                    int x, int y, SDL_Color color);
RenderObject* createRightAlignedText(App* app, const char* text, TTF_Font* font,
                                     int x, int y, SDL_Color color);

// button creation
RenderObject* createCenteredButton(App* app, const char* text, TTF_Font* font,
                                   int y, SDL_Color normalColor,
                                   SDL_Color triggeredColor,
                                   enum Button buttonName);
RenderObject* createBackButton(App* app, TTF_Font* font, int y);

// layout helpers
int getCenteredX(App* app, int width);
int getCenteredXForText(App* app, const char* text, TTF_Font* font);
int getLeftAlignedX(App* app, int width, int offset);
int getRightAlignedX(App* app, int width, int offset);

// color constants
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_GRAY;
extern const SDL_Color COLOR_RED;
extern const SDL_Color COLOR_GREEN;
extern const SDL_Color COLOR_BLUE;

// player difficulty selection
RenderObject* createPlayerDifficultyButton(App* app, const char* text,
                                           TTF_Font* font, int x, int y,
                                           enum Button buttonName);

#endif
