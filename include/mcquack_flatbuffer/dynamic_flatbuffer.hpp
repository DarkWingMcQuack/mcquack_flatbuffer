#pragma once

#include <compare>
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
    using buf_base_t = std::vector<T>;
public:
    using value_type = typename buf_base_t::value_type;
    using reference = typename buf_base_t::reference;
    using const_reference = typename buf_base_t::const_reference;
    using pointer = typename buf_base_t::pointer;
    using const_pointer = typename buf_base_t::const_pointer;
    using iterator = typename buf_base_t::iterator;
    using const_iterator = typename buf_base_t::const_iterator;
    using reverse_iterator = typename buf_base_t::reverse_iterator;
    using const_reverse_iterator = typename buf_base_t::const_reverse_iterator;
    using difference_type = typename buf_base_t::difference_type;
    using size_type = typename buf_base_t::size_type;

    // Disable copying and assignation
    dynamic_flatbuffer(const dynamic_flatbuffer&) = delete;
    auto operator=(const dynamic_flatbuffer&) -> dynamic_flatbuffer& = delete;

    // Allow move semantics
    constexpr dynamic_flatbuffer(dynamic_flatbuffer&&) noexcept = default;
    constexpr auto operator=(dynamic_flatbuffer&&) noexcept -> dynamic_flatbuffer& = default;

    constexpr dynamic_flatbuffer(size_type initial_rows = 0) noexcept
        : buffer_(initial_rows * SLOT_SIZE)
    {}

    template<std::size_t SLOTS>
    constexpr dynamic_flatbuffer(std::span<T, SLOTS * SLOT_SIZE> data) noexcept
        : buffer_(std::begin(data), std::end(data))
    {}


    constexpr auto operator[](size_type idx) const noexcept -> std::span<const T, SLOT_SIZE>
    {
        return std::span<const T, SLOT_SIZE>{buffer_.data() + idx * SLOT_SIZE, SLOT_SIZE};
    }

    constexpr auto operator[](size_type idx) noexcept -> std::span<T, SLOT_SIZE>
    {
        return std::span<T, SLOT_SIZE>{buffer_.data() + idx * SLOT_SIZE, SLOT_SIZE};
    }

    constexpr auto reserve(size_type slot) noexcept -> void { buffer_.reserve(slot * SLOT_SIZE); }

    constexpr auto resize(size_type slot) noexcept -> void { buffer_.resize(slot * SLOT_SIZE); }

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

    constexpr auto number_of_slots() const noexcept -> size_type
    {
        return buffer_.size() / SLOT_SIZE;
    }

    constexpr auto number_of_elements() const noexcept -> size_type { return buffer_.size(); }

    constexpr auto begin() noexcept { return buffer_.begin(); }
    constexpr auto cbegin() const noexcept { return buffer_.cbegin(); }
    constexpr auto end() noexcept { return buffer_.end(); }
    constexpr auto cend() noexcept { return buffer_.cend(); }
    constexpr auto rbegin() noexcept { return buffer_.rbegin(); }
    constexpr auto crbegin() const noexcept { return buffer_.crbegin(); }
    constexpr auto rend() noexcept { return buffer_.rend(); }
    constexpr auto crend() noexcept { return buffer_.crend(); }

    struct slot_access
    {
    private:
        constexpr slot_access(dynamic_flatbuffer& buf) noexcept : buf_(buf) {}
        friend dynamic_flatbuffer;

    public:
        using value_type = std::span<T, SLOT_SIZE>;
        using reference = std::span<T, SLOT_SIZE>;
        using const_reference = std::span<const T, SLOT_SIZE>;
        using pointer = std::span<T, SLOT_SIZE>&;
        using const_pointer = std::span<const T, SLOT_SIZE>;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;

        class iterator_base
        {
            friend slot_access;
            constexpr iterator_base(dynamic_flatbuffer& buf, size_type index)
                : buf_(&buf), cur_slot_index_(index) {}
        public:
            using value_type = slot_access::value_type;
            using reference = slot_access::reference;
            using const_reference = slot_access::const_reference;
            using pointer = slot_access::pointer;
            using const_pointer = slot_access::const_pointer;
            using difference_type = slot_access::difference_type;
            using size_type = slot_access::size_type;
            using iterator_tag = std::random_access_iterator_tag;

            constexpr iterator_base() = default;
            constexpr ~iterator_base() = default;

            constexpr iterator_base(const iterator_base&) = default;
            constexpr auto operator=(const iterator_base&) -> iterator_base& = default;
            constexpr iterator_base(iterator_base&&) noexcept = default;
            constexpr auto operator=(iterator_base&&) noexcept -> iterator_base& = default;

            constexpr auto operator++() noexcept -> iterator_base&
            {
                ++cur_slot_index_;
                return *this;
            }
            constexpr auto operator++(int) noexcept -> iterator_base
            {
                auto pre_iter = *this;
                ++cur_slot_index_;
                return pre_iter;
            }
            constexpr auto operator--() noexcept -> iterator_base&
            {
                --cur_slot_index_;
                return *this;
            }
            constexpr auto operator--(int) noexcept -> iterator_base
            {
                auto pre_iter = *this;
                --cur_slot_index_;
                return pre_iter;
            }

            constexpr auto operator<=>(const iterator_base& other) const noexcept -> std::strong_ordering
            {
                return cur_slot_index_ <=> other.cur_slot_index_;
            }
            constexpr auto operator!=(const iterator_base& other) const noexcept -> bool = default;
            constexpr auto operator==(const iterator_base& other) const noexcept -> bool = default;

            constexpr auto operator*() const noexcept -> value_type
            {
                return buf_->operator[](cur_slot_index_);
            }
            constexpr auto operator*() noexcept -> reference
            {
                return buf_->operator[](cur_slot_index_);
            }

            constexpr auto operator->() const noexcept -> value_type
            {
                return buf_->operator[](cur_slot_index_);
            }
            constexpr auto operator->() noexcept -> reference
            {
                return buf_->operator[](cur_slot_index_);
            }

            auto operator+(difference_type diff) const noexcept -> iterator_base
            {
                return {cur_slot_index_ + diff};
            }
            auto operator+(const iterator_base& other) const noexcept -> iterator_base
            {
                return {cur_slot_index_ + other.cur_slot_index_};
            }

            auto operator-(difference_type diff) const noexcept -> iterator_base
            {
                return {cur_slot_index_ - diff};
            }
            auto operator-(const iterator_base other) const noexcept -> iterator_base
            {
                return {cur_slot_index_ - other.cur_slot_index_};
            }

            auto operator+=(difference_type diff) noexcept -> iterator_base&
            {
                cur_slot_index_ += diff;
                return *this;
            }
            auto operator-=(difference_type diff) noexcept -> iterator_base&
            {
                cur_slot_index_ += diff;
                return *this;
            }

            auto operator[](size_type index) const noexcept -> value_type
            {
                return buf_[index];
            }

            auto operator[](size_type index) noexcept -> reference
            {
                return buf_[index];
            }

        private:
            dynamic_flatbuffer* buf_ = nullptr;
            size_type cur_slot_index_ = 0;
        };

    public:

        class iterator : public iterator_base {
        public:
            using iterator_base::iterator_base;
        };

        class const_iterator : iterator_base {
        public:
            using iterator_base::iterator_base;

            auto operator[](size_type index) noexcept -> reference = delete;
            constexpr auto operator*() noexcept -> reference = delete;
            constexpr auto operator->() noexcept -> reference = delete;
        };

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr auto begin() noexcept -> iterator { return {buf_, 0}; }
        constexpr auto cbegin() const noexcept -> const_iterator { return {buf_, 0}; }
        constexpr auto end() noexcept -> iterator { return {buf_, buf_.number_of_slots()}; }
        constexpr auto cend() noexcept -> const_iterator { return {buf_, buf_.number_of_slots()}; }

        constexpr auto rbegin() noexcept { return std::reverse_iterator(iterator{buf_, 0}); }
        constexpr auto crbegin() const noexcept { return std::reverse_iterator(const_iterator{buf_, 0}); }
        constexpr auto rend() noexcept { return std::reverse_iterator(iterator{buf_, buf_.number_of_slots()}); }
        constexpr auto crend() noexcept { return std::reverse_iterator(const_iterator{buf_, buf_.number_of_slots()}); }

    private:
        dynamic_flatbuffer& buf_;
    };

    constexpr auto slots() & noexcept -> slot_access { return slot_access{*this}; }
    // constexpr auto slots() const noexcept -> slot_access { return const_slot_access{*this}; }

private:
    buf_base_t buffer_;
};

} // namespace mcquack
