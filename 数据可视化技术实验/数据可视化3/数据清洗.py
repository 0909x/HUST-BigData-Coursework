import pandas as pd
import re
import numpy as np

# 加载原始数据
df = pd.read_csv("McDonald_s_Reviews.csv", encoding="latin-1")
df.columns = df.columns.str.strip()


us_state_to_abbrev = {
    "AL": "Alabama", "AK": "Alaska", "AZ": "Arizona", "AR": "Arkansas", "CA": "California",
    "CO": "Colorado", "CT": "Connecticut", "DE": "Delaware", "FL": "Florida", "GA": "Georgia",
    "HI": "Hawaii", "ID": "Idaho", "IL": "Illinois", "IN": "Indiana", "IA": "Iowa",
    "KS": "Kansas", "KY": "Kentucky", "LA": "Louisiana", "ME": "Maine", "MD": "Maryland",
    "MA": "Massachusetts", "MI": "Michigan", "MN": "Minnesota", "MS": "Mississippi", "MO": "Missouri",
    "MT": "Montana", "NE": "Nebraska", "NV": "Nevada", "NH": "New Hampshire", "NJ": "New Jersey",
    "NM": "New Mexico", "NY": "New York", "NC": "North Carolina", "ND": "North Dakota", "OH": "Ohio",
    "OK": "Oklahoma", "OR": "Oregon", "PA": "Pennsylvania", "RI": "Rhode Island", "SC": "South Carolina",
    "SD": "South Dakota", "TN": "Tennessee", "TX": "Texas", "UT": "Utah", "VT": "Vermont",
    "VA": "Virginia", "WA": "Washington", "WV": "West Virginia", "WI": "Wisconsin", "WY": "Wyoming",
    "DC": "District of Columbia"
}


def get_full_state(address):
    if pd.isna(address): return None
    # 将地址按逗号分割并去除空格
    parts = [p.strip() for p in address.split(',')]

    # 从后往前找，第一个匹配 us_state_to_abbrev 的项
    # 有些地址以 "United States" 结尾，有些则以 "TX 78750" 结尾
    for p in reversed(parts):
        # 提取每一段开头的两个字母（如 "TX 78750" -> "TX"）
        potential_abbr = p.split(' ')[0].upper()
        if potential_abbr in us_state_to_abbrev:
            return us_state_to_abbrev[potential_abbr]

    return "Unknown"  # 如果都没匹配到


df['state_full'] = df['store_address'].apply(get_full_state)


def count_words(text):
    if pd.isna(text): return 0
    return len(re.findall(r'\w+', str(text)))


df['review_word_count'] = df['review'].apply(count_words)


def parse_time_enhanced(time_str):
    if pd.isna(time_str): return 0
    t = str(time_str).lower().replace('a ', '1 ')
    num_match = re.search(r'\d+', t)
    num = int(num_match.group()) if num_match else 1
    if 'year' in t: return num * 365
    if 'month' in t: return num * 30
    if 'week' in t: return num * 7
    return num


df['days_ago'] = df['review_time'].apply(parse_time_enhanced)
df['rating_score'] = df['rating'].str.extract(r'(\d+)').astype(float)
# 停用词
stop_words = {'the', 'and', 'i', 'to', 'a', 'was', 'it', 'for', 'in', 'of', 'my', 'on', 'with', 'is', 'this', 'that','they','you','are','from','their','them'}


def tokenize_to_str(text):
    if pd.isna(text): return ""
    words = re.sub(r'[^a-zA-Z\s]', '', str(text).lower()).split()
    return ",".join([w for w in words if w not in stop_words and len(w) > 2])


df['tokens_list'] = df['review'].apply(tokenize_to_str)

# 剔除冗余字段
# 移除 review, review_time, rating, category
cols_to_drop = ['review', 'review_time', 'rating', 'category']
df_cleaned = df.drop(columns=cols_to_drop)

df_cleaned.to_csv("McDonalds_Reviews_Cleaned.csv", index=False, encoding="utf-8-sig")
print("清洗任务完成！")