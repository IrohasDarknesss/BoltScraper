import BoltScraper

urls = [
    'https://qiita.com/',
    'https://cloud.google.com/',
    'https://openai.com/',
    'https://www.python.org',
    'https://www.github.com'
]

contents = BoltScraper.fetch_urls(urls)

for idx, page in enumerate(contents):
    try:
        print(f"Contents of page {idx + 1}:\n{page[:200].encode('utf-8', errors='ignore').decode('utf-8')}\n")
    except UnicodeDecodeError as e:
        print(f"Decode error on page {idx + 1}: {e}")
