import os

# API details and base URL
api_key = "Ee-osjmRSwyXkPA3QBFe"
base_url = "https://data.nasdaq.com/api/v3/datatables/FXCM/"

# Define the timeframes and currency pairs
timeframes = ['M1', 'M5', 'M15', 'M30', 'H1', 'H4', 'D1', 'W1', 'MN']
currency_pairs = [
    ('EUR', 'USD'), ('USD', 'JPY'), ('GBP', 'USD'), ('USD', 'CHF'),
    ('USD', 'CAD'), ('AUD', 'USD'), ('NZD', 'USD'), ('EUR', 'GBP'),
    ('EUR', 'AUD'), ('GBP', 'JPY'), ('EUR', 'JPY'), ('CHF', 'JPY'),
    ('AUD', 'NZD')
]

# Define date range
date_gte = "2023-01-01"
date_lte = "2023-12-31"

# Function to run the curl command
def run_curl_command(symbol, timeframe, filename):
    url = f"{base_url}{timeframe}?date.gte={date_gte}&date.lte={date_lte}&symbol={symbol}&api_key={api_key}"
    command = f'curl "{url}" >> {filename}'
    print(f"Executing: {command}")
    os.system(command)

# Iterate through timeframes and currency pairs
for timeframe in timeframes:
    for base, quote in currency_pairs:
        symbol = f"{base}%2F{quote}"
        filename = f"NASDAQ-{base}-{quote}-{date_gte.replace('-', '')}-{date_lte.replace('-', '')}-{timeframe}.json"
        run_curl_command(symbol, timeframe, filename)

print("All requests completed.")
