# -*- coding: utf-8 -*-
#usar python3
import numpy as np
import matplotlib.pyplot as plt
# import sys
# import math

#####################
# Utility Functions #
#####################
def get_vector_lines_float (myfile, line):
    v_str = myfile[line]
    v_str = v_str.strip('\n')
    str_data = str(fl[line + 1])
    v_data = np.fromstring(str_data, dtype=np.float32, sep=' ')
    print(f"vector: {v_str} numpy array size: {v_data.size} first element: {v_data[0]}")
    return v_data

def get_vector_lines_short (myfile, line):
    v_str = myfile[line]
    v_str = v_str.strip('\n')
    str_data = str(fl[line + 1])
    v_data = np.fromstring(str_data, dtype=np.int16, sep=' ')
    print(f"vector: {v_str} numpy array size: {v_data.size} first element: {v_data[0]}")
    return v_data

def get_vector_lines_ushort (myfile, line):
    v_str = myfile[line]
    v_str = v_str.strip('\n')
    str_data = str(fl[line + 1])
    v_data = np.fromstring(str_data, dtype=np.uint16, sep=' ')
    print(f"vector: {v_str} numpy array size: {v_data.size} first element: {v_data[0]}")
    return v_data


################################################
# Open the data file with the vectors to graph #
################################################
file = open ('data.txt', 'r')

###################
# Get the vectors #
###################
#readlines reads the individual line into a list
fl =file.readlines()

dmx_input_ch1 = get_vector_lines_ushort (fl, 0)
dmx_input_ch2 = get_vector_lines_ushort (fl, 2)
ena_ch1 = get_vector_lines_ushort (fl, 4)
pwm_ch1 = get_vector_lines_ushort (fl, 6)
ena_ch2 = get_vector_lines_ushort (fl, 8)
pwm_ch2 = get_vector_lines_ushort (fl, 10)


file.close()
###########################
# Armo la senial temporal #
###########################
t = np.linspace(0, dmx_input_ch1.size, num=dmx_input_ch1.size)
# vmax_power = np.ones(t.size) * max_power

fig, ax = plt.subplots()
ax.set_title('Channel 1 Input & Output')
ax.set_xlabel('Tiempo en milisegundos')
ax.grid()
ax.plot(t, dmx_input_ch1*10, 'b', label='dmx data ch1')
ax.plot(t, pwm_ch1, 'y', label='pwm ch1')
ax.plot(t, ena_ch1, 'g', label='ena ch1')
ax.legend(loc='upper right')
plt.tight_layout()
plt.show()


# fig, ax = plt.subplots()
# ax.set_title('Channel 2 Input & Output')
# ax.set_xlabel('Tiempo en milisegundos')
# ax.grid()
# ax.plot(t, dmx_input_ch2, 'b', label='dmx data ch2')
# ax.plot(t, pwm_ch2, 'y', label='pwm ch2')
# ax.plot(t, ena_ch2, 'g', label='ena ch2')
# ax.legend(loc='upper right')
# plt.tight_layout()
# plt.show()
