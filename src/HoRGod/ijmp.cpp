#include "ijmp.h"
#include <mutex>
#include <algorithm>
#include <boost/format.hpp>
#include <future>
#include <stdexcept>

#include "helpers.h"

namespace HoRGod {
ImprovedJmp::ImprovedJmp(int my_id) : id_(my_id), recv_lengths_{}, send_{} {}

// std::array<std::array<bool, 5>, 5> send_;
// std::array<std::array<emp::Hash, 5>, 5> send_hash_;
// std::array<std::array<std::vector<uint8_t>, 5>, 5> send_values_;
// std::array<std::array<size_t, 5>, 5> recv_lengths_;
// std::array<std::array<std::vector<uint8_t>, 5>, 5> recv_values_;
void ImprovedJmp::reset() {
  for (size_t i = 0; i < 5; ++i) {
    for (size_t j = 0; j < 5; ++j) {
      for(size_t k = 0; k <5; ++k)
      {
        send_[i][j][k] = false;  //代表进程中通信状态，i是否需要向j通信
        send_hash_[i][j][k].reset(); //
        send_values_[i][j][k].clear(); //每一个都是一个哈希函数
        recv_lengths_[i][j][k] = 0; //size_t 代表发送长度
        recv_values1_[i][j][k].clear(); //std::vector<uint8_t>代表发送的数据
        recv_values2_[i][j][k].clear();
        recv_values3_[i][j][k].clear();
        final_recv_values_[i][j][k].clear();
        is_received1_[i][j][k] = false;
        is_received2_[i][j][k] = false;
        is_received3_[i][j][k] = false;
      }
    }
  }
}

size_t calculate_total_communication(
    const std::array<std::array<std::array<size_t, 5>, 5>, 5>& recv_lengths_) 
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
bool ImprovedJmp::isHashSender(int sender, int other_sender1, int other_sender2, int receiver) { //确定某组三人中，谁负责发送哈希
  return (sender > other_sender1) && (sender > other_sender2); //规定3个人中，number数大的传数据
}

void ImprovedJmp::jumpUpdate(int sender1, int sender2, int sender3, int receiver,
                              size_t nbytes, const void* data) {
  // 【关键修改】使用静态互斥锁保护此函数的执行
  // 这确保了即使多个线程同时调用此函数，同一时间也只有一个线程能修改共享的 buffer
  static std::mutex mtx;
  std::lock_guard<std::mutex> lock(mtx);

  if (sender1 == sender2 || sender1 == sender3 || sender1 == receiver || 
      sender2 == sender3 || sender2 == receiver|| sender3 == receiver) {
    throw std::invalid_argument(boost::str(
        boost::format(
            "ID, other_sender and receiver must be distinct for Jump3 but "
            "received sender1=%1%, sender2=%2%, sender3=%3% and receiver=%5%") %
        sender1 % sender2 % sender3 % receiver));
  }
  auto [min, mid, max] = sortThreeNumbers(sender1, sender2, sender3);
  
  if (id_ == receiver) { // 如果是receiver执行函数，那么update接受消息的长度
    // 这里是竞态条件的高发区：recv_lengths_ += nbytes 需要原子性
    recv_lengths_[min][mid][max] += nbytes;
    is_received1_[min][mid][max] = true;
    is_received2_[min][mid][max] = true;
    is_received3_[min][mid][max] = true;
    return;
  }
  
  if (id_ != sender1 && id_ != sender2 && id_ != sender3) {
    return;
  }

  // 只剩下，id_为发送者的情况，直接发送数据即可
  auto [other_sender1, other_sender2] = findOtherSenders(min, mid, max, id_);
  
  if (isHashSender(id_, other_sender1, other_sender2, receiver)) {
    send_hash_[other_sender1][other_sender2][receiver].put(data, nbytes);
  }
  else {
    const auto* temp = static_cast<const uint8_t*>(data);
    auto& values = send_values_[other_sender1][other_sender2][receiver];
    // 这里是另一个竞态条件高发区：vector::insert 非线程安全
    values.insert(values.end(), temp, temp + nbytes); 
  }
  send_[other_sender1][other_sender2][receiver] = true;
}

// void ImprovedJmp::jumpUpdate(int sender1, int sender2, int sender3, int receiver,
//                               size_t nbytes, const void* data) {
//   if (sender1 == sender2 || sender1 == sender3 || sender1 == receiver || 
//       sender2 == sender3 || sender2 == receiver|| sender3 == receiver) {
//     throw std::invalid_argument(boost::str(
//         boost::format(
//             "ID, other_sender and receiver must be distinct for Jump3 but "
//             "received sender1=%1%, sender2=%2%, sender3=%3% and receiver=%5%") %
//         sender1 % sender2 % sender3 % receiver));
//   }
//   auto [min, mid, max] = sortThreeNumbers(sender1, sender2, sender3);
//   if (id_ == receiver) { //如果是receiver执行函数，那么update接受消息的长度
//     recv_lengths_[min][mid][max] += nbytes;
//     is_received1_[min][mid][max] = true;
//     is_received2_[min][mid][max] = true;
//     is_received3_[min][mid][max] = true;
//     return;
//   }
//   if (id_ != sender1 && id_ != sender2 && id_ != sender3) {
//     return;
//   }

//   //只剩下，id_为发送者的情况，直接发送数据即可
//   auto [other_sender1, other_sender2] = findOtherSenders(min, mid, max, id_);
//   if (isHashSender(id_, other_sender1, other_sender2, receiver)) {
//     send_hash_[other_sender1][other_sender2][receiver].put(data, nbytes);
//   }
//   else {
//     const auto* temp = static_cast<const uint8_t*>(data);
//     auto& values = send_values_[other_sender1][other_sender2][receiver];
//     values.insert(values.end(), temp, temp + nbytes); //在指定位置 pos 之前插入 [first, last) 区间的数据。
//   }
//   send_[other_sender1][other_sender2][receiver] = true;
// }

// 必须包含头文件
#include <vector>
#include <future>

void ImprovedJmp::communicate(io::NetIOMP<5>& network, ThreadPool& tpool) {
  // 使用 vector 存储所有异步任务的句柄（包括发送和接收）
  std::vector<std::future<void>> all_tasks; 

  // ================= 1. 立即启动接收任务 (Recv) =================
  // 必须在此处初始化 recv_hash，确保它在任务执行期间有效
  // 注意：recv_hash 必须能被 lambda 捕获并修改
  // 为了线程安全，这里我们为每个可能的组合预留空间，避免竞争
  // 但 recv_hash 是局部变量，需要确保在 all_tasks 完成前不被销毁。这是安全的。
  std::array<std::array<std::array<std::array<char, emp::Hash::DIGEST_SIZE>, 5>, 5>, 5> recv_hash{};

  for (int sender = 0; sender < 5; ++sender) {
    if (sender == id_) continue;

    // 【核心修复】：直接把接收任务扔进线程池跑起来，绝不等待
    all_tasks.push_back(tpool.enqueue([&, sender]() {
      for (int other_sender1 = 0; other_sender1 < 5; ++other_sender1) {
        for (int other_sender2 = other_sender1+1; other_sender2 < 5; ++other_sender2) {
          if (other_sender1 == sender || other_sender1 == id_ ||
              other_sender2 == sender || other_sender2 == id_ || other_sender1 == other_sender2) {
            continue;
          }
          // 读取 recv_lengths_ 是安全的，因为 jumpUpdate 阶段已结束
          auto [min, mid, max] = sortThreeNumbers(sender, other_sender1, other_sender2);
          auto nbytes = recv_lengths_[min][mid][max];

          if (nbytes != 0) {
            if (sender == min) {
              auto& values = recv_values1_[min][mid][max];
              values.resize(values.size() + nbytes);
              network.recv(sender, values.data() + values.size() - nbytes, nbytes);
            } 
            else if (sender == mid) {
              auto& values = recv_values2_[min][mid][max];
              values.resize(values.size() + nbytes);
              network.recv(sender, values.data() + values.size() - nbytes, nbytes);
            } 
            else if (sender == max) {
              network.recv(sender, recv_hash[min][mid][max].data(), emp::Hash::DIGEST_SIZE);
            }
          }
        }
      }
      // 接收端通常不需要 flush
    }));
  }

  // ================= 2. 同时启动发送任务 (Send) =================
  for (int receiver = 0; receiver < 5; ++receiver) {
    if (receiver == id_) continue;

    all_tasks.push_back(tpool.enqueue([&, receiver]() {
      for (int other_sender1 = 0; other_sender1 < 5; ++other_sender1) {
        for (int other_sender2 = other_sender1+1; other_sender2 < 5; ++other_sender2) {
          if (other_sender1 == receiver || other_sender1 == id_ ||
              other_sender2 == receiver || other_sender2 == id_ || other_sender1 == other_sender2) {
            continue;
          }
          
          int min, max;
          if (other_sender1 < other_sender2) {
            min = other_sender1; max = other_sender2;
          } else {
            min = other_sender2; max = other_sender1;
          }
          
          bool should_send = send_[min][max][receiver];
          if (should_send) {
            if(isHashSender(id_, min, max, receiver)) {
              auto& hash = send_hash_[min][max][receiver];
              std::array<char, emp::Hash::DIGEST_SIZE> digest{};
              hash.digest(digest.data());
              network.send(receiver, digest.data(), digest.size());
            } else {
              auto& values = send_values_[min][max][receiver];
              network.send(receiver, values.data(), values.size());
            }
          }
        }
      }
      network.flush(receiver); // 发送完毕立即刷新缓冲区，确保数据推入网络
    }));
  }

  // ================= 3. 等待所有任务完成 =================
  // 此时：如果 Send 阻塞了，Recv 线程会在后台默默把数据收走，
  // 从而清空对方的发送缓冲区，解开死锁。
  for (auto& f : all_tasks) {
    f.get();
  }

  // ================= 4. 校验逻辑 (Verify) =================
  // 保持原样
  emp::Hash hash;
  std::array<char, emp::Hash::DIGEST_SIZE> digest{};
  for (int sender1 = 0; sender1 < 5; ++sender1) {
    for (int sender2 = sender1 + 1; sender2 < 5; ++sender2) {
      for (int sender3 = sender2 + 1; sender3 < 5; ++sender3) {
        if (sender1 == id_ || sender2 == id_ || sender3 == id_) continue;
        
        auto nbytes = recv_lengths_[sender1][sender2][sender3];
        if (nbytes == 0) continue;

        auto& values1 = recv_values1_[sender1][sender2][sender3];
        auto& values2 = recv_values2_[sender1][sender2][sender3];
        auto& final_values = final_recv_values_[sender1][sender2][sender3];

        hash.put(values1.data(), values1.size());
        hash.digest(digest.data());
        
        bool match = true;
        for(int k=0; k<emp::Hash::DIGEST_SIZE; ++k) {
            if(digest[k] != recv_hash[sender1][sender2][sender3][k]) match = false;
        }

        if (!match) {
          final_values = values2;
        } else {
          final_values = values1;
        }
      }
    }
  }
}
// void ImprovedJmp::communicate(io::NetIOMP<5>& network, ThreadPool& tpool) {
//   std::vector<std::future<void>> res; // std::future<void>作用：表示一个异步操作的结果（来自 std::async、std::promise 或线程池任务）。
  
//   // Send data.
//   for (int receiver = 0; receiver < 5; ++receiver) {
//     if (receiver == id_) { //如果id_是接收者，不用发送数据，于是跳过
//       continue;
//     }
//     //下面的情况，id_一定是发送者，所以遍历所有可能的发送情况，是否需要发送查询send_即可

//     res.push_back(tpool.enqueue([&, receiver]() {
//       for (int other_sender1 = 0; other_sender1 < 5; ++other_sender1) {
//         for (int other_sender2 = other_sender1+1; other_sender2 < 5; ++other_sender2) {//已经确定了
//           if (other_sender1 == receiver || other_sender1 == id_ ||
//               other_sender2 == receiver || other_sender2 == id_ || other_sender1 == other_sender2) { //确保id_一定是发送者
//             continue;
//           }
//           // auto& hash = send_hash_[other_sender][receiver];
//           int min, max;
//           if (other_sender1 < other_sender2) {
//             min = other_sender1;
//             max = other_sender2;
//           }
//           else {
//             min = other_sender2;
//             max = other_sender1;
//           }
          
//           auto& values = send_values_[min][max][receiver];
//           bool should_send = send_[min][max][receiver];
//           if (should_send) {
//             if(isHashSender(id_, min, max, receiver)) {
//               auto& hash = send_hash_[min][max][receiver];
//               std::array<char, emp::Hash::DIGEST_SIZE> digest{};
//               hash.digest(digest.data());
//               // std::cout<<"send: "<<receiver<<" hash values"<<endl;
//               network.send(receiver, digest.data(), digest.size());
              
//             }
//             else {
//               // std::cout<<"send: "<<receiver<<" "<<values.size()<<" Btyes"<<endl;
//               network.send(receiver, values.data(), values.size());
//             }
//             // std::cout<<"id="<<id_<<": "<<min<<", "<<max<<"->"<<receiver<<", "<<values.size()<<"Bytes"<<endl;
//           }
//         }
//       }

//       network.flush(receiver);
//     }));
//   }
//   std::cout<<"等待结果"<<std::endl;
//   for (auto& f : res) {
//     f.get();
//   }
//   std::cout<<"结果已出"<<std::endl;
//   std::vector<std::future<void>> res_recv;
//   // Receive data.
//   // 这里需要初始化 recv_hash，确保它的大小和类型正确
//   std::array<std::array<std::array<std::array<char, emp::Hash::DIGEST_SIZE>, 5>, 5>, 5> recv_hash{};

//   for (int sender = 0; sender < 5; ++sender) {
//     if (sender == id_) { 
//       continue;
//     }

//     res_recv.push_back(tpool.enqueue([&, sender]() {
//       for (int other_sender1 = 0; other_sender1 < 5; ++other_sender1) {
//         for (int other_sender2 = other_sender1 + 1; other_sender2 < 5; ++other_sender2) {
//           if (other_sender1 == sender || other_sender1 == id_ ||
//               other_sender2 == sender || other_sender2 == id_ || other_sender1 == other_sender2) {
//             continue;
//           }
//           auto [min, mid, max] = sortThreeNumbers(sender, other_sender1, other_sender2);
//           auto nbytes = recv_lengths_[min][mid][max];

//           if (nbytes != 0) {
//             // FIX: 使用确定性逻辑分配缓冲区，避免竞争
//             if (sender == min) {
//               auto& values = recv_values1_[min][mid][max];
//               // resize 不是线程安全的，但在这种映射下，只有当前线程会操作这个特定的 values 向量
//               values.resize(values.size() + nbytes);
//               network.recv(sender, values.data() + values.size() - nbytes, nbytes);
//             } 
//             else if (sender == mid) {
//               auto& values = recv_values2_[min][mid][max];
//               values.resize(values.size() + nbytes);
//               network.recv(sender, values.data() + values.size() - nbytes, nbytes);
//             } 
//             else if (sender == max) {
//               // 最大ID发送的是哈希
//               network.recv(sender, recv_hash[min][mid][max].data(), emp::Hash::DIGEST_SIZE);
//             }
//           }
//         }
//       }
//       network.flush(sender);
//     }));
//   }

//   for (auto& f : res_recv) {
//     f.get();
//   }
//   // std::cout<<"have recived"<<endl;
//   // Verify.
//   emp::Hash hash;
//   std::array<char, emp::Hash::DIGEST_SIZE> digest{};
//   for (int sender1 = 0; sender1 < 5; ++sender1) {
//     for (int sender2 = sender1 + 1; sender2 < 5; ++sender2) {
//       for (int sender3 = sender2 + 1; sender3 < 5; ++sender3) {
//         if (sender1 == id_ || sender2 == id_ || sender3 == id_) {
//           continue;
//         }
//         auto nbytes = recv_lengths_[sender1][sender2][sender3];
//         auto& values1 = recv_values1_[sender1][sender2][sender3];
//         auto& values2 = recv_values2_[sender1][sender2][sender3];
//         auto& final_values = final_recv_values_[sender1][sender2][sender3];

//         hash.put(values1.data(), values1.size());
//         hash.digest(digest.data());
//         if (digest != recv_hash[sender1][sender2][sender3]) { //校验哈希值
//           final_values = values2;
//         }
//         else {
//           final_values = values1;
//         }
//       }
//     }
//   }
// }

const std::vector<uint8_t>& ImprovedJmp::getValues(int sender1, int sender2, int sender3) {
  auto [min, mid, max] = sortThreeNumbers(sender1, sender2, sender3);
  return final_recv_values_[min][mid][max];
}
};  // namespace HoRGod