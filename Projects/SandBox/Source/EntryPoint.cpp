#include "Core/Application.h"
#include "Layers/GraphicsLayer.h"

int main()
{
    Application app;
    app.Start();

    auto [id1, win1] = app.CreateWindow({ .xpos = 100,  .ypos = 100, .width = 800, .height = 600, .title = "Window 1" });
    auto [id2, win2] = app.CreateWindow({ .xpos = 950,  .ypos = 100, .width = 800, .height = 600, .title = "Window 2" });
    auto [id3, win3] = app.CreateWindow({ .xpos = 100,  .ypos = 750, .width = 800, .height = 600, .title = "Window 3" });

    app.PushLayer(new GraphicsLayer({ {id1, win1}, {id2, win2}, {id3, win3} }));

    app.Run();
    app.Shutdown();
    return 0;
}