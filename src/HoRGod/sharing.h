#pragma once

#include <emp-tool/emp-tool.h>

#include <array>
#include <vector>

#include "helpers.h"
#include "types.h"

namespace HoRGod {

template <class R>
class ReplicatedShare {
  // values_[i] will denote element common with party having my_id + i + 1.
  std::array<R, 4> values_; //

 public:
  ReplicatedShare() = default;
  explicit ReplicatedShare(std::array<R, 4> values)
      : values_{std::move(values)} {}

  void randomize(emp::PRG& prg) {
    prg.random_data(values_.data(), sizeof(R) * 4);
  }

  void init_zero() {
    for(int i = 0;i<4;i++) {
      values_[i] = 0;
    }
  }
  // Access share elements.
  // idx = i retreives value common with party having my_id + i + 1.
  R& operator[](size_t idx) { return values_.at(idx); }

  R operator[](size_t idx) const { return values_.at(idx); }

  R& commonValueWithParty(int my_id, int pid) {
    int idx = pid - my_id;
    if (idx < 0) {
      idx += 5;
    }
    idx -= 1;

    return values_.at(idx);
  }

  std::array<R, 3> commonTreeValues(int my_id, int pid) {
    std::array<R, 5> values_temp;
    int pos = 0;
    //先假装有5个秘密共享，实际上只有4个
    for (int i = 0; i < 5; ++i) {
        if (i == my_id) {
          values_temp[i] = 0;
        }
        else {
          values_temp[i] = values_.at(pos);
          pos++;
        }
    }

    //拿着5个秘密共享，看my_id和pid共有的是哪三个
    std::array<R, 3> result;
    pos = 0;
    for (int i = 0; i < 5; ++i) {
        if (i != my_id && i != pid) {
            result[pos++] = values_temp.at(i);
        }
    }
    return result;
  }


  [[nodiscard]] R commonValueWithParty(int my_id, int pid) const {
    int idx = pid - my_id;
    if (idx < 0) {
      idx += 5;
    }
    idx -= 1;

    return values_.at(idx);
  }

  [[nodiscard]] R sum() const { return values_[0] + values_[1] + values_[2] + values_[3]; }

  // Arithmetic operators.
  ReplicatedShare<R>& operator+=(const ReplicatedShare<R> rhs) {
    values_[0] += rhs.values_[0];
    values_[1] += rhs.values_[1];
    values_[2] += rhs.values_[2];
    values_[3] += rhs.values_[3];
    return *this;
  }

  
  ReplicatedShare<R> cosnt_add(const R& rhs) const {
    ReplicatedShare<R> result = *this;  // 复制当前对象
    result.values_[0] += rhs;           // 每个元素加常量
    result.values_[1] += rhs;
    result.values_[2] += rhs;
    result.values_[3] += rhs;
    return result;                      // 返回新对象
  }

  ReplicatedShare<R> cosnt_mul(const R& rhs) const {
    ReplicatedShare<R> result = *this;  // 复制当前对象
    result.values_[0] *= rhs;           // 每个元素加常量
    result.values_[1] *= rhs;
    result.values_[2] *= rhs;
    result.values_[3] *= rhs;
    return result;                      // 返回新对象
  }

  friend ReplicatedShare<R> operator+(ReplicatedShare<R> lhs,
                                      const ReplicatedShare<R>& rhs) {
    lhs += rhs;
    return lhs;
  }

  ReplicatedShare<R>& operator-=(const ReplicatedShare<R>& rhs) {
    (*this) += (rhs * -1);
    return *this;
  }

  friend ReplicatedShare<R> operator-(ReplicatedShare<R> lhs,
                                      const ReplicatedShare<R>& rhs) {
    lhs -= rhs;
    return lhs;
  }

  ReplicatedShare<R>& operator*=(const R& rhs) {
    values_[0] *= rhs;
    values_[1] *= rhs;
    values_[2] *= rhs;
    values_[3] *= rhs;
    return *this;
  }

