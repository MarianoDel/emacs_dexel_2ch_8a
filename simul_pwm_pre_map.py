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

pwm_ena_pre = get_vector_lines_ushort (fl, 0)
pwm_ch_pre = get_vector_lines_ushort (fl, 2)
pwm_ena_post = get_vector_lines_ushort (fl, 4)
pwm_ch_post = get_vector_lines_ushort (fl, 6)


file.close()
###########################
# Armo la senial temporal #
###########################
t = np.linspace(0, pwm_ena.size, num=pwm_ena.size)


fig, ax = plt.subplots()
ax.set_title('DMX to PWM Pre Filtered')
ax.set_xlabel('Dmx Data to Map')
ax.grid()
ax.plot(t, pwm_ena, 'b', label='pwm ena')
ax.plot(t, pwm_ch, 'c', label='pwm channel')
ax.legend(loc='upper right')
plt.tight_layout()
plt.show()


fig, ax = plt.subplots()
ax.set_title('DMX to PWM Post Filtered')
ax.set_xlabel('Dmx Data to Map')
ax.grid()
ax.plot(t, pwm_ena, 'b', label='pwm ena')
ax.plot(t, pwm_ch, 'c', label='pwm channel')
ax.legend(loc='upper right')
plt.tight_layout()
plt.show()

