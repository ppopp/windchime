Windchime
=========

Windchime is a generative music generator which runs on the Arduino Uno. It 
uses a mix of markov models and genetic mutations to choose note pitches, 
durations, velocities, as well as the time between notes.

Installing
----------
You will need an [Arduino device](http://store.arduino.cc/) as well as the 
[Arduino App](http://arduino.cc/en/Main/Software) to build and install the 
software. It has been tested on the Arduino Uno. Open the "windchime.ino" file
in the Arduino App, and install just as you would any other Arduino App.

Running
-------
The wind chime runs automatically as soon as power is supplied. A 
[Pulse Width Modulation](http://en.wikipedia.org/wiki/Pulse-width_modulation)
is supplied on pin 11 of the Arduino Board.  You'll need to connect a speaker
to pin 11 and ground in order to hear anything.

