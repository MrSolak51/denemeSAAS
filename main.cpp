#include "crow_all.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello, World from C++ on Render!";
    });

    app.port(8080).multithreaded().run();
}
