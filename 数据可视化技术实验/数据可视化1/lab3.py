import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams
import numpy as np

# 中文显示设置
rcParams['font.sans-serif'] = ['SimHei']
rcParams['axes.unicode_minus'] = False

# 读取Excel数据
df_prov = pd.read_excel('covid19_data.xls', sheet_name='current_prov')

# 确保数值列为数字类型
for col in ['confirm', 'dead', 'heal']:
    df_prov[col] = pd.to_numeric(df_prov[col], errors='coerce')

# 条形图数据
provinces = df_prov['province'].tolist()
confirm = df_prov['confirm'].tolist()
dead = df_prov['dead'].tolist()
heal = df_prov['heal'].tolist()

x = np.arange(len(provinces))
width = 0.25
offset = 1.0  # 条形图整体右移约两个空白格

fig, axes = plt.subplots(2, 1, figsize=(16,12))

# 条形图
bars_confirm = axes[0].bar(x - width + offset, confirm, width=width, color='red', label='确诊')
bars_dead    = axes[0].bar(x + offset, dead, width=width, color='black', label='死亡')
bars_heal    = axes[0].bar(x + width + offset, heal, width=width, color='green', label='治愈')

axes[0].set_yscale('log')
axes[0].set_xticks(x + offset)
axes[0].set_xticklabels(provinces, rotation=0)
axes[0].set_ylabel('人数（对数）', fontsize=12)
axes[0].set_title('各省新冠疫情条形图', fontsize=14)
axes[0].legend()
axes[0].grid(False)  # 移除网格
axes[0].set_xlim(-0.5 + offset, len(provinces)-0.5 + offset)

# 只标注确诊柱子
dx = 0.05  # x轴偏移
for bar in bars_confirm:
    height = bar.get_height()
    y_pos = height * 1.10 if height>0 else 1  # 上移10%
    axes[0].text(bar.get_x() + bar.get_width()/2 + dx, y_pos, f'{int(height)}',
                 ha='center', va='bottom', fontsize=10)

# 直方图
confirm_arr = np.array(confirm)
bins_edges = [0, 100, 500, 1000, 2000, 50000, confirm_arr.max()+1]
counts, _ = np.histogram(confirm_arr, bins=bins_edges)

num_bins = len(counts)
bar_positions = np.arange(num_bins)
bar_width = 0.8

for i in range(num_bins):
    axes[1].bar(bar_positions[i], counts[i], width=bar_width, color='orange', edgecolor='black')
    axes[1].text(bar_positions[i], counts[i]+0.1, str(counts[i]), ha='center', va='bottom', fontsize=10)

labels = []
for i in range(len(bins_edges)-1):
    left = int(bins_edges[i])
    right = int(bins_edges[i+1])
    labels.append(f"{left}-{right-1}")

axes[1].set_xticks(bar_positions)
axes[1].set_xticklabels(labels, rotation=0, ha='center')
axes[1].set_xlabel('确诊人数区间', fontsize=12)
axes[1].set_ylabel('省份数', fontsize=12)
axes[1].set_title('各省新冠疫情确诊人数直方图', fontsize=14)
axes[1].grid(True, linestyle='--', linewidth=0.5)

# 调整左右边距
plt.tight_layout()
plt.subplots_adjust(left=0.17, right=0.95)

plt.savefig("条形图_直方图.png")
plt.show()
