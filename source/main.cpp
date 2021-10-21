#include <iostream>
#include <sstream>
#include <random>
#include <string>
#include "utils/debug.h"
#include "cpphttplib/httplib.h"
#include "simpleini/SimpleIni.h"


int main() {
    Debug::Init();

    DEBUG_LOG_INFO("short_url_cpp start.");

    //read all from ini
    CSimpleIniA ini;
    ini.SetUnicode();

    char* ini_file_path="../data/data.ini";
    SI_Error rc = ini.LoadFile(ini_file_path);
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

        auto current_time = std::chrono::system_clock::now();
        auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());

        double num_seconds = duration_in_seconds.count();
        auto short_url= fmt::to_string(num_seconds);
        ini.SetValue("section", short_url.c_str(), src.str().c_str());

        const char* pv = ini.GetValue("section", short_url.c_str(), "");
        if(strcmp(pv,src.str().c_str())!=0){
            DEBUG_LOG_ERROR("gen error:{}",src.str().c_str());
            res.set_content("gen error", "text/plain");
        }else{
            res.set_content(short_url, "text/plain");
        }

        ini.SaveFile(ini_file_path);
    });

    svr.Get(R"(/go/([\s\S]*))", [&](const httplib::Request& req, httplib::Response& res) {
        auto short_url = req.matches[1];
        DEBUG_LOG_INFO("go:{}", short_url.str());

        const char* pv = ini.GetValue("section", short_url.str().c_str(), "");
        if(strcmp(pv,"")==0){
            DEBUG_LOG_ERROR("ini.GetValue error:{}", short_url.str().c_str());
            res.set_content("not found", "text/plain");
        }else{
            res.set_redirect(pv);
        }
    });

    svr.listen("0.0.0.0", 8080);

    return 0;
}
