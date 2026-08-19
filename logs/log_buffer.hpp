#include <unistd.h>
#include <cstddef>
#include <array>
#include <atomic>

template<typename T,std::size_t Capacity>
class LogBuffer{
private:
  static constexpr std::size_t increment(std::size_t i) noexcept{
    return (i+1)%Capacity;
  }
  std::array<T,Capacity> buffer;
  alignas(64) std::atomic<std::size_t> consumer{0};
  alignas(64) std::atomic<std::size_t> producer{0};
public:
  bool try_push(T val){
    const auto producer_ = producer.load(std::memory_order_relaxed);
    const auto next_producer_ = increment(producer_);

    if(next_producer_ == consumer.load(std::memory_order_acquire)){
      return false;
    }
    buffer[producer_]=val;
    producer.store(next_producer_,std::memory_order_release);
    return true;
  }

  bool try_pop(T& out){
    const auto consumer_ = consumer.load(std::memory_order_relaxed);
    const auto next_consumer_ = increment(consumer_);

    if(consumer_ == producer.load(std::memory_order_acquire)){
      return false;
    }
    out = std::move(buffer[consumer_]);
    consumer.store(next_consumer_,std::memory_order_release);
    return true;
  }
};