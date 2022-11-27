Conversion of audio files (for 64-bit Ubuntu 18.04.5 LTS):
```
# Install ffmpeg:
sudo apt update
sudo apt install ffmpeg

# Example of converting an .mp3 file to a .wav file so it can be used with loadSoundFile():
ffmpeg -i <input>.mp3 -f s16le -acodec pcm_s16le -ac 1 -ar 8192 <output>.wav
```