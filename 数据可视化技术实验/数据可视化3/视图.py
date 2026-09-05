import streamlit as st
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from collections import Counter
from streamlit_echarts import st_echarts
import networkx as nx
import itertools

# 页面基本配置
st.set_page_config(layout="wide", page_title="麦当劳大数据可视化", page_icon="🍟")

@st.cache_data
def load_data():
    try:
        df = pd.read_csv("McDonalds_Reviews_Cleaned.csv")
        df['tokens_list'] = df['tokens_list'].fillna("")
        df['weeks_ago'] = (df['days_ago'] // 7)
        return df
    except Exception as e:
        st.error(f"数据加载失败: {e}")
        return pd.DataFrame()

df = load_data()

# 侧边栏
st.sidebar.header("数据筛选控制台")
if not df.empty:
    all_states = sorted(df['state_full'].dropna().unique().tolist())
    default_states = [s for s in ["Texas", "California", "Florida"] if s in all_states]
    if not default_states: default_states = all_states[:3] if all_states else []

    selected_states = st.sidebar.multiselect("选择州 (可多选):", options=all_states, default=default_states)
    score_range = st.sidebar.select_slider("评分范围:", options=[1, 2, 3, 4, 5], value=(1, 5))
    max_days = int(df['days_ago'].max())
    selected_days = st.sidebar.slider("间范围 (距今天数):", 0, max_days, (0, max_days))

    mask = (df['state_full'].isin(selected_states)) & \
           (df['rating_score'] >= score_range[0]) & \
           (df['rating_score'] <= score_range[1]) & \
           (df['days_ago'] >= selected_days[0]) & \
           (df['days_ago'] <= selected_days[1])
    filtered_df = df[mask].copy()
else:
    filtered_df = pd.DataFrame()

st.title("🍟 麦当劳大数据可视化")
st.markdown("---")

def empty_check(view_name):
    st.info(f"视图 [{view_name}] 目前没有匹配的数据，请调整筛选条件。")

# 1. 评价分布地图
st.markdown("### 📍 1. 评价分布地图")
st.write("**图表作用**：查看每个区域的店铺情况。")
if not filtered_df.empty:
    map_df = filtered_df.groupby(['latitude', 'longitude', 'store_address']).agg({
        'rating_score': 'mean', 'reviewer_id': 'count'}).reset_index()
    map_df['平均评分'] = map_df['rating_score'].round(2)

    fig_map = px.scatter_mapbox(
        map_df, lat="latitude", lon="longitude", size="reviewer_id", color="平均评分",
        color_continuous_scale="RdYlGn", hover_name="store_address",
        labels={'平均评分': '平均评分', 'reviewer_id': '评论数量'},
        hover_data={"latitude": False, "longitude": False, "平均评分": ":.2f", "reviewer_id": True},
        size_max=15, zoom=3, mapbox_style="carto-positron", height=500
    )
    st.plotly_chart(fig_map, use_container_width=True)
    st.caption("**图例说明**：**圆点大小**映射评论总量（人气）；**颜色**从红（1分）到绿（5分）映射满意度。")
else:
    empty_check("评价分布地图")

# 2. 评分星级分布图
st.divider()
st.markdown("### 📊 2. 评分星级分布图")
st.write("**图表作用**：查看评价是趋向于两极分化还是中庸分布。")
if not filtered_df.empty:
    counts = filtered_df['rating_score'].value_counts().sort_index()

    fig_bar = px.bar(x=counts.index, y=counts.values,
                     labels={'x': '星级评分 (1-5星)', 'y': '评论总数 (条)', 'color': '评分'},
                     color=counts.index,
                     color_discrete_sequence=px.colors.diverging.RdYlGn[::2])

    fig_bar.update_layout(showlegend=False)
    fig_bar.update_coloraxes(colorbar_title_text="评分")

    st.plotly_chart(fig_bar, use_container_width=True)
    st.caption("**图例说明**：**横轴**为评分星级，**纵轴**为评论条数，颜色越深代表星级越高。")
else:
    empty_check("评分星级分布图")

# 3. 店铺评分红黑榜
st.divider()
st.markdown("### 🏆 3. 店铺评分红黑榜")
st.write("**图表作用**：查看限制范围内的店铺红黑榜。")
if not filtered_df.empty:
    rank_df = filtered_df.groupby('store_address').agg({'rating_score': 'mean', 'reviewer_id': 'count'}).reset_index()
    rank_df = rank_df[rank_df['reviewer_id'] >= 5]

    col_config = {"评分": st.column_config.NumberColumn("评分", format="%.2f")}

    st.write("**✨ 好评榜 (Top 10)**")
    top_df = rank_df.nlargest(10, 'rating_score').rename(columns={'store_address': '地址', 'rating_score': '评分', 'reviewer_id': '样本量'})
    st.dataframe(top_df, use_container_width=True, hide_index=True, column_config=col_config)

    st.write("**❌ 差评榜 (Bottom 10)**")
    bottom_df = rank_df.nsmallest(10, 'rating_score').rename(columns={'store_address': '地址', 'rating_score': '评分', 'reviewer_id': '样本量'})
    st.dataframe(bottom_df, use_container_width=True, hide_index=True, column_config=col_config)
else:
    empty_check("店铺评分红黑榜")

# 4. 评分演变趋势图
st.divider()
st.markdown("### 📈 4. 评分演变趋势图")
st.write("**图表作用**：查看评分的时间序列波动情况。")
if not filtered_df.empty:
    store_list = sorted(filtered_df['store_address'].unique())
    sel_store = st.selectbox("选择全部店铺/特定店铺:", ["全部店铺"] + store_list)

    if sel_store == "全部店铺":
        t_df = filtered_df.groupby('weeks_ago')['rating_score'].mean().reset_index()
        t_title = "整体平均评分趋势"
    else:
        t_df = filtered_df[filtered_df['store_address'] == sel_store].groupby('weeks_ago')[
            'rating_score'].mean().reset_index()
        t_title = f"{sel_store} 评分走势"

    if not t_df.empty:
        t_df['rating_score'] = t_df['rating_score'].round(2)

        fig_trend = px.line(t_df, x="weeks_ago", y="rating_score", markers=True,
                            labels={'weeks_ago': '距今周数 (0为当前周)', 'rating_score': '平均评分'},
                            title=t_title)

        fig_trend.update_traces(hovertemplate="距今周数: %{x}<br>平均评分: %{y:.2f}")

        fig_trend.update_xaxes(autorange="reversed")
        fig_trend.update_yaxes(range=[0.8, 5.2])
        st.plotly_chart(fig_trend, use_container_width=True)
        st.caption("**图例说明**：**横轴**为距今时间，0代表本周；折线向上代表满意度好转。")
    else:
        empty_check("评分演变趋势图")
else:
    empty_check("评分演变趋势图")

# 5. 评论关键特征词云
st.divider()
st.markdown("### ☁️ 5. 评论关键特征词云")
st.write("**图表作用**：直观展示顾客评价中最核心的高频关键词。")
if not filtered_df.empty:
    tokens = [t.strip() for t in ",".join(filtered_df['tokens_list']).split(",") if len(t.strip()) > 2]
    if tokens:
        word_counts = Counter(tokens)
        top_words = word_counts.most_common(80)
        max_v, min_v = top_words[0][1], top_words[-1][1]
        wc_data = [{"name": n, "value": v,
                    "textStyle": {"color": f"hsl(210, 80%, {70 - ((v - min_v) / (max_v - min_v + 1) * 60):.0f}%)"}} for
                   n, v in top_words]

        wc_option = {
            "tooltip": {"show": True},
            "visualMap": {"show": True, "min": min_v, "max": max_v, "right": 0, "top": "center",
                          "text": ["高频", "低频"], "inRange": {"color": ["#D1E5F7", "#003366"]}},
            "series": [{"type": "wordCloud", "shape": "circle", "sizeRange": [14, 60], "data": wc_data}]
        }
        st_echarts(wc_option, height="450px")
        st.caption("**图例说明**：**字体越大且颜色越深**代表该词出现的频次越高；点击/悬停显示该词在当前限制范围内出现次数。")
    else:
        empty_check("评论关键特征词云")
else:
    empty_check("评论关键特征词云")

# 6. 评论长度-评分箱线图
st.divider()
st.markdown("### 📏 6. 评论长度-评分箱线图")
st.write("**图表作用**：探究差评/好评用户是否倾向于撰写更长、更详细的反馈。")

if not filtered_df.empty:
    box_df = filtered_df.copy()
    box_df['星级数字'] = box_df['rating_score'].astype(int)
    box_df['星级标签'] = box_df['星级数字'].astype(str) + "星"

    # 定义蓝色渐变色序列
    blue_colors = px.colors.sequential.Blues[2:7]

    fig_box = px.box(box_df,
                     x="星级标签",
                     y="review_word_count",
                     color="星级标签",
                     category_orders={"星级标签": ["1星", "2星", "3星", "4星", "5星"]},
                     color_discrete_sequence=blue_colors,
                     labels={"review_word_count": "评论字数"})

    # 手动创建一个隐藏的散点轨迹，仅用于激活右侧的Colorbar
    fig_box.add_trace(go.Scatter(
        x=[None], y=[None],
        mode='markers',
        marker=dict(
            colorscale=blue_colors,
            showscale=True,
            cmin=1,
            cmax=5,
            colorbar=dict(
                title="评分星级",
                thickness=20,
                tickvals=[1, 2, 3, 4, 5],
                ticktext=["1星", "2星", "3星", "4星", "5星"],
                outlinewidth=0
            ),
        ),
        hoverinfo='none',
        showlegend=False
    ))

    fig_box.update_layout(showlegend=False)

    st.plotly_chart(fig_box, use_container_width=True)
    st.caption("**图例说明**：**箱体中心线**为字数中位数；右侧图例展示了从浅蓝到深蓝的色彩映射关系。")
else:
    empty_check("评论长度-评分箱线图")

# 7. 关键词对比雷达图
st.divider()
st.markdown("### 🎯 7. 关键词对比雷达图")
st.write("**图表作用**：多维度对比不同自定义评分组之间关键词特征的显著差异。")
if not filtered_df.empty:
    # 选词数控制
    col_r1, col_r2, col_r3 = st.columns([1, 1, 0.8])
    with col_r1:
        g1_range = st.slider("第一组评分范围:", 1, 5, (1, 2), key="radar_g1")
    with col_r2:
        g2_range = st.slider("第二组评分范围:", 1, 5, (4, 5), key="radar_g2")
    with col_r3:
        # 控制显示维度的滑块
        top_n = st.slider("显示关键词数:", 3, 20, 12, key="radar_n")

    df_g1 = filtered_df[(filtered_df['rating_score'] >= g1_range[0]) & (filtered_df['rating_score'] <= g1_range[1])]
    df_g2 = filtered_df[(filtered_df['rating_score'] >= g2_range[0]) & (filtered_df['rating_score'] <= g2_range[1])]

    if not df_g1.empty and not df_g2.empty:
        c1 = Counter(",".join(df_g1['tokens_list']).split(","))
        c2 = Counter(",".join(df_g2['tokens_list']).split(","))

        # 使用动态的 top_n，并先多取几个以防过滤短词后数量不够
        raw_cats = (c1 + c2).most_common(top_n + 10)
        radar_cats = [w for w, _ in raw_cats if len(w.strip()) > 2][:top_n]

        if radar_cats:
            fig_radar = go.Figure()
            for c, name, col in [(c1, f'第一组 ({g1_range[0]}-{g1_range[1]}星)', '#00008B'),
                                 (c2, f'第二组 ({g2_range[0]}-{g2_range[1]}星)', '#87CEFA')]:
                max_val = max(c.values()) if c.values() else 1
                r = [c.get(w, 0) / max_val for w in radar_cats]
                fig_radar.add_trace(go.Scatterpolar(r=r + [r[0]], theta=radar_cats + [radar_cats[0]],
                                                    fill='toself', name=name, line=dict(color=col)))
            fig_radar.update_layout(polar=dict(radialaxis=dict(visible=True, range=[0, 1])), showlegend=True)
            st.plotly_chart(fig_radar, use_container_width=True)
            st.caption(
                "**图例说明**：轴线代表该词在该组中的相对出现频率；**深蓝色**区域覆盖第一组关键词特征，**浅蓝色**覆盖第二组。")
    else:
        st.warning("所选评分组的数据不足，无法生成雷达图。")
else:
    empty_check("关键词对比雷达图")

# 8. 关键词随时间流变图
st.divider()
st.markdown("### 🌊 8. 关键词随时间流变图")
st.write("**图表作用**：捕捉热点话题的兴衰，识别每个时间段内高频词语。")
if not filtered_df.empty:
    wt_list = []
    for w, group in filtered_df.groupby('weeks_ago'):
        c = Counter(",".join(group['tokens_list']).split(","))
        for word, count in c.most_common(15):
            if len(word)>2: wt_list.append({'weeks_ago': w, 'word': word, 'count': count})
    df_wt = pd.DataFrame(wt_list)
    if not df_wt.empty:
        sel_words = st.multiselect("跟踪关键词:", options=list(df_wt['word'].unique()), default=list(df_wt['word'].unique())[:5])
        fig_stream = px.area(df_wt[df_wt['word'].isin(sel_words)], x="weeks_ago", y="count", color="word")
        fig_stream.update_xaxes(autorange="reversed", title="距今周数")
        st.plotly_chart(fig_stream, use_container_width=True)
        st.caption("**图例说明**：**色块厚度**代表该时间点的词频。")
    else:
        empty_check("关键词随时间流变图")
else:
    empty_check("关键词随时间流变图")

# 9. 关联词网图
st.divider()
st.markdown("### 🕸️ 9. 关联词网图")
st.write("**图表作用**：查看关联紧密的词语网络。")
if not filtered_df.empty:
    net_df = filtered_df[filtered_df['rating_score'] <= 2]
    G = nx.Graph()
    co_occ = Counter()
    for t in net_df['tokens_list']:
        ws = sorted(set([w.strip() for w in t.split(",") if len(w.strip()) > 2]))
        for p in itertools.combinations(ws, 2): co_occ[p] += 1

    pairs = co_occ.most_common(25)
    if pairs:
        max_w, min_w = pairs[0][1], pairs[-1][1]
        for (n1, n2), w in pairs: G.add_edge(n1, n2, weight=w)
        pos = nx.spring_layout(G, k=0.7)

        # 绘制边
        edge_traces = []
        for n1, n2, d in G.edges(data=True):
            x0, y0 = pos[n1]
            x1, y1 = pos[n2]
            nw = (d['weight'] - min_w) / (max_w - min_w + 0.1)
            edge_traces.append(go.Scatter(x=[x0, x1, None], y=[y0, y1, None], mode='lines',
                                          line=dict(width=1.5 + nw * 13,
                                                    color=f'hsla(220,60%,{60 - nw * 60:.0f}%,{0.3 + nw * 0.7:.2f})')))

        # 计算节点大小：根据度数（连接数）动态调整
        # 基础大小 20，每增加一个连接点增加 5，最大不超过 50
        node_sizes = []
        for n in G.nodes():
            degree = G.degree(n)
            size = min(20 + degree * 5, 50)
            node_sizes.append(size)

        # 绘制节点
        node_trace = go.Scatter(x=[pos[n][0] for n in G.nodes()],
                                y=[pos[n][1] for n in G.nodes()],
                                mode='markers+text',
                                text=list(G.nodes()),
                                textposition="top center",
                                marker=dict(size=node_sizes,
                                            color='white',
                                            line=dict(width=2, color='black')))

        fig_net = go.Figure(data=edge_traces + [node_trace])
        fig_net.add_trace(go.Scatter(x=[None], y=[None], mode='markers',
                                     marker=dict(colorscale=[[0, '#999999'], [1, '#000000']], showscale=True,
                                                 cmin=min_w, cmax=max_w, colorbar=dict(title="关联频次", x=1.05))))
        fig_net.update_layout(showlegend=False, xaxis_visible=False, yaxis_visible=False, height=750)
        st.plotly_chart(fig_net, use_container_width=True)
        st.caption(
            "**图例说明**：右侧条展示关联强度；**连线颜色**越深表明联系越紧密；**圆点越大**表示该词与图中更多关键词共现（重要性更高）。")
    else:
        empty_check("关联词网图")
else:
    empty_check("关联词网图")

# 10. 门店评分象限分析图
st.divider()
st.markdown("### 📊 10. 门店评分象限图")
st.write("**图表作用**：通过“人气 vs 质量”两个维度，查看门店分类。")

if not filtered_df.empty:
    q_df = filtered_df.copy()
    q_df['rating_count'] = q_df['rating_count'].astype(str).str.replace(',', '').astype(float)
    quad = q_df.groupby('store_address').agg({'rating_score': 'mean', 'rating_count': 'max'}).reset_index()
    quad['平均评分'] = quad['rating_score'].round(2)

    fig_quad = px.scatter(quad, x="rating_count", y="平均评分", hover_name="store_address",
                          color="平均评分", size="rating_count", color_continuous_scale="RdYlGn",
                          hover_data={"平均评分": ":.2f", "rating_count": True},
                          labels={'rating_count': '总评论数 (人气指标)', '平均评分': '平均评分 (质量指标)'})

    med_r, med_c = quad['平均评分'].median(), quad['rating_count'].median()
    fig_quad.add_hline(y=med_r, line_dash="dash", line_color="gray", annotation_text=f"质量均线: {med_r:.2f}")
    fig_quad.add_vline(x=med_c, line_dash="dash", line_color="gray", annotation_text=f"人气均线: {int(med_c)}")
    fig_quad.update_yaxes(range=[0.8, 5.2])

    st.plotly_chart(fig_quad, use_container_width=True)
    st.caption(f"**图例说明**：**横轴**为评论总量；**纵轴**为平均评分；**圆点大小**代表评论总数（人气）；**右上角**为高流量高评分的优秀店铺。")
else:
    empty_check("门店评分象限图")