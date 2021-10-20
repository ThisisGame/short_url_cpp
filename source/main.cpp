#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include "utils/debug.h"
#include "cpphttplib/httplib.h"
#include "simpleini/SimpleIni.h"

unsigned int random_char() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

std::string generate_hex(const unsigned int len) {
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
        const auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}

int main() {
    Debug::Init();

    DEBUG_LOG_INFO("short_url_cpp start.");

    //read all from ini
    CSimpleIniA ini;
    ini.SetUnicode();

    SI_Error rc = ini.LoadFile("../data/data.ini");
    if (rc < 0) {
        DEBUG_LOG_ERROR("ini.LoadFile error.");
        return 1;
    };

//
//    ini.SetValue("section", "key", "newvalue");
//
//    pv = ini.GetValue("section", "key", "default");
//    ASSERT_STREQ(pv, "newvalue");


    // HTTP
    httplib::Server svr;

    svr.Get(R"(/gen/([\s\S]*))", [&](const httplib::Request& req, httplib::Response& res) {
        auto src = req.matches[1];
        DEBUG_LOG_INFO("gen:{}",src.str());

        auto short_url=generate_hex(16).c_str();
        ini.SetValue("section", short_url, src.str().c_str());

        const char* pv = ini.GetValue("section", short_url, "");
        if(strcmp(pv,src.str().c_str())!=0){
            DEBUG_LOG_ERROR("gen error:{}",src.str().c_str());
            res.set_content("gen error", "text/plain");
        }else{
            res.set_content(short_url, "text/plain");
        }
    });

    svr.Get(R"(/go/([\s\S]*))", [&](const httplib::Request& req, httplib::Response& res) {
        auto short_url = req.matches[1];
        DEBUG_LOG_INFO("go:{}", short_url.str());

        const char* pv = ini.GetValue("section", short_url.str().c_str(), "");
        if(strcmp(pv,"")==0){
            DEBUG_LOG_ERROR("ini.GetValue error:{}", short_url.str().c_str());
            res.set_content("not found", "text/plain");
        }else{
            res.set_content(pv, "text/plain");
        }
    });

    svr.listen("0.0.0.0", 8080);

    return 0;
}
