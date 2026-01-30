#include <crow.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>


std::string name = "Metehan";
std::string surname = "TURGUT";
std::string address = "Elazığ, Türkiye";

std::vector<crow::json::wvalue> csvOku(std::string dosyaAdi, std::vector<std::string> anahtarlar) {
    std::vector<crow::json::wvalue> liste;
    std::ifstream dosya(dosyaAdi);
    std::string satir;

    while (std::getline(dosya, satir)) {
        std::stringstream ss(satir);
        std::string hucre;
        crow::json::wvalue veri;
        int i = 0;

        while (std::getline(ss, hucre, ',')) { // Virgülü ayraç olarak kullan
            if (i < anahtarlar.size()) {
                veri[anahtarlar[i]] = hucre;
                i++;
            }
        }
        liste.push_back(std::move(veri));
    }
    return liste;
}
void csvEkle(std::string dosyaAdi, std::vector<std::string> veriler) {
    std::ofstream dosya;
    // std::ios::app -> dosyanın sonuna ekler (Append)
    dosya.open(dosyaAdi, std::ios::app);

    for (size_t i = 0; i < veriler.size(); ++i) {
        dosya << veriler[i];
        if (i < veriler.size() - 1) dosya << ","; // Son eleman değilse virgül ekle
    }
    dosya << "\n"; // Yeni satıra geç
    dosya.close();
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](){
        crow::mustache::context ctx;
        ctx["isim"] = "Metehan";
        ctx["soyisim"] = "TURGUT";
        ctx["address"] = "Elazığ, Türkiye";
        ctx["mail"] = "metehanturgut794@gmail.com";
        ctx["tel"] = "+90 532 485 3665";
        ctx["about_me"] = "bilgisayarı yaladım yuttum";

        ctx["educations"] = csvOku("db/educations.csv", {"okul_adi", "bolum", "tarih"});
        ctx["experiences"] = csvOku("db/experiences.csv", {"sirket", "pozisyon", "tarih", "detay"});
        ctx["skills"] = csvOku("db/skills.csv", {"skill_name", "skill_level"});

        return crow::mustache::load("index.html").render(ctx);
    });

    CROW_ROUTE(app, "/skill/add/<string>/<string>")([&](std::string skill_name, std::string skill_level) {
        // Verileri hazırla ve dosyaya yaz
        csvEkle("db/skills.csv", {skill_name, skill_level});

        // İşlem bittikten sonra ana sayfaya yönlendir (veya JSON dön)
        crow::response res;
        res.redirect("/");
        return res;
    });

    CROW_ROUTE(app, "/experience/add/<string>/<string>/<string>/<string>")([&](std::string corp, std::string position, std::string date, std::string detail) {
        // Verileri hazırla ve dosyaya yaz
        csvEkle("db/experiences.csv", {corp, position, date, detail});

        // İşlem bittikten sonra ana sayfaya yönlendir (veya JSON dön)
        crow::response res;
        res.redirect("/");
        return res;
    });

    CROW_ROUTE(app, "/education/add/<string>/<string>/<string>")([&](std::string school_name, std::string bolum, std::string date) {
        // Verileri hazırla ve dosyaya yaz
        csvEkle("db/educations.csv", {school_name, bolum, date});

        // İşlem bittikten sonra ana sayfaya yönlendir (veya JSON dön)
        crow::response res;
        res.redirect("/");
        return res;
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
