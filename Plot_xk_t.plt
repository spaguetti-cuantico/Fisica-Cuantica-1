set ytics nomirror
set y2label '<k> A-1'
set ylabel '<x> (A)'
set xlabel 't(no pasos)'
set y2tics
plot 'Euler.txt' using 1:7 axes x2y2 with lines,\
     'Euler.txt' using 1:5 axes x1y1  with lines,\
     'Euler.txt' using 1:2 axes x1y1  with lines
while(1) {
	replot
	pause(0.1)
}