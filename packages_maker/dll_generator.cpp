#include "dll_generator.h"
#include <sstream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::string DllGenerator::generate(const MslbBag& bag, const std::string& dllOutputPath) {
    std::ostringstream ss;

    if (bag.codeType == "c") {
        ss << "#include <string.h>\n";
        ss << "#include <stdlib.h>\n";
        ss << "#include <stdio.h>\n";
    } else {
        ss << "#include <string>\n";
        ss << "#include <cstring>\n";
        ss << "#include <cstdlib>\n";
        ss << "#include <cstdio>\n";
        ss << "#include <fstream>\n";
        ss << "#include <sstream>\n";
        ss << "#include <iostream>\n";
        if (bag.codeType == "py") {
            ss << "#include <windows.h>\n";
        }
    }
    ss << "\n";

    for (const auto& varDef : bag.varDefs) {
        if (bag.codeType == "c") {
            ss << "static char msla_var_" << varDef.name << "[4096];\n";
        } else {
            ss << "static std::string msla_var_" << varDef.name << ";\n";
        }
    }

    ss << "\n";

    std::string infoContent = "lang:" + bag.codeType;
    for (size_t i = 0; i < bag.lines.size(); i++) {
        infoContent += "|";
        infoContent += bag.lines[i].funcName + " grammar:base64 \"" + bag.lines[i].grammar + "\"";
    }
    std::string escapedInfo;
    for (char c : infoContent) {
        if (c == '"') escapedInfo += "\\\"";
        else if (c == '\\') escapedInfo += "\\\\";
        else escapedInfo += c;
    }

    if (bag.codeType == "c") {
        ss << "__declspec(dllexport) const char* msla_get_info() {\n";
        ss << "    static char info[8192];\n";
        ss << "    info[0] = '\\0';\n";
        ss << "    strcat(info, \"" << escapedInfo << "\");\n";
        ss << "    return info;\n";
        ss << "}\n\n";
    } else {
        ss << "extern \"C\" __declspec(dllexport) const char* msla_get_info() {\n";
        ss << "    static std::string info;\n";
        ss << "    info = \"" << escapedInfo << "\";\n";
        ss << "    return info.c_str();\n";
        ss << "}\n\n";
    }

    for (const auto& lineNode : bag.lines) {
        if (bag.codeType == "c") {
            ss << "__declspec(dllexport) const char* msla_" << lineNode.funcName << "(const char** args, int argc) {\n";
        } else {
            ss << "extern \"C\" __declspec(dllexport) const char* msla_" << lineNode.funcName << "(const char** args, int argc) {\n";
        }

        if (bag.codeType == "c") {
            for (size_t i = 0; i < lineNode.grammarParams.size(); i++) {
                ss << "    const char* " << lineNode.grammarParams[i].name
                   << " = (argc > " << i << " && args[" << i << "]) ? args[" << i << "] : \"\";\n";
            }
            for (const auto& varDef : bag.varDefs) {
                ss << "    (void)msla_var_" << varDef.name << ";\n";
            }
            ss << "\n";
            ss << "    static char msla_result[65536];\n";
            ss << "    msla_result[0] = '\\0';\n\n";

            std::istringstream contentStream(lineNode.content);
            std::string contentLine;
            while (std::getline(contentStream, contentLine)) {
                while (!contentLine.empty() && (contentLine.back() == '\r' || contentLine.back() == '\n')) {
                    contentLine.pop_back();
                }
                bool hasPrint = contentLine.find("print(") != std::string::npos;
                if (hasPrint) {
                    size_t pos = contentLine.find("print(");
                    std::string before = contentLine.substr(0, pos);
                    std::string after = contentLine.substr(pos + 6);
                    if (!after.empty() && after.back() == ')') {
                        after.pop_back();
                    }
                    ss << before << "{ char _msla_buf[4096]; sprintf(_msla_buf, \"%s\\n\", " << after << "); strcat(msla_result, _msla_buf); }\n";
                } else {
                    ss << contentLine << "\n";
                }
            }

            ss << "\n    { size_t _len = strlen(msla_result); if (_len > 0 && msla_result[_len-1] == '\\n') msla_result[_len-1] = '\\0'; }\n";
            ss << "    return msla_result;\n";
        } else if (bag.codeType == "cpp" || bag.codeType == "msla") {
            for (size_t i = 0; i < lineNode.grammarParams.size(); i++) {
                ss << "    std::string " << lineNode.grammarParams[i].name
                   << " = (argc > " << i << " && args[" << i << "]) ? args[" << i << "] : \"\";\n";
            }
            for (const auto& varDef : bag.varDefs) {
                ss << "    (void)msla_var_" << varDef.name << ";\n";
            }
            ss << "\n";
            ss << "    static std::string msla_result;\n";
            ss << "    std::ostringstream msla_oss;\n";
            ss << "    std::ostream& msla_out = msla_oss;\n";
            ss << "    (void)msla_out;\n\n";

            std::istringstream contentStream(lineNode.content);
            std::string contentLine;
            while (std::getline(contentStream, contentLine)) {
                while (!contentLine.empty() && (contentLine.back() == '\r' || contentLine.back() == '\n')) {
                    contentLine.pop_back();
                }
                bool hasPrint = contentLine.find("print(") != std::string::npos;
                if (hasPrint) {
                    size_t pos = contentLine.find("print(");
                    std::string before = contentLine.substr(0, pos);
                    std::string after = contentLine.substr(pos + 6);
                    if (!after.empty() && after.back() == ')') {
                        after.pop_back();
                    }
                    ss << before << "msla_oss << " << after << " << std::endl;\n";
                } else {
                    ss << contentLine << "\n";
                }
            }

            ss << "\n    msla_result = msla_oss.str();\n";
            ss << "    if (!msla_result.empty() && msla_result.back() == '\\n') msla_result.pop_back();\n";
            ss << "    return msla_result.c_str();\n";
        } else if (bag.codeType == "py") {
            ss << "    std::string msla_py_script;\n";
            for (size_t i = 0; i < lineNode.grammarParams.size(); i++) {
                ss << "    std::string " << lineNode.grammarParams[i].name
                   << " = (argc > " << i << " && args[" << i << "]) ? args[" << i << "] : \"\";\n";
            }
            ss << "\n";
            for (size_t i = 0; i < lineNode.grammarParams.size(); i++) {
                ss << "    msla_py_script += \"" << lineNode.grammarParams[i].name
                   << " = r'\" + " << lineNode.grammarParams[i].name << " + \"'\\n\";\n";
            }

            std::istringstream contentStream(lineNode.content);
            std::string contentLine;
            while (std::getline(contentStream, contentLine)) {
                while (!contentLine.empty() && (contentLine.back() == '\r' || contentLine.back() == '\n')) {
                    contentLine.pop_back();
                }
                std::string escaped;
                for (char c : contentLine) {
                    if (c == '"') escaped += "\\\"";
                    else if (c == '\\') escaped += "\\\\";
                    else escaped += c;
                }
                ss << "    msla_py_script += \"" << escaped << "\\n\";\n";
            }

            ss << "\n";
            ss << "    std::string msla_tmp_script = std::string(std::getenv(\"TEMP\") ? std::getenv(\"TEMP\") : \"/tmp\") + \"\\\\msla_py_\" + std::to_string(GetCurrentProcessId()) + \".py\";\n";
            ss << "    std::ofstream msla_tmp_file(msla_tmp_script);\n";
            ss << "    msla_tmp_file << msla_py_script;\n";
            ss << "    msla_tmp_file.close();\n";
            ss << "    std::string msla_tmp_out = std::string(std::getenv(\"TEMP\") ? std::getenv(\"TEMP\") : \"/tmp\") + \"\\\\msla_py_out_\" + std::to_string(GetCurrentProcessId()) + \".txt\";\n";
            ss << "    std::string msla_cmd = \"python \\\"\" + msla_tmp_script + \"\\\" > \\\"\" + msla_tmp_out + \"\\\" 2>&1\";\n";
            ss << "    system(msla_cmd.c_str());\n";
            ss << "    std::ifstream msla_result_file(msla_tmp_out);\n";
            ss << "    std::ostringstream msla_oss_result;\n";
            ss << "    msla_oss_result << msla_result_file.rdbuf();\n";
            ss << "    static std::string msla_result;\n";
            ss << "    msla_result = msla_oss_result.str();\n";
            ss << "    if (!msla_result.empty() && msla_result.back() == '\\n') msla_result.pop_back();\n";
            ss << "    if (!msla_result.empty() && msla_result.back() == '\\r') msla_result.pop_back();\n";
            ss << "    remove(msla_tmp_script.c_str());\n";
            ss << "    remove(msla_tmp_out.c_str());\n";
            ss << "    return msla_result.c_str();\n";
        } else {
            ss << "    return \"\";\n";
        }

        ss << "}\n\n";
    }

    fs::path outputDir = fs::path(dllOutputPath).parent_path();
    if (outputDir.empty()) outputDir = ".";
    std::string srcExt = (bag.codeType == "c") ? ".c" : ".cpp";
    std::string tmpSrcPath = (outputDir / (bag.bagName + "_dll_src" + srcExt)).string();

    std::ofstream outFile(tmpSrcPath);
    outFile << ss.str();
    outFile.close();

    return tmpSrcPath;
}
