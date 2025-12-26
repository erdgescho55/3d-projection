#include <SDL3/SDL.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 60
#define FRAME_TIME 1000 / FPS

typedef struct
{
  float x;
  float y;
} Vec2;

typedef struct
{
  float x;
  float y;
  float z;
} Vec3;

void loop(SDL_Renderer *r);
void drawPoint(SDL_Renderer *r, Vec2 point);

Vec2 convertToScreenCoordinates(Vec2 point);
Vec2 projectToScreen(Vec3 point);

Vec3 translate_z(Vec3 point, float dz);
Vec3 rotate_y(Vec3 point, float angle);

int main()
{
  if (!SDL_Init(0))
  {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());

    return 1;
  }

  SDL_Window *w =
      SDL_CreateWindow("Demo x'=x/z", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!w)
  {
    SDL_Log("SDL_CreateWindow creation failed %s", SDL_GetError());
    SDL_Quit();

    return 1;
  }

  SDL_Renderer *r = SDL_CreateRenderer(w, NULL);
  if (!r)
  {
    SDL_Log("SDL_CreateRenderer failed %s", SDL_GetError());
    SDL_DestroyWindow(w);
    SDL_Quit();

    return 1;
  }

  loop(r);

  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();

  return 0;
}

void loop(SDL_Renderer *r)
{
  bool quit = false;
  SDL_Event event;

  float dt = 1. / FPS;

  float dz = 0;
  float angle = 0;

  Vec3 vs[] = {
      {0.5, 0.5, 0.5},    {-0.5, 0.5, 0.5},
      {-0.5, -0.5, 0.5},  {0.5, -0.5, 0.5},

      {0.5, 0.5, -0.5},   {-0.5, 0.5, -0.5},
      {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
  };
  size_t vs_len = sizeof(vs) / sizeof(*vs);

  while (!quit)
  {
    Uint64 frame_start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        quit = true;
      }
    }

    dz += 1 * dt;
    angle += M_PI * dt;

    SDL_SetRenderDrawColor(r, 24, 24, 24, 255);
    SDL_RenderClear(r);

    for (int i = 0; i < vs_len; i++)
    {
      drawPoint(r, convertToScreenCoordinates(projectToScreen(
                       translate_z(rotate_y(vs[i], angle), dz))));
    }

    SDL_RenderPresent(r);

    Uint64 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < FRAME_TIME)
    {
      SDL_Delay(FRAME_TIME - frame_time);
    }
  }
}

void drawPoint(SDL_Renderer *r, Vec2 point)
{
  float size = 20;

  SDL_SetRenderDrawColor(r, 80, 255, 80, 255);
  SDL_RenderFillRect(
      r, &(SDL_FRect){point.x - size / 2, point.y - size / 2, size, size});
}

Vec2 convertToScreenCoordinates(Vec2 point)
{
  // -1..1 => 0..2 => 0..1 => 0..w/h

  return (Vec2){
      .x = (point.x + 1) / 2 * WINDOW_WIDTH,
      .y = (1 - (point.y + 1) / 2) * WINDOW_HEIGHT,
  };
}

Vec2 projectToScreen(Vec3 point)
{
  return (Vec2){
      .x = point.x / point.z,
      .y = point.y / point.z,
  };
}

Vec3 translate_z(Vec3 point, float dz)
{
  return (Vec3){
      .x = point.x,
      .y = point.y,
      .z = point.z + dz,
  };
}

Vec3 rotate_y(Vec3 point, float angle)
{
  /*
    x' = x cos θ − z sin θ
    z' = x sin θ + z cos θ
  */

  float c = cosf(angle);
  float s = sinf(angle);

  return (Vec3){
      .x = point.x * c - point.z * s,
      .y = point.y,
      .z = point.x * s + point.z * c,
  };
}
