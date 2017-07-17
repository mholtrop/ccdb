#include <string>
#include <map>

#include "CCDB/Log.h"
#include "CCDB/Globals.h"


using namespace std;
namespace ccdb {
    int Log::msLastError = CCDB_NO_ERRORS;
    int Log::mErrorLevel = 3;

    int Log::GetLastError() {
        return msLastError;
    }


    void Log::Status(bool status, const string &description) {
    }

    void Log::Error(int errorCode, const string &module, const string &message) {
        if (mErrorLevel < 1)return;

        msLastError = errorCode;
        mErrStream << "CCDB (!!!)ERROR [" << errorCode << "]: ";
        mErrStream << "in [" << module << "] ";
        mErrStream << std::endl;
    }

    void Log::Warning(int errorCode, const string &module, const string &message) {
        if (mErrorLevel < 2) return;

        mErrStream << "CCDB (!)Warning [" << errorCode << "]: ";
        mErrStream << "in [" << module << "] ";
        mErrStream << std::endl;
    }

    void Log::Message(const string &message) {
        if (mErrorLevel < 3) return;

        mStdStream << "CCDB Msg: " << message << std::endl;
    }

    void Log::Verbose(const string &module, const string &message) {
        if (mErrorLevel < 4) return;
        mStdStream << "CCDB Verbose [" << module << "]: " << message << std::endl;
    }

    map<int, string> gCCDBErrorCodes;

}

