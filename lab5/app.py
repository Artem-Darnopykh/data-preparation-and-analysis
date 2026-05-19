import streamlit as st
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os
import urllib.request
from io import StringIO

st.set_page_config(page_title="ЛР5: NOAA Аналіз (Streamlit)", layout="wide")

REINDEX_MAP = {
    1: 22, 2: 24, 3: 23, 4: 25, 5: 3, 6: 4, 7: 8, 8: 19, 9: 20, 10: 5,
    11: 9, 12: 26, 13: 10, 14: 11, 15: 12, 16: 13, 17: 14, 18: 15, 19: 16,
    20: 27, 21: 17, 22: 18, 23: 7, 24: 1, 25: 2, 26: 6, 27: 21
}

PROVINCES = {
    1: "Вінницька", 2: "Волинська", 3: "Дніпропетровська", 4: "Донецька",
    5: "Житомирська", 6: "Закарпатська", 7: "Запорізька", 8: "Івано-Франківська",
    9: "Київська", 10: "Кіровоградська", 11: "Луганська", 12: "Львівська",
    13: "Миколаївська", 14: "Одеська", 15: "Полтавська", 16: "Рівненська",
    17: "Сумська", 18: "Тернопільська", 19: "Харківська", 20: "Херсонська",
    21: "Хмельницька", 22: "Черкаська", 23: "Чернівецька", 24: "Чернігівська",
    25: "Республіка Крим", 26: "Київ", 27: "Севастополь"
}

def download_data(data_dir="data"):
    os.makedirs(data_dir, exist_ok=True)
    for province_id in range(1, 28):
        existing = [f for f in os.listdir(data_dir) if f.startswith(f"vhi_id_{province_id}_")]
        if existing:
            continue
        url = (
            f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php"
            f"?country=UKR&provinceID={province_id}&year1=1981&year2=2024&type=Mean"
        )
        try:
            urllib.request.urlretrieve(url, os.path.join(data_dir, f"vhi_id_{province_id}_data.csv"))
        except Exception as e:
            st.warning(f"Не вдалось завантажити область {province_id}: {e}")

@st.cache_data
def load_data(data_dir="data"):
    if not os.path.exists(data_dir) or not any(f.endswith(".csv") for f in os.listdir(data_dir)):
        with st.spinner("Завантаження даних з NOAA... (лише при першому запуску)"):
            download_data(data_dir)

    frames = []
    for filename in sorted(os.listdir(data_dir)):
        if not filename.endswith(".csv"):
            continue
        file_path = os.path.join(data_dir, filename)
        if os.stat(file_path).st_size == 0:
            continue
        try:
            noaa_id = int(filename.split("_")[2])
            new_id = REINDEX_MAP.get(noaa_id)
            if new_id is None:
                continue

            # Читаємо сирий текст і прибираємо html-сміття (специфіка формату NOAA)
            with open(file_path, 'r', encoding='utf-8') as f:
                raw = f.read()
            raw = raw.replace('<br>', '').replace('<tt><pre>', '').replace('</tt></pre>', '')

            df = pd.read_csv(StringIO(raw), header=1, index_col=False)
            df.columns = df.columns.str.strip()
            df = df.rename(columns={'year': 'Year', 'week': 'Week'})

            for col in ['Year', 'Week', 'VCI', 'TCI', 'VHI']:
                df[col] = pd.to_numeric(df[col], errors='coerce')

            df['Region_ID'] = new_id
            df['Region'] = PROVINCES.get(new_id, "Невідомо")

            df = df.dropna(subset=['Year', 'Week', 'VHI'])
            df = df[df['VHI'] > 0]

            frames.append(df[['Year', 'Week', 'Region_ID', 'Region', 'VCI', 'TCI', 'VHI']])

        except Exception as e:
            st.warning(f"Помилка при читанні {filename}: {e}")
            continue

    if not frames:
        st.error("Не вдалось зчитати жодного файлу! Перевір папку data/")
        st.stop()

    return pd.concat(frames, ignore_index=True)

