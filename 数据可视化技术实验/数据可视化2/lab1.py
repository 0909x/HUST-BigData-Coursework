import json
from pyecharts import options as opts
from pyecharts.charts import Graph

# 读取 weibo.json 数据
with open("weibo.json", "r", encoding="utf-8") as f:
    j = json.load(f)
    nodes, links, categories, cont, mid, userl = j

# 构造关系图
graph = (
    Graph(
        init_opts=opts.InitOpts(
            width="100%", height="900px"   # 全屏
        )
    )
    .add(
        "",
        nodes,
        links,
        categories=categories,
        repulsion=120,  # 加大斥力，节点更分散，无需手动调整
        linestyle_opts=opts.LineStyleOpts(curve=0.2, opacity=0.7),
        label_opts=opts.LabelOpts(is_show=True),
    )
    .set_global_opts(
        legend_opts=opts.LegendOpts(is_show=False),
        toolbox_opts=opts.ToolboxOpts(
            is_show=True,
            feature={
                "saveAsImage": {},
                "restore": None,
                "dataZoom": None,
                "dataView": None,
                "magicType": None,
            }
        ),
        title_opts=opts.TitleOpts(title="微博转发关系图"),
    )
)

graph.render("weibo_graph.html")
print("图已生成为 weibo_graph.html")
