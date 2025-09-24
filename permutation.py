perm_dot_inv = [[9, 8, 7, 6, 5, 4, 3, 2, 1, 0],
                [7, 9, 3, 6, 8, 5, 2, 4, 0, 1]]
alpha_sum_dot_vec = [[8, 9, 6, 2, 7, 5, 3, 0, 4, 1],
                     [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]]
beta_dot_alpha_sum = [[1, 4, 0, 3, 5, 7, 2, 6, 9, 8],
                      [8, 9, 6, 2, 7, 5, 3, 0, 4, 1]]
alpha_i = [[2, 0, 4, 5, 1, 8, 6, 9, 7, 3],
           [1, 0, 9, 5, 2, 8, 6, 3, 7, 4],
           [5, 3, 2, 0, 4, 1, 7, 9, 8, 6],
           [9, 7, 6, 0, 2, 4, 5, 8, 1, 3],
           [8, 9, 6, 2, 7, 5, 3, 0, 4, 1]]
def permu(pi):
    result = list(range(len(pi[0])))
    for j in range(len(pi)-1,-1,-1):
        perm = pi[j]
        temp = result[:]
        for i in range(len(perm)):
            result[perm[i]] = temp[i]
    return result

def inverse_permutation(perm):
    """计算置换的逆"""
    n = len(perm)
    inv = [0] * n
    for i in range(n):
        inv[perm[i]] = i
    return inv

def compose_permutations(p1, p2):
    """计算两个置换的复合 p1 ∘ p2"""
    n = len(p1)
    result = [0] * n
    for i in range(n):
        result[i] = p1[p2[i]]
    return result

def solve_beta(alpha, pi):
    """求解 β 使得 π = β ∘ α"""
    # 计算 α 的逆
    alpha_inv = inverse_permutation(alpha)
    
    # 计算 β = π ∘ α⁻¹
    beta = compose_permutations(pi, alpha_inv)
    
    return beta

print(inverse_permutation(permu(alpha_i)))
# # 示例
# alpha = [8, 9, 6, 2, 7, 5, 3, 0, 4, 1]
# pi = [9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

# beta = solve_beta(alpha, pi)
# print("α:", alpha)
# print("π:", pi)
# print("β:", beta)

# # 验证：β ∘ α 应该等于 π
# verification = compose_permutations(beta, alpha)
# print("验证 β ∘ α:", verification)
# print("验证是否等于 π:", verification == pi)