#generate the RGB values fo the heatmap
def actual_hex(num):
	r = ""
	s = hex(num)[2:]
	if len(s) < 2:
		r = "0"+s
	else:
		r = s
	return r

def tuple_to_rgb(tup):
	res = "#" + tup[0] + tup[1] + tup[2]
	return res

MAX_RGB_VAL = 255
MIN_RGB_VAL = 0
step_size = 5
steps = MAX_RGB_VAL / step_size
min_to_max = []
max_to_min = []
prev_min = MIN_RGB_VAL
prev_max = MAX_RGB_VAL

for i in range(steps+1):
	min_to_max.append(prev_min)
	max_to_min.append(prev_max)
	prev_min += step_size
	prev_max -= step_size

# red = [255] * (steps+1) #red starts 
# red += max_to_min
red = [0] * (steps+1)
red += [0] * (steps+1)
red += min_to_max
red += [255] * (steps+1)

# blue = min_to_max[:]
# blue += [255] * (steps+1)
blue = [255] * (steps+1)
blue += max_to_min
blue += [0] * (steps+1)
blue += [0] * (steps+1)

# green = [0] * (steps+1)
# green += [0] * (steps+1)
green = min_to_max[:]
green += [255] * (steps+1)
green += [255] * (steps+1)
green += max_to_min

# print zip(red,green,blue)

red_hex = map(actual_hex,red)
green_hex = map(actual_hex,green)
blue_hex = map(actual_hex,blue)

RGBs_raw = zip(red_hex,green_hex,blue_hex)
RGBs = map(tuple_to_rgb,RGBs_raw)

print RGBs
print len(RGBs)