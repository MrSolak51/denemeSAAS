#include <crow.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <sstream>
#include <map>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <random>

// Header file where ImageConverter class is defined
#include "ImageConverter.h"

namespace fs = std::filesystem;


// ----------------------------------------------------------------
// HELPER FUNCTIONS
// ----------------------------------------------------------------


std::string get_html(std::string html_name) {
    std::string html_string;
    std::string line;

    std::ifstream file(html_name);
    if (!file.is_open()) return " ";

    while (std::getline(file, line)) {
        html_string.append(line);
        html_string.append("\n");
    }

    return  html_string;
}

std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

// ARKA PLANDA ÇALIŞAN TEMİZLİK FONKSİYONU (24 Saat Kuralı)
void start_cleanup_thread(std::string folder_path) {
    CROW_LOG_INFO << "Started Auto Cleaner";
    std::thread([folder_path]() {
        while (true) {
            auto now = std::chrono::system_clock::now();
            try {
                if (fs::exists(folder_path)) {
                    for (const auto& entry : fs::directory_iterator(folder_path)) {
                        auto last_write = fs::last_write_time(entry.path());
                        // Dosya zamanını sistem zamanına dönüştür
                        auto s_last_write = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                            last_write - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                        );

                        auto age = std::chrono::duration_cast<std::chrono::hours>(now - s_last_write);

                        if (age.count() >= 24) { // 24 saat sınırı
                            fs::remove(entry.path());
                            CROW_LOG_INFO << "Auto-deleted old file: " << entry.path().string();
                        }
                    }
                }
            } catch (const std::exception& e) {
                CROW_LOG_ERROR << "Cleanup Error: " << e.what();
            }
            std::this_thread::sleep_for(std::chrono::hours(1)); // Her saat başı kontrol et
        }
    }).detach();
}

// Unique ID generator (for Session ID)
std::string generate_user_id() {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    return std::to_string(now) + "_" + std::to_string(dis(gen));
}

// Helper function to extract Session ID from Cookie string
std::string get_session_id(const crow::request& req) {
    std::string cookie_header = req.get_header_value("Cookie");
    if (cookie_header.empty()) return "";

    std::string key = "user_session_id=";
    size_t pos = cookie_header.find(key);

    if (pos != std::string::npos) {
        std::string id = cookie_header.substr(pos + key.length());
        size_t end_pos = id.find(";");
        if (end_pos != std::string::npos) {
            return id.substr(0, end_pos);
        }
        return id;
    }
    return "";
}

// CSV Reader
std::vector<crow::json::wvalue> readCSV(std::string fileName, std::vector<std::string> keys) {
    std::vector<crow::json::wvalue> list;
    std::ifstream file(fileName);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string cell;
        crow::json::wvalue data;
        size_t i = 0;

        while (std::getline(ss, cell, ',')) {
            if (i < keys.size()) {
                data[keys[i]] = cell;
                i++;
            }
        }
        list.push_back(std::move(data));
    }
    return list;
}

// CSV Append
void appendCSV(std::string fileName, std::vector<std::string> values) {
    std::ofstream file(fileName, std::ios::app);
    for (size_t i = 0; i < values.size(); ++i) {
        file << values[i];
        if (i < values.size() - 1) file << ",";
    }
    file << "\n";
    file.close();
}

// CSV'den belirli bir anahtara (banner1 vb.) ait linki çeken fonksiyon
std::string get_link_from_csv(std::string banner_key) {
    std::ifstream file("db/ad_links.csv");
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key, url;

        if (std::getline(ss, key, ',') && std::getline(ss, url, ',')) {
            // Eğer aradığımız banner ismini bulursak linki dön
            if (key == banner_key) {
                return url;
            }
        }
    }
    // Eğer CSV'de bulunamazsa güvenli bir yere (anasayfaya) gönder
    return "/";
}

// ----------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------

