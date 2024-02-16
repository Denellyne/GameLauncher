#include <iostream>
#include <filesystem>
#include <format>
namespace fs = std::filesystem;
std::string romPath = "D:\\roms\\Persona Q - Shadow of the Labyrinth (Europe).3ds";

const std::string romExtensions[]{".nds",".3ds",".gb",".gbc",".gba",".n64",".iso",".rvz"};
#define arraySize(x) sizeof(x)/sizeof(x[0])
constexpr int romExtensionsSize = arraySize(romExtensions);

static inline void getRoms(std::string romPath){
    for (const auto& dirEntry : fs::recursive_directory_iterator(romPath, fs::directory_options::skip_permission_denied)) {
        try {
            for (short int i = 0; i < romExtensionsSize; i++)
                if (!strcmp(dirEntry.path().string().substr(dirEntry.path().string().find_last_of(".")).c_str(), romExtensions[i].c_str()))
                    std::cout << dirEntry.path().string() << '\n';
        }
        catch (...) {
            &fs::recursive_directory_iterator::increment;
        }
    }
}

static inline std::string citraPath(){
    for (const auto& dirEntry : fs::recursive_directory_iterator("C:\\Users\\Gustavo Santos\\Appdata\\Local\\Citra\\", fs::directory_options::skip_permission_denied)) {
        try {
            if (!strcmp(dirEntry.path().string().substr(dirEntry.path().string().find_last_of("\\") + 1).c_str(), "citra-qt.exe")) {
                return dirEntry.path().string();
            }
        }
        catch (...) {
            &fs::recursive_directory_iterator::increment;
        }
    }
    return "aaa";
}

int main() {
        
    getRoms("D:\\roms\\");
    std::string executablePath = citraPath();
    system(std::format("\"\"{}\" \"{}\"",executablePath,romPath).c_str());

	return 0;
}