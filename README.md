# Ray Tracer

Creates PNG file in ./results/

Usage:<br>
[--help] - show this help message<br>
[--sizes] <width in pixels> <height in pixels> - set target picture sizes(800x600 by default)<br>
[--import] <path> - import scene using camera from the first entry (.rt and ASCII .stl are supported)<br>
[--threads] <number> - set threads number (auto by default)<br>
[--nolight] - disable all illumination, and consequently any other optical effects (enabled by default)<br>
[--norefl] - disable reflection (enabled by default)<br>
[--norefr] - disable refraction (enabled by default)<br>
[--complexlight] - enable complex illumination<br>
[--photons] <number> - set photons number (5000000 photons by default)<br>
[--adaptiveaa] - enable adaptive anti aliasing (x16 by default)<br>
[--aascales] <width pixels> <height pixels> - set pixel subdivision(x16 (by default) means 4 * 4)<br>
[--hlaliasing] - highlight aliasing with red color<br>
