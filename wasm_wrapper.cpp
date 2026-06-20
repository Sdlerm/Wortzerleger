#include "WordBreaker.h"

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

static WordBreaker breaker;
static bool loaded = false;

static std::string jsonEscape(const std::string& s) {
    std::ostringstream out;
    for (char c : s) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: out << c; break;
        }
    }
    return out.str();
}

static void nodeToJson(const AnalysisNode& node, std::ostringstream& out) {
    out << "{";
    out << "\"word\":\"" << jsonEscape(node.word) << "\",";
    out << "\"meaning\":\"" << jsonEscape(node.meaning) << "\",";
    out << "\"known\":" << (node.known ? "true" : "false") << ",";
    out << "\"cycle\":" << (node.cycle ? "true" : "false") << ",";
    out << "\"children\":[";
    for (size_t i = 0; i < node.children.size(); ++i) {
        nodeToJson(node.children[i], out);
        if (i + 1 < node.children.size()) out << ",";
    }
    out << "]}";
}

static char* copyForJs(const std::string& s) {
    char* result = static_cast<char*>(std::malloc(s.size() + 1));
    if (!result) return nullptr;
    std::memcpy(result, s.c_str(), s.size() + 1);
    return result;
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
int wb_init() {
    breaker.addLinkingElements();
    loaded = breaker.loadDictionary("dictionary.txt");
    return loaded ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
char* wb_analyze(const char* input) {
    if (!loaded) wb_init();
    if (!input) return copyForJs("{\"error\":\"null input\"}");

    AnalysisNode result = breaker.analyze(input);
    std::ostringstream out;
    nodeToJson(result, out);
    return copyForJs(out.str());
}

EMSCRIPTEN_KEEPALIVE
void wb_free(char* ptr) {
    std::free(ptr);
}

}
