import display

d = display.Display(32, 32)
for i in range(32):
    for j in range(32):
        d.setValue(i,j,0);

d.drawSquare(12,12,4,4,255)

d.setValue(10,12,0xaa00ff)
a = raw_input()

print "This"
d.stop()
