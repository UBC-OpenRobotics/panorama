import numpy as np
import matplotlib.pyplot as plt

def noisy_data(start_val, end_val, num_points, noise_standard_deviation):
    time_steps = np.arange(num_points)
    true_signal = np.linspace(start_val, end_val, num_points)

    np.random.seed(42)
    noise = np.random.normal(loc=0.0, scale = noise_standard_deviation, size = num_points)

    noisy_data = true_signal + noise

    return time_steps, true_signal, noisy_data

time, true_signal, noisy_graph = noisy_data(0, 50, 100, 2)

# plt.figure(figsize = (12,6))

# plt.scatter(time, noisy_graph, label = 'Noisy Measurements', alpha = 0.6, s = 15, color = 'gray')

# plt.title('Noisy Data - to be denoised')
# plt.xlabel('Time Step ($k$)')
# plt.ylabel('Value')
# plt.legend()
# plt.grid(True)
# plt.show() 
