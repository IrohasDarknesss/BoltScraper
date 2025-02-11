# BoltScraper
BoltScraper is a lightning-fast, multithreaded web scraping library built with C++ and pybind11. It enables efficient data collection from multiple websites simultaneously, making it ideal for handling large-scale scraping tasks. 

BoltScraperは、C++とpybind11で構築された超高速のマルチスレッド対応Webスクレイピングライブラリです。
複数のウェブサイトから同時にデータを効率的に収集でき、大規模なスクレイピングタスクに最適です。

## インストール方法

1. リポジトリをクローンするか、ソースコードをダウンロードしてください。
2. `pybind11` と `libcurl` をインストールしてください。
   ```
   pip install pybind11
   sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debianの場合
   ```
3. 以下のコマンドでライブラリをビルドします。
   ```
   python setup.py build_ext --inplace
   ```

## 使い方

### 単一URLの取得
import BoltScraper

content = BoltScraper.fetch_url('https://example.com')
print(content)


### 複数URLの並列取得
import BoltScraper

urls = [
    'https://example.com',
    'https://www.python.org',
    'https://www.github.com'
]

contents = BoltScraper.fetch_urls(urls)

for idx, page in enumerate(contents):
    print(f"ページ {idx + 1} のコンテンツ:\n{page[:200]}\n")


## ライセンス
このプロジェクトはMITライセンスの下で提供されています。