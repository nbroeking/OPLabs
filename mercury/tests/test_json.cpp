#include <json/Json.hpp>
#include <json/JsonMercuryTempl.hpp>
#include <test/Tester.hpp>

#include <mercury/dns/TestConfig.hpp>

using namespace test;
using namespace json;
using namespace std;
using namespace io;

void runtester() {
    string str = "{"
                 "  \"teststr\": \"string1\""
                 ", \"strlst\": [\"str1\",\"str2\",\"str3\"]"
                 ", \"testlst\": [1,2,3]"
                 ", \"testobj\": {\"obj2\": 4}"
                 "}";
    uptr<Json> jsn = Json::parse(str);

   
    Json tmp = (*jsn)["teststr"];
    TEST_BOOL("StringValue", tmp.stringValue() == "string1");
    tmp = (*jsn)["testlst"];

    TEST_BOOL("List[0]", tmp[0] == 1);
    TEST_BOOL("List[1]", tmp[1] == 2);
    TEST_BOOL("List[2]", tmp[2] == 3);

    std::vector<s64_t> vec; 
    tmp.toVector(vec);

    TEST_BOOL("Vec[0]", vec[0] == 1);
    TEST_BOOL("Vec[1]", vec[1] == 2);
    TEST_BOOL("Vec[2]", vec[2] == 3);

    std::vector<std::string> strs;
    tmp = (*jsn)["strlst"];
    try {
        tmp.toVector(vec);
        TEST_BOOL("Should fail", false);
    } catch(Exception& e) {
        TEST_BOOL("Should fail", true);
    }

    tmp.toVector(strs);
    TEST_BOOL("Vec[0]", strs[0] == "str1");
    TEST_BOOL("Vec[1]", strs[1] == "str2");
    TEST_BOOL("Vec[2]", strs[2] == "str3");

    TEST_BOOL("TestObject", (*jsn)["testobj"]["obj2"] == 4);

    (*jsn).setAttribute("teh_root", Json::fromInt(3));
    TEST_BOOL("Set", (*jsn)["teh_root"] == 3);

    (*jsn).setAttribute("ipaddr", Json::from(Inet4Address("8.8.8.8", 0)));
    TEST_BOOL("TemplateSet",
        (*jsn)["ipaddr"].convert<Inet4Address>().toString() == "8.8.8.8:0");
}

void test_dns_packet() {
    const char* jsn_str = ""
                    "{"
                    "   \"valid_names\": ["
                    "       \"google.com\""
                    "   ],"
                    "   \"invalid_names\": ["
                    "       \"potato.xyz\""
                    "   ],"
                    "   \"dns_server\": \"8.8.4.4\","
                    "   \"timeout\":1000"
                    "}";
    const char* jsn_str2 = ""
                    "{"
                    "   \"valid_names\": ["
                    "       \"google.com\""
                    "   ],"
                    "   \"invalid_names\": ["
                    "       \"potato.xyz\""
                    "   ],"
                    "   \"dns_server\": \"::1\","
                    "   \"timeout\":1000"
                    "}";

    uptr<Json> jsn(Json::parse(jsn_str));
    dns::TestConfig conf = jsn->convert<dns::TestConfig>();

    TEST_STR_EQ("VALID_HOSTS", conf.valid_hostnames[0], "google.com");
    TEST_STR_EQ("INVALID_HOSTS", conf.invalid_hostnames[0], "potato.xyz");
    TEST_STR_EQ("SERVER", conf.server_address->toString(), "8.8.4.4:0");

    uptr<Json> jsn2(Json::parse(jsn_str2));
    conf = jsn2->convert<dns::TestConfig>();

    TEST_STR_EQ("VALID_HOSTS", conf.valid_hostnames[0], "google.com");
    TEST_STR_EQ("INVALID_HOSTS", conf.invalid_hostnames[0], "potato.xyz");
    TEST_STR_EQ("SERVER", conf.server_address->toString(), "[0000:0000:0000:0000:0000:0000:0000:0100]:0");
}


int main(int argc, char** argv) {
    Tester::init("Json", argc, argv);

    try {
        runtester();
        test_dns_packet();
    } catch(Exception& exp) {
        char arr[4096];
        snprintf(arr, sizeof(arr), "Exception thrown: %s", exp.getMessage());
        TEST_BOOL(arr, false);
    }

    Tester::instance().exit();
}
