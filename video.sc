ffmpeg -framerate 25 -i 0%03d.jpg -c:v libx264 -r 30 -pix_fmt yuv420p out.mp4
