
// RULE: 1px = 10cm irl


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


constexpr Vector2<double> Gravitational_Acceleration{0, 98};

struct objectsProperties {
    double radius{};
    Vector2<double> position{};
    Vector2<double> velocity; // pixel per updateInterval
    Vector2<double> ApplyingForces;
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

        char shapeType = o.shape->getType();
        if (shapeType == 'c') {
            const auto* circle = dynamic_cast<Shape::Circle<double>*>(o.shape);
            Shape::SDL_RenderFillCircle(renderer, static_cast<int>(o.x), static_cast<int>(o.y),
                                       static_cast<int>(circle->radius));
        } else if (shapeType == 'b') {
            const auto* box = dynamic_cast<Shape::Box<double>*>(o.shape);
            Shape::Box<double>{o.x - box->width / 2, o.y - box->height / 2,
                              box->width, box->height}.SDL_FillBox(renderer);
        }
    }
}

void Simulate(SDL_Renderer *renderer) {
    for (auto& obj : objects) {
        // Apply physics step with friction if on floor
        bool onFloor = false;
        char shapeType = obj.shape->getType();
        if (shapeType == 'c') {
            auto* circle = dynamic_cast<Shape::Circle<double>*>(obj.shape);
            onFloor = (obj.y + circle->radius >= 1.0 * iFloor);
        } else if (shapeType == 'b') {
            auto* box = dynamic_cast<Shape::Box<double>*>(obj.shape);
            onFloor = (obj.y + box->height/2 >= 1.0 * iFloor);
        }

        CurrentTick = SDL_GetTicks();
        obj.PhysicStep(FrameUpdateInterval, onFloor, 0.3);
        obj.handleBoundaries(0, WindowSize.w, 0, iFloor);
    }

    // TODO: applying quadtree

    for (int i = 0; i < objects.size() - 1; i++) {
        for (int j = i + 1; j < objects.size(); j++) {
            if (CheckCollide(objects[i], objects[j])) {
                CollisionProcess(&objects[i], &objects[j]);
            }
        }
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

    int BallsAmount = 5;

    while (BallsAmount--) {
        auto randX = static_cast<double>(rand() % WindowSize.w - 100 + 100),
        randY = static_cast<double>(rand() % WindowSize.h - 100 + 100),
        randRadius = static_cast<double>(rand() % 100 + 50);

        bool containsCheck = true;
        while (containsCheck) {
            containsCheck = false;
            for (const auto& obj : objects) {
                if (obj.shape->getType() == 'c') {
                    auto* circle = dynamic_cast<Shape::Circle<double>*>(obj.shape);
                    if (Vector2{obj.x, obj.y}.distance(Vector2{randX, randY}) <= circle->radius + randRadius) {
                        randX = static_cast<double>(rand() % WindowSize.w - 100 + 100);
                        randY = static_cast<double>(rand() % WindowSize.h - 100 + 100);
                        containsCheck = true;
                        break;
                    }
                }
            }
        }

        // Create a circle shape
        auto* circleShape = new Shape::Circle<double>{
            Vector2{randX, randY},
            randRadius
        };

        objects.emplace_back(
            randX, randY,
            0.0, // Will set mass based on shape area
            circleShape,
            randX, randY,
            0, 0
        );

        auto& lastObject = objects.back();
        if (lastObject.shape->getType() == 'c') {
            lastObject.mass = lastObject.shape->area() / 1000;
        }
    }

    for (auto& o : objects) {
        // Newton's 2nd law: F = ma
        o.ApplyingForce(Gravitational_Acceleration);
    }

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
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_SPACE:
                            for (auto& o : objects) {
                                o.ApplyingForce(Vector2<double>{0, -1 * static_cast<double>(WindowSize.h / 2 / 10)});
                                // TODO: fix this
                            }
                            break;
                        default:
                            break;
                    }
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}