import pandas as pd
from pyecharts import options as opts
from pyecharts.charts import Pie, Page

# 1. 读取 student.xls
df = pd.read_excel("student.xls")
df = df.replace({"作弊": 0, "缺考": 0}).fillna(0)

score_cols = ["英语", "体育", "军训", "数分", "高代", "解几"]
course_colors = ["#FF6B6B", "#4E79A7", "#F28E2B", "#76B7B2", "#59A14F", "#EDC948"]
color_map = dict(zip(score_cols, course_colors))  # 课程名→颜色

df[score_cols] = df[score_cols].apply(pd.to_numeric, errors="coerce").fillna(0)
df["总分"] = df[score_cols].sum(axis=1)


# 2. 取总分前3名
top3 = df.sort_values("总分", ascending=False).head(3)


# 3. 创建页面
page = Page(layout=Page.SimplePageLayout)

for idx, row in top3.iterrows():
    student_name = row["姓名"]

    # 先按成绩从小到大排序数据
    sorted_data = sorted([(col, row[col]) for col in score_cols], key=lambda x: x[1])

    # 为每个扇区指定固定颜色
    data_with_style = [
        opts.PieItem(name=name, value=value, itemstyle_opts=opts.ItemStyleOpts(color=color_map[name]))
        for name, value in sorted_data
    ]

    pie = (
        Pie()
        .add(
            "",
            data_with_style,
            radius=["30%", "60%"],
            label_opts=opts.LabelOpts(is_show=True, formatter="{b}: {c}"),
        )
        .set_global_opts(
            title_opts=opts.TitleOpts(title=f"{student_name} 总分构成"),
            legend_opts=opts.LegendOpts(
                pos_top="center",
                pos_right="5%",
                orient="vertical",
            ),
            toolbox_opts=opts.ToolboxOpts(
                is_show=True,
                feature={"saveAsImage": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True)},
            ),
        )
    )
    page.add(pie)


page.render("top3_pie.html")
print("已生成：top3_pie.html")
