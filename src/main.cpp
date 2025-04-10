
// TODO: add multiple more objects and process with quadtree
// TODO: add mouse drawing wall

#include <bits/stdc++.h>
#include <SDL.h>
#include "QuadTree.h"
#include "Circle.h"
#include "PhysicEngine.h"

using std::cout, std::cerr, std::endl, std::string, std::ceil, std::floor, std::vector, std::round, std::abs, std::sqrt, std::atan2, std::pow, std::sin, std::cos, std::acos, std::rand, std::queue, std::stack, HuyNVector::Vector2, std::get, std::move, std::visit, std::decay_t, std::is_same_v;

using namespace HuyNPhysic;

#define HuyN_ int main(int argc, char *argv[])

class SDLException final : public std::runtime_error {
public:
    explicit SDLException(const std::string& message) : std::runtime_error(message + "\n" + SDL_GetError()) {
        cerr << "SDL Error: " << message << endl;
    }
};


// GLOBAL VARIABLE

uint64_t FrameUpdateInterval = 10, // ms
         LatestUpdatedTick = 0,
         CurrentTick;

struct Size {
    int w;
    int h;
};

struct Pos {
    int x;
    int y;
};

Size WindowSize{1280, 720};
constexpr Size WindowMinSize{640, 480};



QuadTree::QuadTree Q{Shape::Box<double>{10, 10, static_cast<double>(WindowSize.w - 20), static_cast<double>(WindowSize.h - 20)}};



int iDistance_From_Bottom_To_Floor = 40,
    iFloor = WindowSize.h - iDistance_From_Bottom_To_Floor;


constexpr Vector2<double> GravitationForce{0, 9.8};

struct objectsProperties {
    double radius{};
    Vector2<double> position{};
    Vector2<double> velocity; // pixel per updateInterval
    Vector2<double> acceleration;
    double mass{};
    char type;
    queue<Vector2<double>> Trail;
};

vector<Object<double>> objects;

// FUNCTIONS

static int resizingEventWatcher(void* data, const SDL_Event* event) {
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
        if (const SDL_Window* win = SDL_GetWindowFromID(event->window.windowID); win == static_cast<SDL_Window *>(data)) {
            WindowSize.w = event->window.data1;
            WindowSize.h = event->window.data2;
            iFloor = WindowSize.h - iDistance_From_Bottom_To_Floor;
        }
        }
    return 0;
}


void DrawObjects(SDL_Renderer *renderer) {
    for (const auto& o : objects) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
        visit([&](const auto& shape) {
            if (is_same_v<decay_t<decltype(shape)>, Shape::Circle<double>>) {
                Shape::SDL_RenderFillCircle(renderer, static_cast<int>(o.x), static_cast<int>(o.y), static_cast<int>(shape.radius));
            } else if (is_same_v<decay_t<decltype(shape)>, Shape::Box<double>>) {
                Shape::Box<double>{o.x - shape.width / 2, o.y - shape.height / 2, shape.width, shape.height}.SDL_FillBox(renderer);
            }
        }, o.shape);
    }
}

void Simulate(SDL_Renderer *renderer) {
    for (auto& obj : objects) {
        // Apply physics step with friction if on floor
        bool onFloor = false;
        // std::visit([&](const auto& shape) {
        //     T halfExtent = std::is_same_v<std::decay_t<decltype(shape)>, Shape::Circle<double>> ? shape.radius : shape.height / 2;
        //     onFloor = (obj.y + halfExtent >= 1.0 * iFloor);
        // }, obj.shape);

        CurrentTick = SDL_GetTicks();

        obj.PhysicStep(CurrentTick - LatestUpdatedTick, onFloor, 0.3);

        // Handle boundaries (minX, maxX, minY, maxY)
        obj.handleBoundaries(0, WindowSize.w, 0, iFloor);
    }

    // Check collisions between all pairs (for simplicity, just the first two here)
    if (CheckCollide(objects[0], objects[1])) {
        CollisionProcess(&objects[0], &objects[1]);
        cout << 0.5 * objects[0].mass * objects[0].velocity.pow(2).magnitude() + 0.5 * objects[1].mass * objects[1].velocity.pow(2).magnitude() << endl;
    }

    LatestUpdatedTick = SDL_GetTicks();
    DrawObjects(renderer);
}

HuyN_ {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        throw SDLException("Failed to initialize SDL");
    }

    SDL_Window *window{SDL_CreateWindow("HuyN's Physic Testing Sandbox", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowSize.w, WindowSize.h, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE)};
    if (window == nullptr) {
        throw SDLException("Failed to create window");
    }

    SDL_SetWindowMinimumSize(window, WindowMinSize.w, WindowMinSize.h);

    SDL_Renderer *renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if (renderer == nullptr) {
        throw SDLException("Failed to create renderer");
    }

    SDL_ShowWindow(window);

    SDL_AddEventWatch(reinterpret_cast<SDL_EventFilter>(resizingEventWatcher), window);

    SDL_Event event;
    bool isRunning{true};

    objects.push_back(HuyNPhysic::Object<double>{
        Vector2<double>{200, 200},
        40,
        Shape::Circle<double>{
            Vector2<double>{200, 200},
            100,
        },
        Vector2<double>{0, 320},
        Vector2<double>{0, 0},
    });
    // objects.push_back(HuyNPhysic::Object<double>{
    //     Vector2<double>{200, 200},
    //     55,
    //     Shape::Circle<double>{
    //         Vector2<double>{200, 200},
    //         120,
    //     },
    //     Vector2<double>{220, 250},
    //     Vector2<double>{0, 0},
    // });

    objects.push_back(HuyNPhysic::Object<double>{
        Vector2<double>{600, 200},
        40,
        Shape::Circle<double>{
            Vector2<double>{600, 200},
            // Vector2<double>{100, 100}
            75
        },
        Vector2<double>{320, 240},
        Vector2<double>{0, 0},
    });

    // for (auto& o : objects) {
    //     o.acceleration += GravitationForce;
    // }

    while (isRunning) {

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {}
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawLine(renderer, 0, iFloor, WindowSize.w, iFloor);

        Pos LinesX = {0, 5};

        while (LinesX.y < WindowSize.w) {
            SDL_RenderDrawLine(renderer, LinesX.x, iFloor, LinesX.y, iFloor + 10);
            LinesX.x += 10; LinesX.y += 10;
        }

        // if (CurrentTick = SDL_GetTicks(); CurrentTick - LatestUpdatedTick >= FrameUpdateInterval) {
        //     Simulate(renderer);
        //     LatestUpdatedTick = CurrentTick;
        // }

        Simulate(renderer);

        SDL_RenderPresent(renderer);

    }

    return EXIT_SUCCESS;
}