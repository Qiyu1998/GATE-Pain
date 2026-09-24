#!/usr/bin/python3
import scipy.signal as sig
import matplotlib.pyplot as plt
import numpy as np

fs = 300  # The sampling frequency of the digital system
wp = [16/fs*2,18/fs*2]  # Passband edge frequency
ws = [14/fs*2,20/fs*2]  # Stopband edge frequency
gpass = 0.01  # The maximum loss in the passband (dB)
gstop = 15  # The minimum attenuation in the stopband (dB)
analog = False  # Digital filter
ftype = 'butter'  # Butterworth
output = 'sos'  # Second-order sections (‘sos’)

sos = sig.iirdesign(wp,ws,gpass,gstop,analog,ftype,output)

for s in sos:
    print("{",end="")
    n = 0
    for c in s:
        print("%.18e" % c,end="")
        n=n+1
        if n<6:
            print(",",end="")
    print("},")

w, h = sig.sosfreqz(sos)

plt.subplot(2, 1, 1)
db = 20*np.log10(np.maximum(np.abs(h), 1e-5))
w=w/np.pi*fs/2
plt.plot(w, db)
plt.xlim(0,50)
plt.grid(True)
plt.ylabel('Gain [dB]')
plt.title('Frequency Response')
plt.subplot(2, 1, 2)
plt.plot(w, np.angle(h))
plt.xlim(0,50)
plt.grid(True)
plt.yticks([-np.pi, -0.5*np.pi, 0, 0.5*np.pi, np.pi],
           [r'$-\pi$', r'$-\pi/2$', '0', r'$\pi/2$', r'$\pi$'])
plt.ylabel('Phase [rad]')
plt.xlabel('Frequency (Hz)')
plt.show()


