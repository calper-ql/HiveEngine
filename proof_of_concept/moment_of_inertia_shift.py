# Question, can you shift the moment of inertia ?

# https://en.wikipedia.org/wiki/Parallel_axis_theorem

import numpy as np # we'll use numpy for our 3d vector
import random
import sys

random.seed(42)
np.random.seed(42)

class Node:
    def __init__(self, pos, mass):
        self.pos = np.array(pos)
        self.mass = mass

    def __str__(self):
        return "pos: " + str(self.pos) + "    mass:" + str(self.mass)
    
def calc_com(nodes):
    com = np.zeros(3)
    tm = 0.0
    for node in nodes:
        com += node.pos * node.mass
        tm += node.mass
    return [com, tm]

def calc_moi(nodes):
    moi = np.zeros([3, 3])
    com = calc_com(nodes)
    com = com[0]/com[1]
    for node in nodes:
        pos = node.pos - com
        moi[0][0] += ((pos[1]*pos[1]) + (pos[2]*pos[2])) * node.mass
        moi[1][1] += ((pos[0]*pos[0]) + (pos[2]*pos[2])) * node.mass
        moi[2][2] += ((pos[0]*pos[0]) + (pos[1]*pos[1])) * node.mass

        moi[0][1] += -node.mass * pos[0] * pos[1]
        moi[1][0] += -node.mass * pos[0] * pos[1]

        moi[0][2] += -node.mass * pos[0] * pos[2]
        moi[2][0] += -node.mass * pos[0] * pos[2]

        moi[1][2] += -node.mass * pos[1] * pos[2]
        moi[2][1] += -node.mass * pos[1] * pos[2]
    return moi


nodes = []

for i in range(10):
    nodes.append(Node(np.random.uniform(-1.0, 1.0, 3), 1.0))

#for node in nodes:
#    print(node)

initial_moi = calc_moi(nodes)

shift_vec = np.array([2.0, 0.0, 0.0])

for i in range(4):
    shifted_moi = calc_moi(nodes)
    com = calc_com(nodes)
    #print(initial_moi - shifted_moi)
    #print(com[0]/com[1])
    #print(calc_moi([Node(shift_vec+(com[]), com[1])]))
    print(shifted_moi)

    for node in nodes: node.pos += shift_vec
    

    print()

#print()
#for node in nodes:
#    print(node)


#print("\nInitial moi")
#print(initial_moi)
#print("\nShifted moi")
#print(shifted_moi)

#print("\nShift method result: ")

#shifted_moi_4d = np.identity(4)
#shifted_moi_4d[:3, :3] = shifted_moi

#shift_mat = np.identity(4)
#shift_mat[:3 , 3] = shift_vec

#print(initial_moi/shifted_moi)

#print(shift_mat)
#print(np.dot(shifted_moi_4d, shift_mat))