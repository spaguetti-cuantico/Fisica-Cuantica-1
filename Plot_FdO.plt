set ytics nomirror
set autoscale y
set y2range[0:1000]
set y2tics
set y2label 'V(x) (eV)'
set ylabel '|F(x)|^2'
set xlabel 'x(Angstrom)'
plot 'Potencial.dat' axes x2y2 with lines,\
     'FdO_0.dat' axes x1y1 with lines,\
     'FdO.dat' axes x1y1 with lines linetype 7
while(1) {
	replot
	pause(0.1)
}