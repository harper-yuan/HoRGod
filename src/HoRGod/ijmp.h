#pragma once

#include <emp-tool/emp-tool.h>

#include <array>

#include "../io/netmp.h"

namespace HoRGod {

// Manages instances of jump.
class ImprovedJmp {
  int id_;
  // std::array<std::array<bool, 5>, 5> send_;
  // std::array<std::array<emp::Hash, 5>, 5> send_hash_;
  // std::array<std::array<std::vector<uint8_t>, 5>, 5> send_values_;
  // std::array<std::array<size_t, 5>, 5> recv_lengths_;
  // std::array<std::array<std::vector<uint8_t>, 5>, 5> recv_values_;
  std::array<std::array<std::array<bool, 5>, 5>, 5> send_;
  std::array<std::array<std::array<emp::Hash, 5>, 5>, 5> send_hash_;
  std::array<std::array<std::array<std::vector<uint8_t>, 5>, 5>, 5> send_values_;
  std::array<std::array<std::array<size_t, 5>, 5>, 5> recv_lengths_;
  std::array<std::array<std::array<bool, 5>, 5>, 5> is_received1_;
  std::array<std::array<std::array<bool, 5>, 5>, 5> is_received2_;
  std::array<std::array<std::array<bool, 5>, 5>, 5> is_received3_;
  std::array<std::array<std::array<std::vector<uint8_t>, 5>, 5>, 5> recv_values1_;
  std::array<std::array<std::array<std::vector<uint8_t>, 5>, 5>, 5> recv_values2_;
  std::array<std::array<std::array<std::vector<uint8_t>, 5>, 5>, 5> recv_values3_;
  std::array<std::array<std::array<std::vector<uint8_t>, 5>, 5>, 5> final_recv_values_;
  uint64_t counter=0;

  static bool isHashSender(int sender, int other_sender1, int other_sender2, int receiver);

 public:
  explicit ImprovedJmp(int my_id);

  void reset();

  void jumpUpdate(int sender1, int sender2, int sender3, int receiver, size_t nbytes,
                  const void* data = nullptr);
  void communicate(io::NetIOMP<5>& network, ThreadPool& tpool);
  const std::vector<uint8_t>& getValues(int sender1, int sender2, int sender3);
  size_t calculate_total_communication() 
  {
      size_t total_bits = 0;
      
      // 遍历所有发送方 (Sender PID)
      for (size_t sender = 0; sender < recv_lengths_.size(); ++sender) {
          
          // 遍历所有接收方 (Receiver PID)
          for (size_t receiver = 0; receiver < recv_lengths_[sender].size(); ++receiver) {
              
              // 遍历所有上下文/连接 (Context ID)
              for (size_t context = 0; context < recv_lengths_[sender][receiver].size(); ++context) {
                  
                  // 将当前元素的通信量累加到总数
                  total_bits += recv_lengths_[sender][receiver][context];
              }
          }
      }
      
      return total_bits;
  }
};

};  // namespace HoRGod
