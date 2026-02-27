#include <Engine.h>

class Application : public Engine::Application {
public:
    Application() {

    }

    ~Application() {

    }

};

int main() {
    Application* application = new Application();
    application->Run();
    delete application;

    return 0;
}