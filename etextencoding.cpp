#include "etextencoding.h"

#include "efspath.h"

#include <fstream>
#include <iterator>
#include <cstdint>
#include <cstdio>

namespace {

eTextEncoding gEncoding = eTextEncoding::autoDetect;
eTextEncoding gLegacyEncoding = eTextEncoding::cp1252;

// Code points for bytes 0x80 - 0xFF. Bytes below 0x80 are ASCII in both
// code pages. Positions left undefined by the code page map to themselves.
const uint16_t kCp1252[128] = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
    0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
    0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

const uint16_t kCp1250[128] = {
    0x20AC, 0x0081, 0x201A, 0x0083, 0x201E, 0x2026, 0x2020, 0x2021,
    0x0088, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0098, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
    0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
    0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
    0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
    0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
    0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
    0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
    0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
    0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
    0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

void appendUtf8(std::string& out, const uint32_t cp) {
    if(cp < 0x80) {
        out.push_back(static_cast<char>(cp));
    } else if(cp < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if(cp < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

std::string utf16ToUtf8(const std::string& str, const bool bigEndian) {
    std::string result;
    result.reserve(str.size());
    const size_t sMax = str.size() & ~static_cast<size_t>(1);
    for(size_t i = 0; i < sMax; i += 2) {
        const auto b0 = static_cast<unsigned char>(str[i]);
        const auto b1 = static_cast<unsigned char>(str[i + 1]);
        uint32_t unit = bigEndian ? (b0 << 8) | b1 : (b1 << 8) | b0;
        if(unit >= 0xD800 && unit <= 0xDBFF && i + 3 < sMax) {
            const auto c0 = static_cast<unsigned char>(str[i + 2]);
            const auto c1 = static_cast<unsigned char>(str[i + 3]);
            const uint32_t low = bigEndian ? (c0 << 8) | c1 : (c1 << 8) | c0;
            if(low >= 0xDC00 && low <= 0xDFFF) {
                unit = 0x10000 + ((unit - 0xD800) << 10) + (low - 0xDC00);
                i += 2;
            }
        }
        appendUtf8(result, unit);
    }
    return result;
}

}

void eTextEncodings::sSetEncoding(const eTextEncoding e) {
    gEncoding = e;
}

eTextEncoding eTextEncodings::sEncoding() {
    return gEncoding;
}

void eTextEncodings::sSetLegacyEncoding(const eTextEncoding e) {
    // autoDetect and utf8 are not code pages, keep the previous fallback
    if(e == eTextEncoding::cp1252 || e == eTextEncoding::cp1250) {
        gLegacyEncoding = e;
    }
}

eTextEncoding eTextEncodings::sLegacyEncoding() {
    return gLegacyEncoding;
}

std::string eTextEncodings::sName(const eTextEncoding e) {
    switch(e) {
    case eTextEncoding::utf8: return "utf8";
    case eTextEncoding::cp1252: return "cp1252";
    case eTextEncoding::cp1250: return "cp1250";
    case eTextEncoding::autoDetect: break;
    }
    return "auto";
}

eTextEncoding eTextEncodings::sFromName(const std::string& name) {
    if(name == "utf8") return eTextEncoding::utf8;
    if(name == "cp1252") return eTextEncoding::cp1252;
    if(name == "cp1250") return eTextEncoding::cp1250;
    return eTextEncoding::autoDetect;
}

bool eTextEncodings::sIsValidUtf8(const std::string& str) {
    const size_t sMax = str.size();
    size_t i = 0;
    while(i < sMax) {
        const auto c = static_cast<unsigned char>(str[i]);
        int following;
        uint32_t cp;
        if(c < 0x80) {
            i++;
            continue;
        } else if((c & 0xE0) == 0xC0) {
            following = 1;
            cp = c & 0x1F;
        } else if((c & 0xF0) == 0xE0) {
            following = 2;
            cp = c & 0x0F;
        } else if((c & 0xF8) == 0xF0) {
            following = 3;
            cp = c & 0x07;
        } else {
            return false; // continuation byte or 5/6 byte lead
        }
        if(i + following >= sMax) return false;
        for(int k = 1; k <= following; k++) {
            const auto cc = static_cast<unsigned char>(str[i + k]);
            if((cc & 0xC0) != 0x80) return false;
            cp = (cp << 6) | (cc & 0x3F);
        }
        // reject overlong encodings, surrogates and out of range values
        if(following == 1 && cp < 0x80) return false;
        if(following == 2 && cp < 0x800) return false;
        if(following == 3 && cp < 0x10000) return false;
        if(cp > 0x10FFFF) return false;
        if(cp >= 0xD800 && cp <= 0xDFFF) return false;
        i += following + 1;
    }
    return true;
}

std::string eTextEncodings::sToUtf8(const std::string& str,
                                    const eTextEncoding e) {
    const uint16_t* table;
    switch(e) {
    case eTextEncoding::cp1252: table = kCp1252; break;
    case eTextEncoding::cp1250: table = kCp1250; break;
    default: return str;
    }
    std::string result;
    result.reserve(str.size());
    for(const auto ch : str) {
        const auto c = static_cast<unsigned char>(ch);
        if(c < 0x80) {
            result.push_back(static_cast<char>(c));
        } else {
            appendUtf8(result, table[c - 0x80]);
        }
    }
    return result;
}

bool eTextEncodings::sReadLines(const std::string& path,
                                std::vector<std::string>& lines) {
    std::ifstream file(eFsPath::sPath(path), std::ios::in | std::ios::binary);
    if(!file.good()) {
        printf("File missing %s\n", path.c_str());
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    bool converted = false;
    if(content.size() >= 3 &&
       static_cast<unsigned char>(content[0]) == 0xEF &&
       static_cast<unsigned char>(content[1]) == 0xBB &&
       static_cast<unsigned char>(content[2]) == 0xBF) {
        content.erase(0, 3);
        converted = true;
    } else if(content.size() >= 2 &&
              static_cast<unsigned char>(content[0]) == 0xFF &&
              static_cast<unsigned char>(content[1]) == 0xFE) {
        content = utf16ToUtf8(content.substr(2), false);
        converted = true;
    } else if(content.size() >= 2 &&
              static_cast<unsigned char>(content[0]) == 0xFE &&
              static_cast<unsigned char>(content[1]) == 0xFF) {
        content = utf16ToUtf8(content.substr(2), true);
        converted = true;
    }

    if(!converted) {
        switch(gEncoding) {
        case eTextEncoding::utf8:
            break;
        case eTextEncoding::cp1252:
        case eTextEncoding::cp1250:
            content = sToUtf8(content, gEncoding);
            break;
        case eTextEncoding::autoDetect:
            // ASCII and UTF-8 files pass through untouched, anything else
            // is assumed to be in the active language's code page
            if(!sIsValidUtf8(content)) {
                content = sToUtf8(content, gLegacyEncoding);
            }
            break;
        }
    }

    lines.clear();
    std::string line;
    for(const auto c : content) {
        if(c == '\n') {
            if(!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
            line.clear();
        } else {
            line.push_back(c);
        }
    }
    if(!line.empty()) {
        if(line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return true;
}
