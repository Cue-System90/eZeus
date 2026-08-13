#include "qfile.h"
#include "efspath.h"

#include <algorithm>

QFile::QFile(const std::string& filename) :
    std::ifstream(eFsPath::sPath(filename), std::ios::in | std::ios::binary) {
    mSize = 0;
    std::ifstream file(eFsPath::sPath(filename), std::ios::binary);

    mSize = file.tellg();
    file.seekg(0, std::ios::end);
    mSize = file.tellg() - mSize;
    file.close();
}

void QFile::reset() {
    seek(0);
}

bool QFile::isReadable() {
    return !fail();
}

bool QFile::atEnd() {
    const auto pos = tellg();
    return pos >= mSize;
}

int64_t QFile::size() const {
    return mSize;
}

int64_t QFile::pos() {
    return tellg();
}

void QFile::seek(const int64_t pos) {
    seekg(pos);
}

int64_t QFile::read(char* const data, const int64_t maxSize) {
    const int64_t remLen = mSize - tellg();
    const int64_t len = std::min(remLen, maxSize);
    std::ifstream::read(data, len);
    return len;
}

bool QFile::getChar(char* const data) {
    if(atEnd()) return false;
    std::ifstream::read(data, 1);
    return true;
}
