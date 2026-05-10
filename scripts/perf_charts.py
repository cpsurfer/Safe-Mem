import json
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

def generate_charts(json_file):
    if not os.path.exists(json_file):
        print(f"❌ Error: {json_file} not found!")
        return

    with open(json_file, 'r') as f:
        data = json.load(f)

    benchmarks = data['benchmarks']
    df_list = []

    for bench in benchmarks:
        name = bench['name']

        if any(suffix in name for suffix in ['_mean', '_median', '_stddev', '_cv']):
            continue

        name_parts = name.split('/')
        try:

            if len(name_parts) > 1:
   
                size_str = name_parts[1]
                size = int(size_str) 
                
                df_list.append({
                    'Size (Bytes)': size,
                    'Latency (ns)': bench['real_time'],
                    'Iterations': bench['iterations']
                })
        except (ValueError, IndexError):
            print(f"⚠️ Skipping non-numeric entry: {name}")
            continue

    if not df_list:
        print("❌ No valid data found to plot!")
        return

    df = pd.DataFrame(df_list)

    sns.set_theme(style="whitegrid")
    plt.figure(figsize=(10, 6))

    sns.lineplot(data=df, x='Size (Bytes)', y='Latency (ns)', marker='o', linewidth=2, color='#2ecc71')
    
    plt.title('Safe-Mem Allocation Latency', fontsize=14)
    plt.axhline(y=8.5, color='r', linestyle='--', label='Target (8ns)') 
    plt.legend()

    os.makedirs('assets', exist_ok=True)
    
    plt.savefig('assets/latency_chart.png')
    print("✅ Latency chart saved to assets/latency_chart.png")

if __name__ == "__main__":
    generate_charts('result.json')
