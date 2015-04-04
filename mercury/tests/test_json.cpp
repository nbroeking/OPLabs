#include <json/Json.hpp>
#include <json/JsonMercuryTempl.hpp>
#include <test/Tester.hpp>

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

    (*jsn).setAttribute("teh_root", Json(3));
    TEST_BOOL("Set", (*jsn)["teh_root"] == 3);

    (*jsn).setAttribute("ipaddr", Json::from(Inet4Address("8.8.8.8", 0)));
    TEST_BOOL("TemplateSet",
        (*jsn)["ipaddr"].convert<Inet4Address>().toString() == "8.8.8.8:0");
}

int main(int argc, char** argv) {
    Tester::init("Json", argc, argv);

    try {
        runtester();
    } catch(Exception& exp) {
        char arr[4096];
        snprintf(arr, sizeof(arr), "Exception thrown: %s", exp.getMessage());
        TEST_BOOL(arr, false);
    }
}
