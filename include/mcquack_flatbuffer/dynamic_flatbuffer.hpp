#pragma once

#include <concepts>
#include <cstddef>
#include <iterator>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

namespace mcquack {

template<class T, std::size_t SLOT_SIZE>
class dynamic_flatbuffer
{
public:
    using value_type = typename std::vector<T>::value_type;
    using reference = typename std::vector<T>::reference;
    using const_reference = typename std::vector<T>::const_reference;
    using pointer = typename std::vector<T>::pointer;
    using const_pointer = typename std::vector<T>::const_pointer;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
    using difference_type = typename std::vector<T>::difference_type;
    using size_type = typename std::vector<T>::size_type;

    // Disable copying and assignation
    dynamic_flatbuffer(const dynamic_flatbuffer&) = delete;
    auto operator=(const dynamic_flatbuffer&) -> dynamic_flatbuffer& = delete;

    // Allow move semantics
    constexpr dynamic_flatbuffer(dynamic_flatbuffer&&) noexcept = default;
    constexpr auto operator=(dynamic_flatbuffer&&) noexcept -> dynamic_flatbuffer& = default;

    constexpr dynamic_flatbuffer(std::size_t initial_rows = 0) noexcept
        : buffer_(initial_rows * SLOT_SIZE)
    {}

    template<std::size_t SLOTS>
    constexpr dynamic_flatbuffer(std::span<T, SLOTS * SLOT_SIZE> data) noexcept
        : buffer_(std::begin(data), std::end(data))
    {}


    constexpr auto operator[](std::size_t idx) const noexcept -> std::span<const T, SLOT_SIZE>
    {
        return std::span<const T, SLOT_SIZE>{buffer_.data() + idx * SLOT_SIZE, SLOT_SIZE};
    }

    constexpr auto operator[](std::size_t idx) noexcept -> std::span<T, SLOT_SIZE>
    {
        return std::span<T, SLOT_SIZE>{buffer_.data() + idx * SLOT_SIZE, SLOT_SIZE};
    }

    constexpr auto reserve(std::size_t slot) noexcept -> void { buffer_.reserve(slot * SLOT_SIZE); }

    constexpr auto resize(std::size_t slot) noexcept -> void { buffer_.resize(slot * SLOT_SIZE); }

    constexpr auto append(std::vector<T>&& slot) noexcept -> void
    {
        buffer_.reserve(buffer_.size() + SLOT_SIZE);
        std::move(std::begin(slot), std::end(slot), std::back_inserter(buffer_));
    }

    constexpr auto append(std::span<T, SLOT_SIZE>&& slot) noexcept -> void
    {
        buffer_.reserve(buffer_.size() + SLOT_SIZE);
        std::copy(std::begin(slot), std::end(slot), std::back_inserter(buffer_));
    }

    constexpr auto append(std::array<T, SLOT_SIZE>&& slot) noexcept -> void
    {
        buffer_.reserve(buffer_.size() + SLOT_SIZE);
        std::move(std::begin(slot), std::end(slot), std::back_inserter(buffer_));
    }

    constexpr auto number_of_slots() const noexcept -> std::size_t
    {
        return buffer_.size() / SLOT_SIZE;
    }

    constexpr auto number_of_elements() const noexcept -> std::size_t { return buffer_.size(); }

    constexpr auto elements() const& noexcept
    {
        struct internal
        {
            constexpr internal(const dynamic_flatbuffer<T, SLOT_SIZE>& buffer) noexcept
                : buffer_(buffer)
            {}
            constexpr auto begin() noexcept { return buffer_.begin(); }
            constexpr auto cbegin() const noexcept { return buffer_.cbegin(); }
            constexpr auto end() noexcept { return buffer_.end(); }
            constexpr auto cend() noexcept { return buffer_.cend(); }
            constexpr auto rbegin() noexcept { return buffer_.rbegin(); }
            constexpr auto crbegin() const noexcept { return buffer_.crbegin(); }
            constexpr auto rend() noexcept { return buffer_.rend(); }
            constexpr auto crend() noexcept { return buffer_.crend(); }

        private:
            const dynamic_flatbuffer<T, SLOT_SIZE>& buffer_;
        };

        return internal{*this};
    }

    constexpr auto elements() & noexcept
    {
        struct internal
        {
            constexpr internal(dynamic_flatbuffer<T, SLOT_SIZE>& buffer) noexcept : buffer_(buffer)
            {}
            constexpr auto begin() noexcept { return buffer_.begin(); }
            constexpr auto end() noexcept { return buffer_.end(); }
            constexpr auto rbegin() noexcept { return buffer_.rbegin(); }
            constexpr auto rend() noexcept { return buffer_.rend(); }

        private:
            dynamic_flatbuffer<T, SLOT_SIZE>& buffer_;
        };

        return internal{*this};
    }


private:
    std::vector<T> buffer_;
};

} // namespace mcquack