df = load_data()

def reset_filters():
    for key in st.session_state.keys():
        del st.session_state[key]

st.title("Аналіз вегетаційних індексів (VCI, TCI, VHI)")

col1, col2 = st.columns([1, 3])

with col1:
    st.header("⚙️ Налаштування")
    
    st.button("Скинути налаштування", on_click=reset_filters)
    st.markdown("---")
    
    index_type = st.selectbox("Оберіть індекс", ['VCI', 'TCI', 'VHI'], key='index_type')
    region = st.selectbox("Оберіть область", sorted(df['Region'].unique()), key='region')
    
    weeks = st.slider("Інтервал тижнів", 1, 52, (1, 52), key='weeks')
    years = st.slider("Інтервал років", 1981, 2024, (1981, 2024), key='years')
    
    st.markdown("---")
    st.write("Сортування даних у таблиці:")
    sort_asc = st.checkbox("За зростанням", key='sort_asc')
    sort_desc = st.checkbox("За спаданням", key='sort_desc')

with col2:
    st.header(f"📊 Результати для: {region}")
    
    df_filtered = df[
        (df['Region'] == region) & 
        (df['Week'] >= weeks[0]) & (df['Week'] <= weeks[1]) & 
        (df['Year'] >= years[0]) & (df['Year'] <= years[1])
    ].copy()
    
    if sort_asc and sort_desc:
        st.warning("⚠️ Увага: Обрано обидва типи сортування! Застосовано сортування за зростанням.")
        df_filtered = df_filtered.sort_values(by=index_type, ascending=True)
    elif sort_asc:
        df_filtered = df_filtered.sort_values(by=index_type, ascending=True)
    elif sort_desc:
        df_filtered = df_filtered.sort_values(by=index_type, ascending=False)
        
    tab1, tab2, tab3 = st.tabs(["📋 Таблиця даних", "📈 Графік динаміки", "📊 Порівняння по областях"])
    
    with tab1:
        st.write(f"Відфільтровані дані ({years[0]}-{years[1]}, тижні {weeks[0]}-{weeks[1]})")
        st.dataframe(df_filtered, width='stretch')
        
    with tab2:
        st.write(f"Динаміка індексу **{index_type}** для області: **{region}**")
        if not df_filtered.empty:
            df_trend = df_filtered.groupby('Year')[index_type].mean().reset_index()
            
            fig, ax = plt.subplots(figsize=(10, 5))
            sns.lineplot(data=df_trend, x='Year', y=index_type, marker='o', ax=ax, color='b')
            ax.set_title(f"Середній {index_type} за вибрані тижні по роках")
            ax.set_ylabel(index_type)
            ax.set_xlabel("Рік")
            ax.grid(True, linestyle='--', alpha=0.7)
            st.pyplot(fig)
        else:
            st.info("Немає даних для відображення.")
            
    with tab3:
        st.write(f"Порівняння **{index_type}** з іншими областями за обраний період")
        
        df_all = df[
            (df['Week'] >= weeks[0]) & (df['Week'] <= weeks[1]) & 
            (df['Year'] >= years[0]) & (df['Year'] <= years[1])
        ]
        
        df_comp = df_all.groupby('Region')[index_type].mean().reset_index()
        df_comp = df_comp.sort_values(by=index_type, ascending=False)
        
        fig2, ax2 = plt.subplots(figsize=(12, 6))
        colors = ['tomato' if r == region else 'steelblue' for r in df_comp['Region']]
        sns.barplot(data=df_comp, x='Region', y=index_type, palette=colors, hue='Region', legend=False, ax=ax2)
        plt.xticks(rotation=90)
        ax2.set_title(f"Середній {index_type} по всіх областях ({years[0]}-{years[1]}, тижні {weeks[0]}-{weeks[1]})")
        ax2.set_ylabel(f"Середній {index_type}")
        st.pyplot(fig2)
