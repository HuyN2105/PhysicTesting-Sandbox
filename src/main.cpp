
// TODO: add multiple more objects and process with quadtree
// TODO: add mouse drawing wall

#include <bits/stdc++.h>
#include <SDL.h>
#include <QuadTree.h>
#include <Circle.h>

using std::cout, std::cerr, std::endl, std::string, std::ceil, std::floor, std::vector, std::round, std::abs, std::sqrt, std::atan2, std::pow, std::sin, std::cos, std::acos, std::rand, std::queue, std::stack, HuyNVector::Vector2;

#define HuyN_ int main(int argc, char *argv[])

#define PI 3.14159265358979323846

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
    double angularVelocity{};
    double angularAcceleration{};
    queue<Vector2<double>> Trail;
};

vector<objectsProperties> objects;

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
    // Trail
    for (auto & o : objects) {
        queue<Vector2<double>> tempTrail = o.Trail;
        stack<Vector2<double>> DrawTrail;
        while (!tempTrail.empty()) {
            DrawTrail.push(tempTrail.front());
            tempTrail.pop();
        }

        int offsetColor = 0;

        while (!DrawTrail.empty()) {
            SDL_SetRenderDrawColor(renderer, 0xFF - offsetColor, 0xFF - offsetColor, 0xFF - offsetColor, 255);
            Shape::SDL_RenderFillCircle(renderer, static_cast<int>(DrawTrail.top().x), static_cast<int>(DrawTrail.top().y), static_cast<int>(o.radius));
            DrawTrail.pop();
            offsetColor += 12;
        }
        if (o.Trail.size() > 10) o.Trail.pop();
    }

    // Main Object
    for (auto & o : objects) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
        Shape::SDL_RenderFillCircle(renderer, static_cast<int>(o.position.x), static_cast<int>(o.position.y), static_cast<int>(o.radius));
    }
}

void Simulate(SDL_Renderer *renderer) {

    if (objects[0].position.x - objects[0].radius <= 0 || objects[0].position.x + objects[0].radius >= WindowSize.w) {
        objects[0].velocity.x *= -1;
    }
    if (objects[0].position.y - objects[0].radius <= 0 || objects[0].position.y + objects[0].radius >= 1.0 * iFloor) {
        objects[0].velocity.y *= -1;
    }
    if (objects[1].position.x - objects[1].radius <= 0 || objects[1].position.x + objects[1].radius >= WindowSize.w) {
        objects[1].velocity.x *= -1;
    }
    if (objects[1].position.y - objects[1].radius <= 0 || objects[1].position.y + objects[1].radius >= 1.0 * iFloor) {
        objects[1].velocity.y *= -1;
    }

    if (objects[1].position.distance(objects[0].position) <= objects[1].radius + objects[0].radius) {
        const double vMassSum = objects[0].mass + objects[1].mass;
        Vector2<double> vDiff = objects[1].velocity - objects[0].velocity;
        Vector2<double> vPosSub = objects[1].position - objects[0].position;
        const double vDist = vPosSub.magnitude();
        objects[0].velocity += (2 * objects[1].mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub;
        vDiff = -1.0 * vDiff;
        vPosSub = -1.0 * vPosSub;
        objects[1].velocity += (2 * objects[0].mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub;
    }

    // Acceleration

    objects[0].velocity += (static_cast<double>(CurrentTick - LatestUpdatedTick) / 1000) * objects[0].acceleration;
    objects[1].velocity += (static_cast<double>(CurrentTick - LatestUpdatedTick) / 1000) * objects[1].acceleration;

    // Trail

    objects[0].Trail.push(objects[0].position);
    objects[1].Trail.push(objects[1].position);

    objects[0].position += objects[0].velocity;
    objects[1].position += objects[1].velocity;
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

    objects.push_back({
        100,
        Vector2<double>{400, static_cast<double>(iFloor - 100 - 1)},
        Vector2<double>{6, 2},
        Vector2<double>{0, 0},
        40,
        0,
        0
    });

    objects.push_back({
        75,
        Vector2<double>{600, static_cast<double>(iFloor - 100 - 1)},
        Vector2<double>{5, 1},
        Vector2<double>{0, 0},
        40,
        0,
        0
    });

    for (auto& o : objects) {
        o.acceleration += GravitationForce;
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
        
        if (CurrentTick = SDL_GetTicks(); CurrentTick - LatestUpdatedTick >= FrameUpdateInterval) {
            Simulate(renderer);
            LatestUpdatedTick = CurrentTick;
        }

        SDL_RenderPresent(renderer);

    }

    return EXIT_SUCCESS;
}