import streamlit as st
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

st.set_page_config(page_title="Засоби підготовки та аналізу даних: Лабораторна №5", layout="wide")

@st.cache_data
def load_data():
    np.random.seed(42)
    regions = ['Вінницька', 'Волинська', 'Дніпропетровська', 'Донецька', 'Житомирська', 
               'Закарпатська', 'Запорізька', 'Івано-Франківська', 'Київська', 'Кіровоградська', 
               'Львівська', 'Миколаївська', 'Одеська', 'Полтавська', 'Рівненська', 'Сумська', 
               'Тернопільська', 'Харківська', 'Херсонська', 'Хмельницька', 'Черкаська', 
               'Чернівецька', 'Чернігівська', 'Крим', 'Київ']
    
    years = np.arange(1981, 2025)
    weeks = np.arange(1, 53)
    
    data = []
    for reg in regions:
        n_records = len(years) * len(weeks)
        y_arr = np.repeat(years, len(weeks))
        w_arr = np.tile(weeks, len(years))
        
        vci = np.random.uniform(10, 90, n_records)
        tci = np.random.uniform(10, 90, n_records)
        vhi = 0.5 * vci + 0.5 * tci
        
        df_temp = pd.DataFrame({
            'Year': y_arr,
            'Week': w_arr,
            'Region': reg,
            'VCI': np.round(vci, 2),
            'TCI': np.round(tci, 2),
            'VHI': np.round(vhi, 2)
        })
        data.append(df_temp)
        
    return pd.concat(data, ignore_index=True)

df = load_data()

# Функція для скидання фільтрів
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
    
    weeks_range = st.slider("Інтервал тижнів", 1, 52, (1, 52), key='weeks')
    years_range = st.slider("Інтервал років", 1981, 2024, (1981, 2024), key='years')
    
    st.markdown("---")
    st.write("Сортування даних у таблиці:")
    
    # Розумна логіка чекбоксів: один вимикає інший
    sort_asc = st.checkbox("За зростанням", key='sort_asc', 
                           on_change=lambda: st.session_state.update(sort_desc=False) if st.session_state.sort_asc else None)
    sort_desc = st.checkbox("За спаданням", key='sort_desc', 
                            on_change=lambda: st.session_state.update(sort_asc=False) if st.session_state.sort_desc else None)

with col2:
    st.header(f"📊 Результати для: {region}")
    
    # Фільтрація
    df_filtered = df[
        (df['Region'] == region) & 
        (df['Week'] >= weeks_range[0]) & (df['Week'] <= weeks_range[1]) & 
        (df['Year'] >= years_range[0]) & (df['Year'] <= years_range[1])
    ].copy()
    
    # Застосування сортування
    if st.session_state.get('sort_asc'):
        df_filtered = df_filtered.sort_values(by=index_type, ascending=True)
    elif st.session_state.get('sort_desc'):
        df_filtered = df_filtered.sort_values(by=index_type, ascending=False)
        
    tab1, tab2, tab3 = st.tabs(["📋 Таблиця даних", "📈 Графік динаміки", "📊 Порівняння по областях"])
    
    with tab1:
        st.write(f"Відфільтровані дані ({years_range[0]}-{years_range[1]}, тижні {weeks_range[0]}-{weeks_range[1]})")
        st.dataframe(df_filtered, use_container_width=True)
        
    with tab2:
        st.write(f"Динаміка індексу **{index_type}** для області: **{region}**")
        if not df_filtered.empty:
            # Групуємо для графіку (середнє за рік у вибраному діапазоні тижнів)
            df_trend = df_filtered.groupby('Year')[index_type].mean().reset_index()
            
            fig, ax = plt.subplots(figsize=(10, 5))
            sns.lineplot(data=df_trend, x='Year', y=index_type, marker='o', ax=ax, color='b')
            ax.set_title(f"Середній {index_type} за тижні {weeks_range[0]}-{weeks_range[1]}")
            ax.set_ylabel(index_type)
            ax.set_xlabel("Рік")
            ax.grid(True, linestyle='--', alpha=0.7)
            st.pyplot(fig)
        else:
            st.info("Немає даних для відображення.")
            
    with tab3:
        st.write(f"Порівняння **{index_type}** з іншими областями за обраний період")
        
        # Дані для всіх областей за той самий період
        df_all = df[
            (df['Week'] >= weeks_range[0]) & (df['Week'] <= weeks_range[1]) & 
            (df['Year'] >= years_range[0]) & (df['Year'] <= years_range[1])
        ]
        
        df_comp = df_all.groupby('Region')[index_type].mean().reset_index()
        df_comp = df_comp.sort_values(by=index_type, ascending=False)
        
        fig2, ax2 = plt.subplots(figsize=(12, 6))
        
        # Кольорова розмітка: червоний для вибраної області, синій для інших
        colors = ['tomato' if r == region else 'steelblue' for r in df_comp['Region']]
        
        sns.barplot(data=df_comp, x='Region', y=index_type, palette=colors, ax=ax2, hue='Region', legend=False)
        plt.xticks(rotation=90)
        ax2.set_title(f"Середній {index_type} по всіх областях ({years_range[0]}-{years_range[1]})")
        ax2.set_ylabel(f"Середній {index_type}")
        st.pyplot(fig2)