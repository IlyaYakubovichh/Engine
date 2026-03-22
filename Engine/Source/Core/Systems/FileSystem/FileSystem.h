#pragma once

#include "Macros.h"
#include "Singleton.h"
#include <filesystem>
#include <vector>
#include <string>
#include <cstdint>

namespace Engine {

    /**
     * @brief Singleton filesystem utility.
     *
     * Thin wrapper over std::filesystem and binary/text file I/O.
     * Follows the same lifecycle pattern as LogSystem, VulkanSystem, etc.
     * Start() / Shutdown() are called from Application.
     */
    class ENGINE_API FileSystem final : public Singleton<FileSystem> {
        friend class Singleton;
    public:
        // ── Path queries ──────────────────────────────────────────────────────────

        /** Returns true if the path exists on disk. */
        [[nodiscard]] bool Exists(const std::filesystem::path& path) const;

        /** Returns the file extension including the dot, e.g. ".spv". */
        [[nodiscard]] std::string GetExtension(const std::filesystem::path& path) const;

        /** Returns the filename stem, e.g. "Triangle.vert" from "Triangle.vert.spv". */
        [[nodiscard]] std::string GetStem(const std::filesystem::path& path) const;

        /** Returns the filename with extension, e.g. "Triangle.vert.spv". */
        [[nodiscard]] std::string GetFilename(const std::filesystem::path& path) const;

        // ── Directory helpers ─────────────────────────────────────────────────────

        /** Creates all directories in path if they do not exist. No op if already present. */
        void CreateDirectories(const std::filesystem::path& path) const;

        // ── Binary I/O ────────────────────────────────────────────────────────────

        /**
         * @brief Reads an entire binary file into a byte buffer.
         * @return File contents as raw bytes. Empty vector on failure.
         */
        [[nodiscard]] std::vector<uint8_t> ReadBytes(const std::filesystem::path& path) const;

        /**
         * @brief Reads a binary file into a uint32_t word buffer.
         *
         * Intended for SPIR-V loading. Asserts that file size % 4 == 0.
         * @return SPIR-V words. Empty vector on failure.
         */
        [[nodiscard]] std::vector<uint32_t> ReadWords(const std::filesystem::path& path) const;

        /**
         * @brief Reads an entire text file into a string.
         * @return File contents as UTF-8 string. Empty string on failure.
         */
        [[nodiscard]] std::string ReadText(const std::filesystem::path& path) const;

        /**
         * @brief Writes a uint32_t word buffer to a binary file.
         * @return True on success, false on failure.
         */
        bool WriteWords(const std::filesystem::path& path, const std::vector<uint32_t>& words) const;

    private:
        FileSystem() = default;
        ~FileSystem() override = default;
    };

} // namespace Engine