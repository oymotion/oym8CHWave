# oym8CHWave Project

Captures EMG signals from gForcePro and play sound according to channel signals.

## oym8CHWave

Displays waveforms according to EMG data from gForcePro. Sends sound on/off bit flags to oym8CHWaveArduino if oym8CHWaveArduino is connected.

## oym8CHWaveArduino

Receives sound on/off bit flags which indicate channels to play note.  
Plays note for each channel if relevant bit mask set.

## Tools

|Script Name| Description|
|:-|:-|
|bin2csv.py|Python script to convert binary data into CSV format|
|emg_data_plot.py|Python script to plot EMG data|
|emg_data_plot.m|Matlab code to plot EMG data|
