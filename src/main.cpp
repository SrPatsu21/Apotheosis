#include "client/Render.hpp"

int main() {
    Render* render = new Render();

    render->run();

    delete(render);
    return 0;
}