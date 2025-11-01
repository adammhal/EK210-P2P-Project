import matplotlib.pyplot as plt
import numpy as np

def plot_voltage_graph():
    plt.style.use('seaborn-v0_8-darkgrid')
    
    time = [0, 30, 60, 90, 120]
    
    battery1_no_resistor = [9.0, 8.8, 8.6, 8.4, 8.2]
    battery1_with_resistor = [8.8, 8.6, 8.4, 8.2, 8.0]
    
    battery2_no_resistor = [5.0, 4.9, 4.8, 4.7, 4.6]
    battery2_with_resistor = [4.8, 4.7, 4.6, 4.5, 4.4]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7), sharey=False)
    fig.suptitle('Battery Voltage Drop Over Time Under LCD Load', fontsize=20, weight='bold')

    ax1.plot(time, battery1_no_resistor, 'o-', label='Battery 1 (9V) - No Resistor', linewidth=2.5, markersize=8)
    ax1.plot(time, battery1_with_resistor, 's--', label='Battery 1 (9V) - With Resistor', linewidth=2.5, markersize=8)
    ax1.set_title('9V Battery Test', fontsize=16)
    ax1.set_xlabel('Time (seconds)', fontsize=14)
    ax1.set_ylabel('Voltage (V)', fontsize=14)
    ax1.legend(fontsize=12, loc='upper right')
    ax1.grid(True, which='both', linestyle='--', linewidth=0.7)
    ax1.tick_params(axis='both', which='major', labelsize=12)

    ax2.plot(time, battery2_no_resistor, 'o-', label='Battery 2 (5V USB) - No Resistor', color='green', linewidth=2.5, markersize=8)
    ax2.plot(time, battery2_with_resistor, 's--', label='Battery 2 (5V USB) - With Resistor', color='red', linewidth=2.5, markersize=8)
    ax2.set_title('5V USB Source Test', fontsize=16)
    ax2.set_xlabel('Time (seconds)', fontsize=14)
    ax2.set_ylabel('Voltage (V)', fontsize=14)
    ax2.legend(fontsize=12, loc='upper right')
    ax2.grid(True, which='both', linestyle='--', linewidth=0.7)
    ax2.tick_params(axis='both', which='major', labelsize=12)
    
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig('voltage_comparison_graph.png', dpi=300)
    print("Voltage graph saved as 'voltage_comparison_graph.png'")
    plt.close()

def plot_speed_graph():
    plt.style.use('seaborn-v0_8-darkgrid')

    test_message = "hello"
    
    time_slow_test_A = 12540
    time_fast_test_B = 3780

    labels = ['Test A (Slow - LCD in loop)', 'Test B (Fast - LCD outside loop)']
    times = [time_slow_test_A, time_fast_test_B]
    
    colors = ['#E63946', '#457B9D']

    plt.figure(figsize=(10, 7))
    bars = plt.bar(labels, times, color=colors, width=0.5, zorder=3)
    
    plt.title(f'Decoding Speed Comparison for "{test_message}"', fontsize=18, weight='bold')
    plt.ylabel('Total Time (milliseconds)', fontsize=14)
    plt.xticks(fontsize=12)
    plt.yticks(fontsize=12)
    plt.grid(axis='y', linestyle='--', linewidth=0.7, zorder=0)
    
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2.0, height, f'{height} ms', ha='center', va='bottom', fontsize=14, weight='bold')

    plt.tight_layout()
    plt.savefig('speed_comparison_graph.png', dpi=300)
    print("Speed graph saved as 'speed_comparison_graph.png'")
    plt.close()

if __name__ == '__main__':
    print("Generating graphs...")
    
    plot_voltage_graph()
    
    plot_speed_graph()
    
    print("All graphs generated.")
