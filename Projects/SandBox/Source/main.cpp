#include "App.h"
#include "GraphicsLayer.h"

int main()
{
    App app;

    app.Start();
    app.PushLayer(new GraphicsLayer());
    app.Run();
    app.Shutdown();

    return 0;
}