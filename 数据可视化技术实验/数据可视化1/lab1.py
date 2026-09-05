import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import rcParams

# 中文显示设置
rcParams['font.sans-serif'] = ['SimHei']
rcParams['axes.unicode_minus'] = False

# 读取Excel数据
df = pd.read_excel('covid19_data.xls', sheet_name='data_history', header=None)
df.columns = ['date', 'confirm', 'dead', 'heal', 'suspect']

# 提取月份-日期
df['date_only'] = df['date'].str[5:].astype(str)

# 确保数值列为数字
df['confirm'] = pd.to_numeric(df['confirm'], errors='coerce')
df['dead'] = pd.to_numeric(df['dead'], errors='coerce')
df['heal'] = pd.to_numeric(df['heal'], errors='coerce')

# 绘制折线图和散点图
fig, axes = plt.subplots(2, 1, figsize=(16,10))

# 横坐标每隔一天显示刻度
xticks_pos = list(range(0, len(df), 2))  # 每隔一天
xticks_labels = [df['date_only'][i] for i in xticks_pos]

# 折线图
axes[0].plot(df['date_only'], df['confirm'], color='red', linestyle='-', marker='o', label='确诊')
axes[0].plot(df['date_only'], df['dead'], color='black', linestyle='--', marker='x', label='死亡')
axes[0].plot(df['date_only'], df['heal'], color='green', linestyle='-.', marker='s', label='治愈')
axes[0].set_title('2020年新冠疫情数据折线图', fontsize=14)
axes[0].set_xlabel('日期', fontsize=12)
axes[0].set_ylabel('人数', fontsize=12)
axes[0].set_xticks(xticks_pos)
axes[0].set_xticklabels(xticks_labels, rotation=0)
axes[0].legend()
axes[0].grid(True)

# 散点图
axes[1].scatter(df['date_only'], df['confirm'], color='red', marker='o', label='确诊')
axes[1].scatter(df['date_only'], df['dead'], color='black', marker='x', label='死亡')
axes[1].scatter(df['date_only'], df['heal'], color='green', marker='s', label='治愈')
axes[1].set_title('2020年新冠疫情数据散点图）', fontsize=14)
axes[1].set_xlabel('日期', fontsize=12)
axes[1].set_ylabel('人数', fontsize=12)
axes[1].set_xticks(xticks_pos)
axes[1].set_xticklabels(xticks_labels, rotation=0)
axes[1].legend()
axes[1].grid(True)

plt.tight_layout()
plt.savefig('折线图_散点图.png')
plt.show()
