#get the shit
import math

def bandpass(R1,C1,R2,C2):
	R1 = float(R1)
	C1 = float(C1)
	R2 = float(R2)
	C2 = float(C2)
	print "R1 = %.4E\nC1 = %.4E\nR2 = %.4E\nC2 = %.4E\n" % (R1, C1, R2, C2)
	gain = -1* (R2/R1)
	lower = 1/(2*math.pi*R1*C1)
	upper = 1/(2*math.pi*R2*C2)
	print "gain  = %.4E \nlower = %.4E \nupper = %.4E" % (gain,lower,upper)


bandpass(500,10e-9,3.32e4,100e-12)