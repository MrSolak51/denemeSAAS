#include <crow.h>

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "<h1>Hello, World from C++ on Render!</h1>";
    });

    app.port(8080).multithreaded().run();
}
