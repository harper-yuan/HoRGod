import re

def analyze_log(file_path):
    times = []
    sents = []

    # 定义匹配模式
    time_pattern = re.compile(r"time:\s+([\d.]+)\s+ms")
    sent_pattern = re.compile(r"sent:\s+(\d+)\s+bytes")

    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            
            # 查找所有匹配项
            times = [float(t) for t in time_pattern.findall(content)]
            sents = [int(s) for s in sent_pattern.findall(content)]

        if not times or not sents:
            print("未在文件中找到有效的数据。")
            return

        # 计算统计信息
        avg_time = sum(times) / len(times)
        avg_sent = sum(sents) / len(sents)

        print(f"--- 统计结果 ---")
        print(f"样本数量: {len(times)}")
        print(f"平均时间 (Average Time): {avg_time:.4f} ms")
        print(f"平均通信量 (Average Sent): {avg_sent:.2f} bytes")
        print(f"时间最大值: {max(times):.4f} ms")
        print(f"时间最小值: {min(times):.4f} ms")

    except FileNotFoundError:
        print(f"错误：找不到文件 '{file_path}'")

if __name__ == "__main__":
    # 假设你的日志文件名为 log.txt
    analyze_log("log.txt")