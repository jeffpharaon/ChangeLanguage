#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
using namespace std;

void execute(const string& command) {
    string fullCommand = "powershell.exe -Command \"" + command + "\"";
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (CreateProcessA(
        NULL,
        const_cast<LPSTR>(fullCommand.c_str()),
        NULL,
        NULL, 
        FALSE, 
        0, 
        NULL, 
        NULL, 
        &si, 
        &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int ru() { 
    execute("Set-WinUserLanguageList ru-RU -Force"); 
    return 0; 
}

int en() { 
    execute("Set-WinUserLanguageList en-US -Force"); 
    return 0; 
}

string get(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) return "";
    
    string line;
    string language;
    while (getline(inputFile, line)) {
        size_t keyPos = line.find("\"language\"");
        if (keyPos != string::npos) {
            size_t colon = line.find(":", keyPos);
            if (colon != string::npos) {
                size_t firstQuote = line.find("\"", colon);
                if (firstQuote != string::npos) {
                    size_t secondQuote = line.find("\"", firstQuote + 1);
                    if (secondQuote != string::npos) {
                        language = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                        break;
                    }
                }
            }
        }
    }
    inputFile.close();
    return language;
}

void update(const string& filename, const string& change) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) return;

    string fileContent;
    string line;
    while (getline(inputFile, line)) {
        size_t keyPos = line.find("\"language\"");
        if (keyPos != string::npos) {
            size_t colon = line.find(":", keyPos);
            if (colon != string::npos) {
                size_t firstQuote = line.find("\"", colon);
                if (firstQuote != string::npos) {
                    size_t secondQuote = line.find("\"", firstQuote + 1);
                    if (secondQuote != string::npos)
                        line.replace(firstQuote + 1, secondQuote - firstQuote - 1, change);
                }
            }
        }
        fileContent += line + "\n";
    }
    inputFile.close();

    ofstream outputFile(filename);
    if (outputFile.is_open()) {
        outputFile << fileContent;
        outputFile.close();
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
    
    string language = get("Language.json");
    if (language == "RU") {
        ru();
        update("language.json", "EN");
    }
    else if (language == "EN") {
        en();
        update("language.json", "RU");
    }
    return 0;
}
 