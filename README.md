# oym8CHWave
Capture EMG signals from GForcePro and play sound according to individual channel signal.

## oym8CHWave
Requests to play note for any channel if EMG signal is in valid region.  
Sends bit masks to arduino.

## oym8CHWave
Receives uint8_t data which indicate channels to play note.  
Plays note for each channel if relevant bit mask set.
