from display import Display
import alsaaudio as aa
import numpy as np
from struct import unpack
import wave
import audioop

sampling_rate = 24000
num_channels = 2
chunk_size = 1024
data_out = aa.PCM(aa.PCM_PLAYBACK, aa.PCM_NORMAL)
data_out.setchannels(num_channels)
data_out.setrate(sampling_rate)
data_out.setformat(aa.PCM_FORMAT_U8)
data_out.setperiodsize(chunk_size)

d = Display(32, 32)
sound = wave.open('./cena.wav', 'r')

def transform_to_levels(data, chunk, sampling_rate):
    data = np.fromstring(data, dtype= np.int8)
    fourier = data[0::2]
    fourier = np.fft.rfft(data)[:-1]
    fourier = (np.abs(fourier))

    data = np.reshape(fourier,(32, chunk/32))
    
    power_min = (np.log10(np.min(data,1))**2)
    matrix_min = np.int_(power_min)
    matrix_min = map(lambda x: x if x < 32 else 32, matrix_min)
    matrix_min = map(lambda x: x if x > 0 else 0, matrix_min)

    power_avg = (np.log10(np.average(data,1))**2)
    matrix_avg = np.int_(power_avg)
    matrix_avg = map(lambda x: x if x < 32 else 32, matrix_avg)
    matrix_avg = map(lambda x: x if x > 0 else 0, matrix_avg)

    power_max = (np.log10(np.max(data,1))**2)
    matrix_max = np.int_(power_max)
    matrix_max = map(lambda x: x if x < 32 else 32, matrix_max)
    matrix_max = map(lambda x: x if x > 0 else 0, matrix_max)
    
    return matrix_max, matrix_avg,  matrix_min

data = sound.readframes(chunk_size)
while(data!=''):
    try:
        data_out.write(data) 
        levels_max, levels_avg, levels_min = transform_to_levels(data, 
                                                                 chunk_size, 
                                                                 sampling_rate)
        d.clear()
        for i in range(32):
            d.drawSquare(i,0,1, levels_max[31-i], 0xff0000)
            d.drawSquare(i,0,1, levels_avg[31-i], 0xff00ff)
            d.drawSquare(i,0,1, levels_min[31-i], 0x0000ff)
        data = sound.readframes(chunk_size)
    
    except Exception :
        d.clear()
    


