from time import sleep

from bs4 import BeautifulSoup
import polars as pl
import re
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys


def load_csv() -> pl.DataFrame:
    df = pl.read_csv('./data/uline_boxes.csv')
    return df

def load_page() -> str:
    url = 'https://www.uline.com/Product/GuidedNav?t=184360'

    driver = webdriver.Safari()
    driver.get(url)

    height = lambda: driver.execute_script('return document.body.scrollHeight')

    body = driver.find_element(By.TAG_NAME, 'html')
    while True:
        h0 = height()
        body.send_keys(Keys.END)
        sleep(3)

        if height() <= h0:
            break

    element = driver.find_element(By.ID, 'tblChartBody')
    html = f"{element.get_attribute('outerHTML')}"
    
    driver.quit()
    return html

def parse_html(html: str) -> None:
    rows = []
    soup = BeautifulSoup(html, 'html.parser')
    for box in soup.find_all('tr'):
        try:
            box_id = box.find('a').get_text(strip=True)  # type: ignore
            cols = [ col.get_text(strip=True) for col in box.find_all('td')[1:] ]  # type: ignore
            rows.append({
                'id': box_id,
                **{f'col_{i+1}': col for i, col in enumerate(cols)}
            })
        except:
            continue

    df = pl.DataFrame(rows)
    df.write_csv('./data/uline_boxes.csv')

def transform_df(df: pl.DataFrame) -> pl.DataFrame:
    result = \
        df.filter(
            pl.col('col_2') == '200 lb. Test'
        ) \
        .drop('col_2') \
        .with_columns(
            (
                pl.col('col_1')
                .map_elements(
                    lambda s: re.sub(
                        r'(\d+)⁄(\d+)',
                        lambda m: f"{str(round(int(m.group(1)) / int(m.group(2)), 1))[1:]}",
                        s
                    ),
                    return_dtype=pl.String
                )
                .str.replace_all(r'[" ]', '')
                .str.split('x')
                .list.eval(pl.element().str.strip_chars().cast(pl.Float64))
                .list.sort()
                .list.to_struct('max_width', fields=['dim_1', 'dim_2', 'dim_3'])
            ),
            pl.col('col_3').str.replace_all(r'[^0-9.]', '').cast(pl.Float64),
            pl.col('col_4').str.replace_all(r'[^0-9.]', '').cast(pl.Float64),
            pl.col('col_5').str.replace_all(r'[^0-9.]', '').cast(pl.Float64),
            pl.col('col_6').str.replace_all(r'[^0-9.]', '').cast(pl.Float64),
            pl.col('col_7').str.replace_all(r'[^0-9.]', '').cast(pl.Float64)
        ) \
        .unnest('col_1') \
        .drop(['col_8', 'col_9']) \
        .rename({
            'col_3': 'price_25',
            'col_4': 'price_100',
            'col_5': 'price_250',
            'col_6': 'price_500',
            'col_7': 'price_1000'
        })
    return result

def filter_df(df: pl.DataFrame) -> pl.DataFrame:
    result = \
        df.filter(
            (   
                pl.col('dim_3') <= 42
            ),
            (
                (pl.col('dim_1') % 2 == 0) & 
                (pl.col('dim_2') % 2 == 0) & 
                (pl.col('dim_3') % 2 == 0)
            )
        ) \
        .sort('price_25') \
        .unique(['dim_1', 'dim_2', 'dim_3'], keep='first', maintain_order=True)

    # cartesian product, remove boxes where there exists a larger, cheaper box
    cross = result \
        .join(other=result, how='cross') \
        .filter(
            (
                (pl.col('dim_1') > pl.col('dim_1_right')) &
                (pl.col('dim_2') > pl.col('dim_2_right')) &
                (pl.col('dim_3') > pl.col('dim_3_right')) &
                (pl.col('price_25') < pl.col('price_25_right'))
            )
        ) \
        .select(pl.col('id')) \
        .unique()
    
    result = result.join(cross, on=['id'], how='anti')
    return result

def main():
    df = load_csv()
    df = transform_df(df)
    df = filter_df(df)
    df.write_csv('./data/cleaned_data.csv')

    with pl.Config(tbl_cols=-1):
        print(df)

if __name__ == "__main__":
    main()
