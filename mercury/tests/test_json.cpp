#include <json/Json.hpp>
#include <test/Tester.hpp>

using namespace test;
using namespace json;
using namespace std;

void runtester() {
    string str = "{\"teststr\": \"string1\", \"testlst\": [1,2,3], \"testobj\": {\"obj2\": 4}  }";
    uptr<Json> jsn = Json::parse(str);
    
    Json tmp = (*jsn)["teststr"];
    TEST_BOOL("StringValue", tmp.stringValue() == "string1");
    tmp = (*jsn)["testlst"];
    TEST_BOOL("List[0]", (*jsn)[0] == 1);
    TEST_BOOL("List[0]", (*jsn)[1] == 2);
    TEST_BOOL("List[0]", (*jsn)[2] == 3);
}

int main(int argc, char** argv) {
    Tester::init("Json", argc, argv);

    try {
        runtester();
    } catch(Exception& exp) {
        TEST_BOOL("Exception thrown", false);
    }
}
