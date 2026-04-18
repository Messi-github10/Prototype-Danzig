#pragma once

namespace common {

/**
 * @brief A base class that disables copy construction and copy assignment.
 *
 * Inherit from this class (privately or publicly) to make your class non-copyable.
 * Example:
 *     class MyClass : private NonCopyable {
 *         // MyClass cannot be copied
 *     };
 */
class NonCopyable {
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;

    // Delete copy operations
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    // Allow move operations (optional, can be overridden by derived class)
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

} // namespace common