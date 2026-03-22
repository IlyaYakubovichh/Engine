#pragma once

#include <Engine.h>
#include <filesystem>

class Application final : public Engine::Application {
public:
    Application() = default;
    ~Application() override = default;
};                                                                      