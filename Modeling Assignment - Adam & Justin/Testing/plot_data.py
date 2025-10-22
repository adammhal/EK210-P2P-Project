import matplotlib.pyplot as plt

bit_delay_ms = [100, 50, 20, 10, 5]
baud_rate = [10, 20, 50, 100, 200]
successful_transmissions = [20, 20, 18, 11, 3]
bit_error_rate = [0, 0, 1.25, 5.6, 10.6]

fig, ax1 = plt.subplots(figsize=(8, 5))

color1 = 'tab:blue'
ax1.set_xlabel('Bit Delay (ms)')
ax1.set_ylabel('Successful Transmissions (out of 20)', color=color1)
ax1.plot(bit_delay_ms, successful_transmissions, 'o-', color=color1, label='Successful Transmissions')
ax1.tick_params(axis='y', labelcolor=color1)
ax1.set_xticks(bit_delay_ms)
ax1.set_xticklabels([str(x) for x in bit_delay_ms])

ax2 = ax1.twinx()
color2 = 'tab:red'
ax2.set_ylabel('Bit Error Rate (%)', color=color2)
ax2.plot(bit_delay_ms, bit_error_rate, 's--', color=color2, label='Bit Error Rate')
ax2.tick_params(axis='y', labelcolor=color2)

fig.suptitle('Transmission Success & Bit Error Rate vs. Bit Delay', fontsize=14)
fig.tight_layout(rect=[0, 0, 1, 0.97])

lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()
ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper right')

plt.savefig('plot_data.png')
plt.close()