  friend ReplicatedShare<R> operator*(ReplicatedShare<R> lhs, const R& rhs) {
    lhs *= rhs;
    return lhs;
  }

  ReplicatedShare<R>& add(R val, int pid) {
    // if (pid == 0) {
    //   values_[0] += val;
    // } else if (pid == 1) {
    //   values_[2] += val;
    // }
    values_[pid] += val;

    return *this;
  }
};

// Contains all elements of a secret sharing. Used only for generating dummy
// preprocessing data.
template <class R>
struct DummyShare {
  std::array<R, 5> share_elements;

  DummyShare() = default;

  explicit DummyShare(std::array<R, 5> share_elements)
      : share_elements(std::move(share_elements)) {}

  DummyShare(R secret, emp::PRG& prg) {
    prg.random_data(share_elements.data(), sizeof(R) * 4);

    R sum = share_elements[0];
    for (int i = 1; i < 4; ++i) {
      sum += share_elements[i]; //把前四个数相加
    }
    share_elements[4] = secret - sum; //最后一个共享的值
  }
  
  void randomize(emp::PRG& prg) {
    prg.random_data(share_elements.data(), sizeof(R) * 5); //随机化5个值
  }

  [[nodiscard]] R secret() const { //返回5个随机值的和，秘密共享\beta = x + sum即可
    R sum = share_elements[0];
    for (size_t i = 1; i < 5; ++i) {
      sum += share_elements[i];
    }
    return sum;
  }

  DummyShare<R>& operator+=(const DummyShare<R>& rhs) {
    for (size_t i = 0; i < 5; ++i) {
      share_elements[i] += rhs.share_elements[i];
    }

    return *this;
  }

  friend DummyShare<R> operator+(DummyShare<R> lhs, const DummyShare<R>& rhs) {
    lhs += rhs;
    return lhs;
  }

  DummyShare<R>& operator-=(const DummyShare<R>& rhs) {
    for (size_t i = 0; i < 5; ++i) {
      share_elements[i] -= rhs.share_elements[i];
    }

    return *this;
  }

  friend DummyShare<R> operator-(DummyShare<R> lhs, const DummyShare<R>& rhs) {
    lhs -= rhs;
    return lhs;
  }

  DummyShare<R>& operator*=(const R& rhs) {
    for (size_t i = 0; i < 5; ++i) {
      share_elements[i] *= rhs;
    }

    return *this;
  }

  friend DummyShare<R> operator*(DummyShare<R> lhs, const R& rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend DummyShare<R> operator*(const R& lhs, DummyShare<R> rhs) {
    // Assumes abelian ring.
    rhs *= lhs;
    return rhs;
  }

  ReplicatedShare<R> getRSS(size_t pid) {//返回对应的冗余秘密共享，对于pid=0，返回的共享值为1,2,3,4，即不包含0
    pid = pid % 5;
    std::array<R, 4> values;
    size_t counter = 0;
    for (size_t i = 0; i < 5; ++i) {
      if (i != pid) { 
        values[counter] = share_elements.at(i);
        counter++;
      }
    }
    return ReplicatedShare<R>(values);
  }

  R getShareElement(size_t i, size_t j) {
    return share_elements.at(upperTriangularToArray(i, j));
  }
};

template <class R>
struct PermShare {
  std::array<vector<R>, 4> share_elements;
  size_t length_;

  PermShare() = default;

  explicit PermShare(std::array<vector<R>, 4> share_elements, size_t length)
      : share_elements(std::move(share_elements)) {
    length_ = length;
  }
  
  // 生成一个 [0, length_-1] 的随机置换
  std::vector<R> random_permutation(emp::PRG& prg, size_t length_) {
    // 1. 初始化序列 0,1,2,...,length_-1
    std::vector<R> perm(length_);
    for (size_t i = 0; i < length_; i++) {
        perm[i] = static_cast<R>(i);
    }

    // 2. 用 prg 随机生成打乱顺序的随机数
    //    这里我们用 Fisher-Yates 洗牌算法
    for (size_t i = length_ - 1; i > 0; --i) {
        uint32_t rand_val;
        prg.random_data(&rand_val, sizeof(rand_val));
        size_t j = rand_val % (i + 1); // 随机选择 [0, i]
        std::swap(perm[i], perm[j]);
    }
    return perm;
  }

