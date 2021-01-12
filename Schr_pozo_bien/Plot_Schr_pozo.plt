set ytics nomirror
set y2tics
set xrange [-1.000000:1.000000]
set yrange [-1.739366:1.739366]
set y2range [-0.200200:2.202200]
set xlabel 'x(A)' 
set ylabel 'F(x) A-1' 
set y2label 'V(x) eV' 
plot 'Schr_pozo_FdO.txt' using 1:2 axes x1y1 with lines lt 7 title 'F(x) A-1',\
     'Schr_pozo_FdO.txt' using 1:3 axes x1y1 with lines lt 3 title 'F(x) pozo infinito',\
     'Schr_pozo_FdO.txt' using 1:4 axes x1y2 with lines lt 1 title 'V(x) eV)'