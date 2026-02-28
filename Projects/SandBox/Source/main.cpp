#include "SandBoxApplication.h"
#include "SandBoxGraphicsLayer.h"

int main() {
    SandBoxApplication* application = new SandBoxApplication();

    application->Start();

    // Layers
    application->PushLayer(new SandBoxGraphicsLayer());

    application->Run();
    application->Shutdown();

    delete application;

    return 0;
}