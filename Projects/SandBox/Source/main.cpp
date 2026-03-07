#include "SandBoxApplication.h"
#include "SandBoxGraphicsLayer.h"

int main() {
    const auto application = new SandBoxApplication();

    application->Start();

    application->CreateWindow({ 200, 200, 800, 600, "Window 1" });
    application->CreateWindow({ 300, 300, 800, 600, "Window 2" });

    application->PushLayer(new SandBoxGraphicsLayer());

    application->Run();
    application->Shutdown();

    delete application;

    return 0;
}