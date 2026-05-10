import json
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def generate_charts(json_file):
    with open(json_file, 'r') as f:
        data = json.load(f)

    benchmarks = data['benchmarks']
    
    df_list = []
    for bench in benchmarks:
        name_parts = bench['name'].split('/')
        size = int(name_parts[1]) if len(name_parts) > 1 else 0
        
        df_list.append({
            'Size (Bytes)': size,
            'Latency (ns)': bench['real_time'],
            'Iterations': bench['iterations']
        })

    df = pd.DataFrame(df_list)

    sns.set_theme(style="whitegrid")
    plt.figure(figsize=(10, 6))

    plot = sns.lineplot(data=df, x='Size (Bytes)', y='Latency (ns)', marker='o', linewidth=2.5, color='#2ecc71')
    
    plt.title('Safe-Mem: Allocation Latency vs Block Size', fontsize=16, fontweight='bold')
    plt.xlabel('Block Size (Bytes)', fontsize=12)
    plt.ylabel('Latency (nanoseconds)', fontsize=12)

    plt.axhline(y=8, color='r', linestyle='--', label='Target (8ns)')
    plt.legend()

    plt.savefig('./assets/latency_chart.png', dpi=300, bbox_inches='tight')
    print("✅ Latency chart saved to assets/latency_chart.png")


    # Throughput = 1e9 / Latency (taaki Ops/sec mil jaye)
    df['Throughput (M Ops/s)'] = 1000 / df['Latency (ns)']
    
    plt.figure(figsize=(10, 6))
    sns.barplot(data=df, x='Size (Bytes)', y='Throughput (M Ops/s)', palette='viridis')
    plt.title('Safe-Mem: Throughput Analysis', fontsize=16, fontweight='bold')
    
    plt.savefig('./assets/throughput_chart.png', dpi=300, bbox_inches='tight')
    print("✅ Throughput chart saved to assets/throughput_chart.png")

if __name__ == "__main__":
    generate_charts('result.json')
