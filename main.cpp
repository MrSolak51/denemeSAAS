#include <crow.h>

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](){
        return "<h1>Hoş geldin!</h1><p>Bu bir C++ web sayfası.</p>";
    });
    CROW_ROUTE(app, "/api/hello")([](){
        crow::json::wvalue res;
        res["message"] = "Hello from Crow API!";
        return res;
    });

    CROW_ROUTE(app, "/download/<string>")([](const crow::request&, crow::response& res, std::string filename) {
        std::string path = "files/" + filename;
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open()) {
            res.code = 404;
            res.write("File not found.");
            res.end();
            return;
        }

        res.add_header("Content-Type", "application/octet-stream");
        res.add_header("Content-Disposition", "attachment; filename=\"" + filename + "\"");

        const size_t buffer_size = 4096;
        char buffer[buffer_size];

        while (file.read(buffer, buffer_size) || file.gcount() > 0) {
            res.body.append(buffer, file.gcount());
        }

        res.end();
    });


    app.port(8080).multithreaded().run();
}
