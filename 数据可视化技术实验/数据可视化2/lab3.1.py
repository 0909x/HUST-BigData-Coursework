import pandas as pd
from pyecharts import options as opts
from pyecharts.charts import Bar

# 1. 读取 student.xls
df = pd.read_excel("student.xls")

# 把“作弊、缺考、空值”处理为 0
df = df.replace({"作弊": 0, "缺考": 0})
df = df.fillna(0)

# 确保所有课程列都转成数值
score_cols = ["英语", "体育", "军训", "数分", "高代", "解几"]
df[score_cols] = df[score_cols].apply(pd.to_numeric, errors="coerce").fillna(0)

# --------------------------
# 2. 计算总分
# --------------------------
df["总分"] = df[score_cols].sum(axis=1)

# 3. 按总分排序（从高到低）
df = df.sort_values("总分", ascending=False)

names = df["姓名"].tolist()
totals = df["总分"].tolist()
genders = df["性别"].tolist()

# 4. 构造 y 轴数据（单一系列，颜色按性别区分）
y_data = [
    {"value": total, "itemStyle": {"color": "#3399FF" if g == "男" else "#FF69B4"}}
    for total, g in zip(totals, genders)
]

# 5. 绘制条形图
bar = (
    Bar(init_opts=opts.InitOpts(width="100%", height="800px"))
    .add_xaxis(names)
    .add_yaxis(
        "总分",
        y_data,
        category_gap="40%",
        label_opts=opts.LabelOpts(is_show=True),
    )
    .set_global_opts(
        title_opts=opts.TitleOpts(title="学生总分统计图（从高到低）"),
        xaxis_opts=opts.AxisOpts(axislabel_opts=opts.LabelOpts(rotate=0)),
        yaxis_opts=opts.AxisOpts(name="总分"),
        toolbox_opts=opts.ToolboxOpts(
            is_show=True,
            feature={
                "saveAsImage": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True),
                "magicType": opts.ToolBoxFeatureMagicTypeOpts(
                    is_show=True, type_=["line", "bar"]
                ),
            },
        ),
    )
)


bar.render("student_total_score.html")
print("已生成：student_total_score.html")
