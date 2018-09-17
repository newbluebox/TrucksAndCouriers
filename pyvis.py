import numpy
import matplotlib.pyplot as plt

plt.switch_backend( 'Qt5Agg' )

with open( 'in.txt' ) as file:
    file.readline()
    file.readline()

    file.readline()
    line = file.readline()
    whx = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    why = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    whi = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    whq = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    cx = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    cy = [ int( x ) for x in line.strip().split() ]

    file.readline()
    line = file.readline()
    ci = [ int( x ) for x in line.strip().split() ]

plt.scatter( cx, cy, c = ci )
plt.scatter( whx, why, c = 'red' )
plt.show()
