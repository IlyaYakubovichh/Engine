#pragma once

#include <memory>

namespace Engine {

    // ─── Smart pointer aliases ────────────────────────────────────────────────────
    template<typename T> using Ref = std::shared_ptr<T>;
    template<typename T> using Scope = std::unique_ptr<T>;

    // ─── Non-copyable, non-movable base ──────────────────────────────────────────
    /**
     * @brief Prevents copy and move for any derived class.
     *
     * Inherit publicly to express ownership semantics clearly.
     */
    class NonCopyable {
    public:
        NonCopyable() = default;
        virtual ~NonCopyable() = default;

        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable(NonCopyable&&) = delete;
        NonCopyable& operator=(NonCopyable&&) = delete;
    };

} // namespace Engine