int main() {
    crow::SimpleApp app;

    start_cleanup_thread("out_files");

    // Create required directories at startup
    fs::create_directories("in_files");
    fs::create_directories("out_files");
    fs::create_directories("db");

    // Assets klasörü altındaki tüm dosyaları (reklamlar, logolar vb.) sunmak için:
    CROW_ROUTE(app, "/assets/<path>")
    ([](const crow::request& req, crow::response& res, std::string path) {
        // path değişkeni "img/ads/banner1.jpeg" gibi değerler alacaktır.
        // Dosyanın tam yolunu belirtiyoruz:
        std::string local_path = "assets/" + path;

        // Crow'a bu yolu statik dosya olarak yüklemesini söylüyoruz
        res.set_static_file_info(local_path);
        res.end();
    });


    CROW_ROUTE(app, "/privacy_policy/")
       ([]() {
           crow::mustache::context ctx;
           ctx["name"] = "Metehan";
           ctx["surname"] = "TURGUT";

           ctx["nav"] = get_html("templates/nav.html");
           return crow::mustache::load("privacy_policy.html").render(ctx);
    });
    CROW_ROUTE(app, "/terms_of_service/")
       ([]() {
           crow::mustache::context ctx;
           ctx["name"] = "Metehan";
           ctx["surname"] = "TURGUT";
           ctx["nav"] = get_html("templates/nav.html");
           return crow::mustache::load("terms_of_service.html").render(ctx);
       });
    // 1. HOME PAGE
    CROW_ROUTE(app, "/")
    ([]() {
        crow::mustache::context ctx;
        ctx["name"] = "Metehan";
        ctx["surname"] = "TURGUT";
        ctx["about_me"] = "";
        ctx["educations"] = readCSV("db/educations.csv", {"school_name", "department", "date"});
        ctx["experiences"] = readCSV("db/experiences.csv", {"company", "position", "date", "details"});
        ctx["skills"] = readCSV("db/skills.csv", {"skill_name", "skill_level"});

        ctx["nav"] = get_html("templates/nav.html");
        return crow::mustache::load("index.html").render(ctx);
    });

    // 2. CONTACT PAGE
    CROW_ROUTE(app, "/contact_me/")
    ([]() {
        crow::mustache::context ctx;
        ctx["name"] = "Metehan";
        ctx["surname"] = "TURGUT";
        ctx["address"] = "Elazig, Turkey";
        ctx["mail"] = "metehanturgut794@gmail.com";
        ctx["tel"] = "+90 532 485 3665";

        ctx["nav"] = get_html("templates/nav.html");
        return crow::mustache::load("contact_me.html").render(ctx);
    });



    // 3. IMAGE CONVERTER INTERFACE (Session Managed)
    CROW_ROUTE(app, "/image_converter/")
    ([&](const crow::request& req) {
        crow::mustache::context ctx;
        ctx["name"] = "Metehan";
        ctx["surname"] = "TURGUT";

        ctx["nav"] = get_html("templates/nav.html");

        // Session ID check
        std::string user_id = get_session_id(req);
        bool is_new_user = false;

        if (user_id.empty()) {
            user_id = generate_user_id();
            is_new_user = true;
        }

        // List only files belonging to this user
        std::vector<crow::json::wvalue> file_list;
        if (fs::exists("out_files")) {
            for (const auto& entry : fs::directory_iterator("out_files")) {
                if (entry.is_regular_file()) {
                    std::string f_name = entry.path().filename().string();
                    if (f_name.rfind(user_id, 0) == 0) {
                        file_list.push_back(f_name);
                    }
                }
            }
        }
        ctx["uploaded_images"] = std::move(file_list);

        auto rendered = crow::mustache::load("image_converter.html").render(ctx);
        crow::response res(rendered.body_);

        // Send Cookie to browser if new user
        if (is_new_user) {
            res.add_header("Set-Cookie", "user_session_id=" + user_id + "; Path=/; HttpOnly");
        }
        return res;
    });

    // 4. FILE UPLOAD AND CONVERT (POST)
    CROW_ROUTE(app, "/image_converter/upload/").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        CROW_LOG_INFO << "Upload request started.";

        std::string user_id = get_session_id(req);
        if (user_id.empty()) return crow::response(400, "Session expired or invalid.");

        crow::multipart::message msg(req);
        if (msg.parts.empty()) return crow::response(400, "Form data is empty.");

        std::string filename = "";
        std::string file_body = "";
        std::string target_ext = "";

        try {
            for (auto& part : msg.parts) {
                auto it = part.headers.find("Content-Disposition");
                if (it != part.headers.end()) {
                    if (it->second.params.count("filename")) {
                        filename = it->second.params["filename"];
                        file_body = part.body;
                    }
                    else if (it->second.params.count("name")) {
                        std::string field_name = it->second.params["name"];
                        if (field_name == "extension" || field_name == "target_format") {
                            target_ext = part.body;
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "Multipart parsing error: " << e.what();
            return crow::response(500, "Failed to parse data.");
        }

        if (filename.empty()) return crow::response(400, "Filename not found.");

        if (target_ext.empty()) target_ext = "jpg";
        if (target_ext[0] != '.') target_ext = "." + target_ext;

        std::string unique_name = user_id + "_" + filename;
        std::string in_path = "in_files/" + unique_name;

        try {
            std::ofstream out(in_path, std::ios::binary);
            if (!out.is_open()) throw std::runtime_error("File could not be written (permission issue).");
            out << file_body;
            out.close();
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "File save error: " << e.what();
            return crow::response(500, "Server failed to save the file.");
        }

        std::string out_filename = user_id + "_out_" + fs::path(filename).stem().string() + "_" + get_current_timestamp() + target_ext;


        try {
            CROW_LOG_INFO << "Conversion started: " << unique_name << " -> " << out_filename;
            ImageConverter ic(unique_name, out_filename);
            ic.convert();
            CROW_LOG_INFO << "Conversion successful.";
        } catch (const std::exception& e) {
            CROW_LOG_ERROR << "CONVERTER ERROR: " << e.what();
            return crow::response(500, std::string("Conversion error occurred: ") + e.what());
        }

        if (fs::exists(in_path)) fs::remove(in_path);

        crow::response res;
        res.code = 200;
        res.add_header("Content-Type", "text/html");
        res.write("<script>window.location.href='/image_converter/';</script>");
        return res;
    });

    // Redirect if upload route is accessed via GET
    CROW_ROUTE(app, "/image_converter/upload/")
    ([]() {
        crow::response res;
        res.redirect("/image_converter/");
        return res;
    });

    // 5. FILE DELETE (Secure)
    CROW_ROUTE(app, "/image_converter/delete/<string>")
    ([&](const crow::request& req, std::string filename) {
        std::string user_id = get_session_id(req);

        if (!user_id.empty() && filename.rfind(user_id, 0) == 0) {
            std::string path = "out_files/" + filename;
            if (fs::exists(path)) fs::remove(path);
        }

        crow::response res;
        res.redirect("/image_converter/");
        return res;
    });

    // 6. FILE DOWNLOAD
    CROW_ROUTE(app, "/download/<string>")
    ([](std::string filename) {
        std::string path = "out_files/" + filename;
        crow::response res;

        if (!fs::exists(path)) {
            res.code = 404;
            res.write("File not found.");
            return res;
        }

        res.set_static_file_info(path);
        res.add_header("Content-Disposition", "attachment; filename=\"" + filename + "\"");
        return res;
    });

    // 7. DATA INSERT ROUTES (API)
    CROW_ROUTE(app, "/skill/add/<string>/<string>")
    ([&](std::string name, std::string level) {
        appendCSV("db/skills.csv", {name, level});
        return crow::response(302, "/");
    });

    CROW_ROUTE(app, "/experience/add/<string>/<string>/<string>/<string>")
    ([&](std::string corp, std::string pos, std::string date, std::string detail) {
        appendCSV("db/experiences.csv", {corp, pos, date, detail});
        return crow::response(302, "/");
    });

    CROW_ROUTE(app, "/education/add/<string>/<string>/<string>")
    ([&](std::string school, std::string department, std::string date) {
        appendCSV("db/educations.csv", {school, department, date});
        return crow::response(302, "/");
    });

    // Start application
    app.port(8080).multithreaded().run();
}








