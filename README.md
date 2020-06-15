# oym8CHWave
Captures EMG signals from GForcePro and play sound according to individual channel signal.

## oym8CHWave
Displays waveforms according to EMG data from gForcePro. Sends sound on/off bit flags to oym8CHWaveArduino if oym8CHWaveArduino is connected.

## oym8CHWaveArduino
Receives sound on/off bit flags which indicate channels to play note.  
Plays note for each channel if relevant bit mask set.
