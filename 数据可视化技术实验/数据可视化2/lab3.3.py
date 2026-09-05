import pandas as pd
from pyecharts import options as opts
from pyecharts.charts import Line

# 1. 读取 student.xls
df = pd.read_excel("student.xls")
df = df.replace({"作弊": 0, "缺考": 0}).fillna(0)

score_cols = ["英语", "数分", "高代", "解几"]
df[score_cols] = df[score_cols].apply(pd.to_numeric, errors="coerce").fillna(0)

# 2. 构建分数段统计
bins = list(range(0, 101, 10))  # 0-10, 10-20, ..., 90-100
labels = [f"{i}-{i+9}" for i in bins[:-1]]  # 横坐标标签

# 统计每门课程在每个分数段的人数
score_distributions = {}
for col in score_cols:
    score_distributions[col] = pd.cut(df[col], bins=bins, right=False).value_counts().sort_index().tolist()

# 3. 绘制折线图
line = (
    Line(init_opts=opts.InitOpts(width="100%", height="600px"))
    .add_xaxis(labels)
)

# 为每门课程添加折线
colors = ["#FF6B6B", "#4E79A7", "#F28E2B", "#76B7B2"]  # 课程颜色固定
for i, col in enumerate(score_cols):
    line.add_yaxis(
        series_name=col,
        y_axis=score_distributions[col],
        is_smooth=True,  # 平滑曲线
        label_opts=opts.LabelOpts(is_show=True),
        linestyle_opts=opts.LineStyleOpts(color=colors[i]),
        symbol="circle",
        symbol_size=8,
    )

# 4. 全局配置
line.set_global_opts(
    title_opts=opts.TitleOpts(title="四门课程成绩分布（每10分一段）"),
    xaxis_opts=opts.AxisOpts(name="分数段"),
    yaxis_opts=opts.AxisOpts(name="人数"),
    tooltip_opts=opts.TooltipOpts(trigger="axis"),
    legend_opts=opts.LegendOpts(pos_top="5%", pos_right="5%"),
    toolbox_opts=opts.ToolboxOpts(
        is_show=True,
        feature={
            "saveAsImage": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True),
            "magicType": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True, type_=["line", "bar"])
        },
    )
)


line.render("four_subjects_score.html")
print("已生成：four_subjects_score.html")
