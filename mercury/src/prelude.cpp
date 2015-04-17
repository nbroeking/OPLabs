#include <Prelude>

#include <log/LogManager.hpp>

using namespace logger;

class DefaultExceptionHandler: public ExceptionHandler {
public:
    virtual void onException(Exception& ex) {
        LogContext& log = LogManager::instance().getLogContext("Prelude", "Global");
        log.printfln(FATAL, "Global uncaught exception %s", ex.getMessage());
        exit(127);
    }
};

DefaultExceptionHandler g_exp_def_handler;
ExceptionHandler* g_exp_handler = &g_exp_def_handler;

void ExceptionHandler::setGlobalUncaughtExceptionHandler(ExceptionHandler* handler) {
    g_exp_handler = handler;
}

ExceptionHandler* ExceptionHandler::getGlobalUncaughtExceptionHandler() {
    return g_exp_handler;
}

ExceptionHandler* ExceptionHandler::getDefaultGlobalUncaughtExceptionHandler() {
    return &g_exp_def_handler;
}
