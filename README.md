# Robotizer

This is my implementation of a VST plug-in that robotizes the input signal. The plugin was created using C++ and JUCE.

The plugin's operation is based on the use of the Hilbert transformation. The transformed signal is used to create two sidebands, which are modulated and then mixed with the input signal.
The ratio of the sideband and input signal amplitudes, as well as the modulation frequencies of the bands, can be specified by the user.

