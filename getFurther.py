import matplotlib.pyplot as plt
import numpy as np
import wave

data = wave.open("music.wav", "r")

frames = data.readframes(512)
while frames != '':
    fdat = np.fromstring(frames, dtype=np.int16)
    freqs = np.fft.rfft(fdat)
    x = np.arrange(512)/512 
    plt.plot(x, fdat)
    plt.plot(freqs)
    input()
