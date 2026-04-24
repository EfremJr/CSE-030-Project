#include <Application.h>

using namespace bobcat;

Application::Application() {
    // this is a change that I made - John Quinn

    window = new Window(25, 75, 400, 400, "Simple Navigation Project");


    window->show();
}