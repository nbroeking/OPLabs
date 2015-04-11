#include <io/FilePointer.hpp>

namespace io {

FilePointer* g_stdout;
FilePointer* g_stdin;

FilePointer* FilePointer::getStandardIn() {
    if(!g_stdin) {
        g_stdin = new FilePointer(stdin);
    }
    return g_stdin;
}
FilePointer* FilePointer::getStandardOut() {
    if(!g_stdout) {
        g_stdout = new FilePointer(stdout);
    }
    return g_stdout;
}

}
