#include <Engine.h>

class Application final : public Engine::Application {
public:
    Application() = default;
    ~Application() override = default;
};

int main() {
    auto* application = new Application();

    application->Start();
    application->Run();
    application->Shutdown();

    delete application;

    return 0;
}