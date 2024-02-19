#include <iostream>
#include <filesystem>
#include <format>
namespace fs = std::filesystem;

std::vector<std::string> romPaths{};

#define arraySize(x) sizeof(x)/sizeof(x[0])

static inline void getRoms(std::string romPath){

    const std::string romExtensions[]{ ".nds",".3ds",".gb",".gbc",".gba",".n64",".iso",".rvz" };
    constexpr int romExtensionsSize = arraySize(romExtensions);

    for (const auto& dirEntry : fs::recursive_directory_iterator(romPath, fs::directory_options::skip_permission_denied)) {
        for (short int i = 0; i < romExtensionsSize; i++)
            try {
                if (dirEntry.path().string().substr(dirEntry.path().string().find_last_of(".")) == romExtensions[i])
                    romPaths.push_back(dirEntry.path().string());
        }
        catch (std::out_of_range) { ; }
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
    return "Not Found";
}

int main() {
    getRoms("C:\\Users\\Gustavo-Notebook\\Desktop\\");
    for (int i = 0; i < romPaths.size(); i++) std::cout << romPaths[i] << '\n';
    
    return 0;

}