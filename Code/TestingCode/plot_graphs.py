import matplotlib.pyplot as plt
import numpy as np

def plot_voltage_graph():
    plt.style.use('seaborn-v0_8-darkgrid')
    
    time = [0, 30, 60, 90, 120]
    
    with_resistor_trial_1 = [7.91, 7.89, 7.88, 7.87, 7.86]
    with_resistor_trial_2 = [9.65, 9.31, 9.26, 9.24, 9.20]
    
    with_resistor_avg = np.mean([with_resistor_trial_1, with_resistor_trial_2], axis=0)
    
    without_resistor_trial_1 = [7.91, 7.90, 7.89, 7.89, 7.88] 
    without_resistor_trial_2 = [9.65, 9.62, 9.60, 9.58, 9.56] 
    
    without_resistor_avg = np.mean([without_resistor_trial_1, without_resistor_trial_2], axis=0)

    plt.figure(figsize=(10, 7))
    
    plt.plot(time, with_resistor_avg, 'o-', label='With Resistor (Average)', linewidth=2.5, markersize=8, color='#E63946')
    plt.plot(time, without_resistor_avg, 's--', label='Without Resistor (Average)', linewidth=2.5, markersize=8, color='#457B9D')
    
    plt.title('Battery Voltage Drop Under LCD Load', fontsize=18, weight='bold')
    plt.xlabel('Time (seconds)', fontsize=14)
    plt.ylabel('Voltage (V)', fontsize=14)
    plt.legend(fontsize=12, loc='lower right')
    plt.grid(True, which='both', linestyle='--', linewidth=0.7)
    plt.tick_params(axis='both', which='major', labelsize=12)
    plt.ylim(bottom=min(min(with_resistor_avg), min(without_resistor_avg)) - 0.5) 
    
    plt.tight_layout()
    plt.savefig('voltage_comparison_graph.png', dpi=300)
    print("Voltage graph saved as 'voltage_comparison_graph.png'")
    plt.close()

def plot_speed_graph():
    plt.style.use('seaborn-v0_8-darkgrid')

    test_message = "hello"
    
    lag_slow_test_A = 3576 
    lag_fast_test_B = 615  

    labels = ['Test A (Slow - LCD in loop)', 'Test B (Fast - LCD outside loop)']
    times = [lag_slow_test_A, lag_fast_test_B]
    
    colors = ['#E63946', '#457B9D']

    plt.figure(figsize=(10, 7))
    bars = plt.bar(labels, times, color=colors, width=0.5, zorder=3)
    
    plt.title(f'Receiver Computational Lag for "{test_message}"', fontsize=18, weight='bold')
    plt.ylabel('Total LCD Lag Time (milliseconds)', fontsize=14)
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

