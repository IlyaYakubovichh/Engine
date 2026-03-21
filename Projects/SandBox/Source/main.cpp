#include "App.h"
#include "GraphicsLayer.h"

int main()
{
    App app;
    app.Start();

    auto [windowId, window] = app.CreateWindow({
        .xpos = 100,
        .ypos = 100,
        .width = 800,
        .height = 600,
        .title = "Main Window"
        });

    app.PushLayer(new GraphicsLayer(windowId, window));
    app.Run();
    app.Shutdown();
    return 0;
}