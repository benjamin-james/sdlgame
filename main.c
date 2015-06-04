#include "math.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "signal.h" /* sig_atomic_t */
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#define WIDTH 640
#define HEIGHT 480
#define NUM_OBJECTS 50
volatile sig_atomic_t running = 1;
volatile sig_atomic_t resize = 0;

struct object {
	double x,y,radius,dx,dy,mass;
	Uint32 color;
};

void init(SDL_Window **w, SDL_Renderer **r, struct object *objects);
void processEvent(SDL_Event e);
void logic(struct object *objects);
void render(SDL_Renderer *r, struct object *objects);
void handler(int i);
void collision(struct object *a, struct object *b);


int width, height;

int main(int argc, char **argv)
{
	SDL_Window *w;
	SDL_Renderer *r;
	SDL_Event e;
	struct object objects[NUM_OBJECTS];
	init(&w, &r, objects);
	while (running) {
		while (SDL_PollEvent(&e))
			processEvent(e);
		SDL_GetWindowSize(w, &width, &height);
		logic(objects);
		SDL_SetRenderDrawColor(r, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(r);
		render(r, objects);
		SDL_RenderPresent(r);
		SDL_Delay(50);
	}
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
	return 0;
}

void init(SDL_Window **w, SDL_Renderer **r, struct object *objects)
{
	int i;
	srand(time(0));
	signal(SIGINT, handler);
	SDL_Init(SDL_INIT_VIDEO);
	*w = SDL_CreateWindow("Ben's Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
	*r = SDL_CreateRenderer(*w, -1, 0);
	width = WIDTH;
	height = HEIGHT;
	for (i = 0; i < NUM_OBJECTS; i++) {
		int x = rand() % width;
		int y = rand() % height;
		int dx = rand() % 40 - 20;
		int dy = rand() % 40 - 20;
		objects[i].x = (double)x;
		objects[i].y = (double)y;
		objects[i].radius = rand() % 5 + 5;
		objects[i].dx = (double)dx / 5.00;
		objects[i].dy = (double)dy / 5.00;
		objects[i].color = (((rand() % 32768) << 16) | (rand() % 32768)) ^ 0xFF; /* SDL2_gfx uses RGBA */
	}

}
void processEvent(SDL_Event event)
{
	switch(event.type) {
	case SDL_WINDOWEVENT: switch(event.window.type) {
		case SDL_WINDOWEVENT_CLOSE:
			running = 0;
			return;
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			resize = 1;
			return;
		}
		break;
	case SDL_QUIT:
		running = 0;
		break;
	default:
		break;
	}
}

void logic(struct object *objects)
{
	int i,j;
	for (i = 0; i < NUM_OBJECTS; i++) {
		for (j = i + 1; j < NUM_OBJECTS; j++)
			collision(objects + i, objects + j);
		if (objects[i].x + objects[i].dx - objects[i].radius < 0.0)
			objects[i].dx = fabs(objects[i].dx);
		else if (objects[i].x + objects[i].dx + objects[i].radius > (double)width)
			objects[i].dx = -fabs(objects[i].dx);
		if (objects[i].y + objects[i].dy - objects[i].radius < 0.0)
			objects[i].dy = fabs(objects[i].dy);
		else if (objects[i].y + objects[i].dy + objects[i].radius > (double)height)
			objects[i].dy = -fabs(objects[i].dy);
		if (objects[i].x < 0.0)
			objects[i].x = 0.0;
		else if (objects[i].x > (double)width)
			objects[i].x = width - 1;
		if (objects[i].y < 0.0)
			objects[i].y = 0.0;
		else if (objects[i].y > (double)height)
			objects[i].y = height - 1;
		objects[i].x += objects[i].dx;
		objects[i].y += objects[i].dy;
	}
}

void render(SDL_Renderer *r, struct object *objects)
{
	int i;
	for (i = 0; i < NUM_OBJECTS; i++)
		filledCircleColor(r, objects[i].x, objects[i].y, objects[i].radius, objects[i].color);
}

void handler(int i)
{
	running = 0;
}

void collision(struct object *a, struct object *b)
{
	double dist2 = (a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y);
	double rad2 = (a->radius + b->radius) * (a->radius + b->radius);
	double dot, nx, ny, slope;
/* Test to see if they are moving toward each other with the dot product*/
	if (dist2 > rad2)
		return;
	dot = (a->x - b->x) * (b->dx - a->dx) + (a->y - b->y) * (b->dy - a->dx);
	if (dot <= 0.0)
		return;
	/* reflect the vectors: R = D - 2 (D . N) * N where n is the normal vector of the slope */
	slope = (a->y - b->y) / (a->x - b->x);
	nx = 1.0 / sqrt(1 + slope * slope); /* cos(arctan(dot)) */
	ny = slope / sqrt(1 + slope * slope); /* sin(arctan(dot)) */
	dot = a->dx * nx + a->dy * ny;
	a->dx += -2 * dot * nx;
	a->dy += -2 * dot * ny;
	dot = b->dx * nx + b->dy * ny;
	b->dx += -2 * dot * nx;
	b->dy += -2 * dot * ny;
}