  void randomize(emp::PRG& prg) {
    for (size_t i = 0; i < 4; i++) {
      share_elements[i] = random_permutation(prg, length_);
    }
  }

  // Access share elements.
  // idx = i retreives value common with party having my_id + i + 1.
  vector<R>& operator[](size_t idx) { return share_elements[idx]; }

  vector<R> operator[](size_t idx) const { return share_elements[idx]; }

  
};

template <>
void ReplicatedShare<BoolRing>::randomize(emp::PRG& prg);

template <>
DummyShare<BoolRing>::DummyShare(BoolRing secret, emp::PRG& prg);

template <>
void DummyShare<BoolRing>::randomize(emp::PRG& prg);

template <class R>
class PermutationShare {
  // values_[i] will denote element common with party having my_id + i + 1.
  std::array<std::vector<R>, 4> values_;

 public:
  PermutationShare() = default;
  explicit PermutationShare(std::array<std::vector<R>, 4> values)
      : values_{std::move(values)} {}
  
  std::vector<R>& operator[](size_t idx) { return values_.at(idx); }

  std::vector<R> operator[](size_t idx) const { return values_.at(idx); }

  [[nodiscard]] std::vector<R> sum() const { //返回5个随机值的和，秘密共享\beta = x + sum即可
    std::vector<R> sum = values_[0];
    for (size_t i = 1; i < 4; ++i) {
      sum = composePermutations(values_[i], sum);
    }
    return sum;
  }
};

template <class R>
class PermutationDummyShare {
  // values_[i] will denote element common with party having my_id + i + 1.
  std::array<std::vector<R>, 5> values_;
  uint64_t permutation_length_;

 public:
  PermutationDummyShare() = default;
  explicit PermutationDummyShare(std::array<std::vector<R>, 5> values, uint64_t permutation_length)
      : values_{std::move(values)} {
    permutation_length_ = permutation_length;
  }

  explicit PermutationDummyShare(uint64_t permutation_length) {
    permutation_length_ = permutation_length;
  }

  std::vector<R> generateRandomPermutation(emp::PRG& prg, uint64_t permutation_length) {
    std::vector<R> permutation;
    
    // 创建初始序列 [0, 1, 2, ..., n-1]
    for (int i = 0; i < permutation_length; ++i) {
      permutation.push_back(i);
    }
    
    // 使用 Fisher-Yates 洗牌算法
    for (int i = permutation_length - 1; i > 0; --i) {
        // 生成 [0, i] 范围内的随机数
        R rand_val;
        prg.random_data(&rand_val, sizeof(R));
        int j = rand_val % (i + 1);
        
        // 交换元素
        std::swap(permutation[i], permutation[j]);
    }
    return permutation;
  }

  void randomize(emp::PRG& prg) {
    for(int i = 0; i < 5; i++) {
      values_[i] = generateRandomPermutation(prg, permutation_length_);
    }
  }

  [[nodiscard]] std::vector<R> secret() const { //返回5个随机值的和，秘密共享\beta = x + sum即可
    std::vector<R> sum = values_[0];
    for (size_t i = 1; i < 5; ++i) {
      sum = composePermutations(values_[i], sum);
    }
    return sum;
  }

  PermutationShare <R> getRSS(size_t pid) {//返回对应的冗余秘密共享，对于pid=0，返回的共享值为1,2,3,4，即不包含0
    pid = pid % 5;
    std::array<std::vector<R>, 4> values;
    size_t counter = 0;
    for (size_t i = 0; i < 5; ++i) {
      if (i != pid) {
        values[counter] = values_[i];
        counter++;
      }
    }
    return PermutationShare<R>(values);
  }
};

};  // namespace HoRGod
