
// TODO: add multiple more objects and process with quadtree
// TODO: add mouse drawing wall

#include <bits/stdc++.h>
#include <SDL.h>

using std::cout, std::cerr, std::endl, std::string, std::ceil, std::floor, std::vector, std::round, std::abs, std::sqrt, std::atan2, std::pow, std::sin, std::cos, std::acos, std::rand, std::queue, std::stack;

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

int
SDL_RenderDrawCircle(SDL_Renderer * renderer,const int x,const int y,const int radius)
{
    int offsetX = 0;
    int offsetY = radius;
    int d = radius - 1;
    int status = 0;

    while (offsetY >= offsetX) {
        status += SDL_RenderDrawPoint(renderer, x + offsetX, y + offsetY);
        status += SDL_RenderDrawPoint(renderer, x + offsetY, y + offsetX);
        status += SDL_RenderDrawPoint(renderer, x - offsetX, y + offsetY);
        status += SDL_RenderDrawPoint(renderer, x - offsetY, y + offsetX);
        status += SDL_RenderDrawPoint(renderer, x + offsetX, y - offsetY);
        status += SDL_RenderDrawPoint(renderer, x + offsetY, y - offsetX);
        status += SDL_RenderDrawPoint(renderer, x - offsetX, y - offsetY);
        status += SDL_RenderDrawPoint(renderer, x - offsetY, y - offsetX);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetX) {
            d -= 2*offsetX + 1;
            offsetX +=1;
        }
        else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY -= 1;
        }
        else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY -= 1;
            offsetX += 1;
        }
    }

    return status;
}

int
SDL_RenderFillCircle(SDL_Renderer * renderer, const int x, const int y, const int radius)
{
    int offsetX = 0;
    int offsetY = radius;
    int d = radius - 1;
    int status = 0;

    while (offsetY >= offsetX) {

        status += SDL_RenderDrawLine(renderer, x - offsetY, y + offsetX,
                                     x + offsetY, y + offsetX);
        status += SDL_RenderDrawLine(renderer, x - offsetX, y + offsetY,
                                     x + offsetX, y + offsetY);
        status += SDL_RenderDrawLine(renderer, x - offsetX, y - offsetY,
                                     x + offsetX, y - offsetY);
        status += SDL_RenderDrawLine(renderer, x - offsetY, y - offsetX,
                                     x + offsetY, y - offsetX);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetX) {
            d -= 2*offsetX + 1;
            offsetX +=1;
        }
        else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY -= 1;
        }
        else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY -= 1;
            offsetX += 1;
        }
    }

    return status;
}


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

int iDistance_From_Bottom_To_Floor = 40,
    iFloor = WindowSize.h - iDistance_From_Bottom_To_Floor;

class PVector {
public:
    double x;
    double y;

    PVector() : x(0), y(0) {};

    PVector(const double x_, const double y_) : x(x_), y(y_) {};

    void add(const PVector v) {
        x = x + v.x;
        y = y + v.y;
    }

    void distribute(const PVector v) {
        x = x - v.x;
        y = y - v.y;
    }

    [[nodiscard]] PVector rotate(const double deg) const {
        return PVector({x + deg, y + deg});
    }

    [[nodiscard]] PVector mult(const double lhs) const {
        return PVector({x * lhs, y * lhs});
    }

    [[nodiscard]] PVector div(const double lhs) const {
        return PVector({x / lhs, y / lhs});
    }

    [[nodiscard]] double mag() const {
        return sqrt(pow(x, 2) + pow(y, 2));
    }

    [[nodiscard]] PVector normalize() const {
        return div(mag() == 0 ? 1 : mag());
    }

    [[nodiscard]] PVector plus(const PVector v) const {
        return PVector{x + v.x, y + v.y};
    }

    [[nodiscard]] PVector sub(const PVector v) const {
        return PVector{x - v.x, y - v.y};
    }

    [[nodiscard]] double dot(const PVector v) const {
        return x * v.x + y * v.y;
    }

    [[nodiscard]] double dist(const PVector v) const {
        return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2));
    }

    [[nodiscard]] double angleBetween(const PVector v) const {
        return acos(this->dot(v) / (this->mag() * v.mag()));
    }

    void random2D(const int start = 50, const int end = 100) {
        x = rand() % (end - start) + start;
        y = rand() % (end - start) + start;
    }

};

PVector operator*(const double lhs, const PVector & sub) {
    return PVector{sub.x * lhs, sub.y * lhs};
};


const PVector GravitationForce({0, 9.8});

struct objectsProperties {
    double radius{};
    PVector position{};
    PVector velocity; // pixel per updateInterval
    PVector acceleration;
    double mass{};
    double angularVelocity{};
    double angularAcceleration{};
    queue<PVector> Trail;
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
        queue<PVector> tempTrail = o.Trail;
        stack<PVector> DrawTrail;
        while (!tempTrail.empty()) {
            DrawTrail.push(tempTrail.front());
            tempTrail.pop();
        }

        int offsetColor = 0;

        while (!DrawTrail.empty()) {
            SDL_SetRenderDrawColor(renderer, 0xFF - offsetColor, 0xFF - offsetColor, 0xFF - offsetColor, 255);
            SDL_RenderFillCircle(renderer, static_cast<int>(DrawTrail.top().x), static_cast<int>(DrawTrail.top().y), static_cast<int>(o.radius));
            DrawTrail.pop();
            offsetColor += 12;
        }
        if (o.Trail.size() > 10) o.Trail.pop();
    }

    // Main Object
    for (const auto & o : objects) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 255);
        SDL_RenderFillCircle(renderer, static_cast<int>(o.position.x), static_cast<int>(o.position.y), static_cast<int>(o.radius));
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

    if (objects[1].position.dist(objects[0].position) <= objects[1].radius + objects[0].radius) {
        const double vMassSum = objects[0].mass + objects[1].mass;
        PVector vDiff = objects[1].velocity.sub(objects[0].velocity);
        PVector vPosSub = objects[1].position.sub(objects[0].position);
        const double vDist = vPosSub.mag();
        objects[0].velocity.add((2 * objects[1].mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub);
        vDiff = -1 * vDiff;
        vPosSub = -1 * vPosSub;
        objects[1].velocity.add((2 * objects[0].mass / vMassSum) * vDiff.dot(vPosSub) / pow(vDist, 2) * vPosSub);
    }

    // Acceleration

    objects[0].velocity.add((static_cast<double>(CurrentTick - LatestUpdatedTick) / 1000) * objects[0].acceleration);
    objects[1].velocity.add((static_cast<double>(CurrentTick - LatestUpdatedTick) / 1000) * objects[1].acceleration);

    // Trail

    objects[0].Trail.push(objects[0].position);
    objects[1].Trail.push(objects[1].position);

    objects[0].position.add(objects[0].velocity);
    objects[1].position.add(objects[1].velocity);
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
        {400, static_cast<double>(iFloor - 100 - 1)},
        {6, 2},
        {0, 0},
        40,
        0,
        0
    });

    objects.push_back({
        75,
        {600, static_cast<double>(iFloor - 100 - 1)},
        {5, 1},
        {0, 0},
        40,
        0,
        0
    });

    for (auto& o : objects) {
        o.acceleration.add(GravitationForce);
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