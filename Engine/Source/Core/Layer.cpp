#include "Layer.h"

namespace Engine {

    Layer::Layer(std::string name)
        : mName(std::move(name))
    {
    }

} // namespace Engine