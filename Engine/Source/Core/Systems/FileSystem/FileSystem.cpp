#include "Systems/FileSystem/Filesystem.h"
#include "Systems/LogSystem/LogSystem.h"
#include "Macros.h"

namespace Engine {

    // ── Path queries ──────────────────────────────────────────────────────────────

    bool FileSystem::Exists(const std::filesystem::path& path) const
    {
        return std::filesystem::exists(path);
    }

    std::string FileSystem::GetExtension(const std::filesystem::path& path) const
    {
        return path.extension().string();
    }

    std::string FileSystem::GetStem(const std::filesystem::path& path) const
    {
        return path.stem().string();
    }

    std::string FileSystem::GetFilename(const std::filesystem::path& path) const
    {
        return path.filename().string();
    }

    // ── Directory helpers ─────────────────────────────────────────────────────────

    void FileSystem::CreateDirectories(const std::filesystem::path& path) const
    {
        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path);
    }

    // ── Binary I/O ────────────────────────────────────────────────────────────────

    std::vector<uint8_t> FileSystem::ReadBytes(const std::filesystem::path& path) const
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            ENGINE_LOG_ERROR("FileSystem", "Failed to open file: {}", path.string());
            return {};
        }

        const auto size = file.tellg();
        if (size <= 0) {
            ENGINE_LOG_ERROR("FileSystem", "File is empty or unreadable: {}", path.string());
            return {};
        }

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), size);

        return buffer;
    }

    std::vector<uint32_t> FileSystem::ReadWords(const std::filesystem::path& path) const
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            ENGINE_LOG_ERROR("Filesystem", "Failed to open SPIR-V file: {}", path.string());
            return {};
        }

        const auto byteSize = file.tellg();
        if (byteSize <= 0) {
            ENGINE_LOG_ERROR("Filesystem", "SPIR-V file is empty: {}", path.string());
            return {};
        }

        ENGINE_ASSERT_MSG(byteSize % 4 == 0, "SPIR-V file size must be a multiple of 4 bytes");

        std::vector<uint32_t> words(static_cast<size_t>(byteSize) / sizeof(uint32_t));
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(words.data()), byteSize);

        return words;
    }

    std::string FileSystem::ReadText(const std::filesystem::path& path) const
    {
        if (!std::filesystem::exists(path)) {
            ENGINE_LOG_ERROR("Filesystem", "File not found: {}", path.string());
            return {};
        }

        std::ifstream file(path, std::ios::in | std::ios::ate);
        if (!file) {
            ENGINE_LOG_ERROR("Filesystem", "Failed to open: {}", path.string());
            return {};
        }

        std::string content;
        content.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(content.data(), content.size());

        return content;
    }

    bool FileSystem::WriteWords(const std::filesystem::path& path, const std::vector<uint32_t>& words) const
    {
        std::ofstream file(path, std::ios::binary);

        if (!file.is_open()) {
            ENGINE_LOG_ERROR("Filesystem", "Failed to write file: {}", path.string());
            return false;
        }

        file.write(reinterpret_cast<const char*>(words.data()), static_cast<std::streamsize>(words.size() * sizeof(uint32_t)));

        return file.good();
    }

} // namespace Engine