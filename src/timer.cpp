#include <SDL.h>
#include "../include/timer.hpp"

using Clock = std::chrono::high_resolution_clock;

void Timer::init() {
    timer = Clock::now();
}

void Timer::reset() {
    timer = Clock::now();
}

float Timer::getTime() {
    auto now = Clock::now();
	float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::seconds>(now - timer)).count();
    return elapsed_sec;
}

// void Timer::draw() {
//     TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24); //this opens a font style and sets a size

//     SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

//     SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

//     SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture

//     SDL_Rect Message_rect; //create a rect
//     Message_rect.x = 0;  //controls the rect's x coordinate 
//     Message_rect.y = 0; // controls the rect's y coordinte
//     Message_rect.w = 100; // controls the width of the rect
//     Message_rect.h = 100; // controls the height of the rect

//     //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

//     //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

//     SDL_RenderCopy(renderer, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

//     //Don't forget too free your surface and texture
// }