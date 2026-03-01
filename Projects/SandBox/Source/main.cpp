#include "SandBoxApplication.h"
#include "SandBoxGraphicsLayer.h"

int main() {
    const auto application = new SandBoxApplication();

    application->Start();

    application->PushLayer(new SandBoxGraphicsLayer());

    application->Run();
    application->Shutdown();

    delete application;

    return 0;
}