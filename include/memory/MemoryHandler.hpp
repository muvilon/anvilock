
#pragma once

#include <anvilock/include/memory/MemoryImpl.hpp>

namespace anvlk::memory
{

// -------- Unique Pointer Wrapper --------

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr : public SmartPtrBase<std::unique_ptr<T, Deleter>, T>
{
  using Base = SmartPtrBase<std::unique_ptr<T, Deleter>, T>;

public:
  using Base::Base;

  UniquePtr() noexcept                               = default;
  UniquePtr(UniquePtr&&) noexcept                    = default;
  auto operator=(UniquePtr&&) noexcept -> UniquePtr& = default;

  // Disallow copy
  UniquePtr(const UniquePtr&)                    = delete;
  auto operator=(const UniquePtr&) -> UniquePtr& = delete;
};

// -------- Shared Pointer Wrapper --------

template <typename T> class SharedPtr : public SmartPtrBase<std::shared_ptr<T>, T>
{
  using Base = SmartPtrBase<std::shared_ptr<T>, T>;

public:
  using Base::Base;

  SharedPtr() noexcept                                    = default;
  SharedPtr(const SharedPtr&) noexcept                    = default;
  auto operator=(const SharedPtr&) noexcept -> SharedPtr& = default;

  SharedPtr(SharedPtr&&) noexcept                    = default;
  auto operator=(SharedPtr&&) noexcept -> SharedPtr& = default;

  [[nodiscard]] auto use_count() const noexcept -> long { return this->ptr_.use_count(); }
};

// -------- Weak Pointer Wrapper --------

template <typename T> class WeakPtr
{
  std::weak_ptr<T> ptr_;

public:
  WeakPtr() noexcept = default;
  explicit WeakPtr(const SharedPtr<T>& shared) noexcept : ptr_(shared.ptr_) {}

  auto lock() const noexcept -> SharedPtr<T> { return SharedPtr<T>(ptr_.lock()); }

  [[nodiscard]] auto expired() const noexcept -> bool { return ptr_.expired(); }

  void reset() noexcept { ptr_.reset(); }
};

// -------- Factory helpers --------

template <typename T, typename... Args> auto make_unique(Args&&... args) -> UniquePtr<T>
{
  return UniquePtr<T>(std::make_unique<T>(std::forward<Args>(args)...));
}

template <typename T, typename... Args> auto make_shared(Args&&... args) -> SharedPtr<T>
{
  return SharedPtr<T>(std::make_shared<T>(std::forward<Args>(args)...));
}

} // namespace anvlk::memory
