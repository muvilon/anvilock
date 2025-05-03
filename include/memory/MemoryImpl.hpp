#pragma once

#include <cassert>
#include <memory>
#include <utility>

namespace anvlk::memory
{

// Base class for shared functionality
template <typename PtrType, typename T> class SmartPtrBase
{
public:
  using element_type = T;

  SmartPtrBase() noexcept = default;
  explicit SmartPtrBase(PtrType ptr) noexcept : ptr_(std::move(ptr)) {}

  auto get() const noexcept -> T* { return ptr_.get(); }
  auto operator->() const noexcept -> T* { return ptr_.get(); }
  auto operator*() const noexcept -> T&
  {
    assert(ptr_);
    return *ptr_;
  }
  explicit operator bool() const noexcept { return static_cast<bool>(ptr_); }

  void swap(SmartPtrBase& other) noexcept { std::swap(ptr_, other.ptr_); }

  void reset() noexcept { ptr_.reset(); }

  auto release() -> T*
    requires std::is_same_v<PtrType, std::unique_ptr<T>>
  {
    return ptr_.release();
  }

  auto ref() const -> T&
  {
    assert(ptr_);
    return *ptr_;
  }

protected:
  PtrType ptr_;
};

} // namespace anvlk::memory
