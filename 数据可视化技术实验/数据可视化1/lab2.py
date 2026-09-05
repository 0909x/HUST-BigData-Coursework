import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 中文显示设置
rcParams['font.sans-serif'] = ['SimHei']
rcParams['axes.unicode_minus'] = False

# 读取Excel数据
df_world = pd.read_excel('covid19_data.xls', sheet_name='data_world')

# 确保数值列为数字类型
for col in ['confirm', 'dead', 'heal', 'suspect']:
    df_world[col] = pd.to_numeric(df_world[col], errors='coerce')

# 按确诊人数排序，取前4个国家
top4 = df_world.sort_values(by='confirm', ascending=False).head(4)

labels = ['确诊', '死亡', '治愈', '疑似']
colors = ['red', 'black', 'green', 'yellow']

# 创建2x2子图
fig, axes = plt.subplots(2, 2, figsize=(14, 10))

for i, (idx, row) in enumerate(top4.iterrows()):
    data = [row['confirm'], row['dead'], row['heal'], row['suspect']]
    # 从小到大排序
    data_sorted = sorted(zip(data, labels, colors), key=lambda x: x[0])
    values, labels_sorted, colors_sorted = zip(*data_sorted)

    ax = axes[i // 2, i % 2]
    wedges, _ = ax.pie(values, colors=colors_sorted, startangle=90, counterclock=False)
    ax.set_title(f"{row['country']} 2020年新冠疫情数据分布饼图", fontsize=14)

    # 图例显示百分比
    total = sum(values)
    labels_with_pct = [f"{label} ({v / total * 100:.1f}%)" for label, v in zip(labels_sorted, values)]
    ax.legend(wedges, labels_with_pct, title="类型", loc="center left", bbox_to_anchor=(1, 0, 0.5, 1))

plt.tight_layout()
plt.savefig("饼图_前四国家_百分比.png")
plt.show()
