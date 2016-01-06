from display import Display
import alsaaudio as aa
import numpy as np
from struct import unpack
import wave
import audioop

BOARD_SIZE = 32

def normalize(data):
    """ limit the range of an array to be between 0 and BOARD_SIZE """
    norm_matrix = np.int_(np.log10(data)**2)
    norm_matrix = map(lambda x: x if x < BOARD_SIZE else BOARD_SIZE, norm_matrix)
    norm_matrix = map(lambda x: x if x > 0 else 0, norm_matrix)
    return norm_matrix
    

def transform_to_levels(data, chunk, sampling_rate):
    # Convert the data into a numpy array 
    data = np.fromstring(data, dtype= np.int16)
    
    # Grab one channel
    fourier = data[0::2]

    # Fourier Transform
    fourier = np.abs(np.fft.rfft(data)[:-1])

    # make bins
    matrix = np.reshape(fourier,(BOARD_SIZE, chunk/BOARD_SIZE))
    
    # Get the maxs, averages, and mins for the bins
    levels_min = normalize(np.min(matrix, 1))
    levels_avg = normalize(np.average(matrix, 1))
    levels_max = normalize(np.max(matrix, 1))

    # return those maxs, averages, and mins
    return levels_max, levels_avg, levels_min


# Get user input
a = raw_input()
sound = wave.open('./' + a +'.wav', 'r')


# Setup audio output stream
data_out = aa.PCM(aa.PCM_PLAYBACK, aa.PCM_NORMAL)

num_channels = sound.getnchannels()
data_out.setchannels(num_channels)

sampling_rate = sound.getframerate()
data_out.setrate(sampling_rate)

data_out.setformat(aa.PCM_FORMAT_S16_LE)

chunk_size = 512
data_out.setperiodsize(chunk_size)


# Create a display object
d = Display(BOARD_SIZE, BOARD_SIZE)

# Read a chunk's worth of data
data = sound.readframes(chunk_size)

# While music is playing
while(data != ''):
    # play music  
    data_out.write(data)

    # Get the max, min, and average levels
    levels_max, levels_avg, levels_min = transform_to_levels(data, 
                                                             chunk_size, 
                                                             sampling_rate)
    # Clear the board
    d.clear()

    # Redraw the board
    for i in range(BOARD_SIZE):
        d.drawRectangle(i, 0, 1, levels_max[31-i], 0xff0000)
        d.drawRectangle(i, 0, 1, levels_avg[31-i], 0xff00ff)
        d.drawRectangle(i, 0, 1, levels_min[31-i], 0x0000ff)
    
    # Read more data
    data = sound.readframes(chunk_size)
