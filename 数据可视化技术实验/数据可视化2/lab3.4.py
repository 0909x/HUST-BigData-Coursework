import pandas as pd
from pyecharts import options as opts
from pyecharts.charts import Bar

# 1. 读取 student.xls
df = pd.read_excel("student.xls")
df = df.replace({"作弊": 0, "缺考": 0}).fillna(0)

score_cols = ["英语", "体育", "军训", "数分", "高代", "解几"]
df[score_cols] = df[score_cols].apply(pd.to_numeric, errors="coerce").fillna(0)

# 2. 计算男女平均成绩并四舍五入到2位小数
avg_scores = df.groupby("性别")[score_cols].mean().round(2)
genders = ["男", "女"]
avg_scores = avg_scores.reindex(genders)

# 3. 按平均分排序课程（男女总平均分）
total_avg = avg_scores.mean(axis=0)  # 课程总平均分
sorted_courses = total_avg.sort_values(ascending=False).index.tolist()  # 从高到低排序

# 4. 绘制分组柱状图
bar = (
    Bar(init_opts=opts.InitOpts(width="100%", height="600px"))
    .add_xaxis(sorted_courses)
    .add_yaxis(
        series_name="男生",
        y_axis=[avg_scores.loc["男", c] for c in sorted_courses],
        itemstyle_opts=opts.ItemStyleOpts(color="#4E79A7"),
        label_opts=opts.LabelOpts(is_show=True),
    )
    .add_yaxis(
        series_name="女生",
        y_axis=[avg_scores.loc["女", c] for c in sorted_courses],
        itemstyle_opts=opts.ItemStyleOpts(color="#FF6B6B"),
        label_opts=opts.LabelOpts(is_show=True),
    )
    .set_global_opts(
        title_opts=opts.TitleOpts(title="男生和女生各科平均成绩对比（按平均分从高到低）"),
        xaxis_opts=opts.AxisOpts(name="课程"),
        yaxis_opts=opts.AxisOpts(name="平均成绩"),
        tooltip_opts=opts.TooltipOpts(trigger="axis"),
        legend_opts=opts.LegendOpts(pos_top="5%", pos_right="5%"),
        toolbox_opts=opts.ToolboxOpts(
            is_show=True,
            feature={
                "saveAsImage": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True),
                "magicType": opts.ToolBoxFeatureSaveAsImageOpts(is_show=True, type_=["line", "bar"])
            },
        ),
    )
)

bar.render("gender_avg_scores.html")
print("已生成：gender_avg_scores.html")
