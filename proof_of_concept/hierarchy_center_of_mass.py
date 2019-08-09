# Question, can the center of mass calculation be optimized ?

import numpy as np # we'll use numpy for our 3d vector
import random
import sys

random.seed(42)
np.random.seed(42)

class Node:
    def __init__(self, position, mass, parent=None):
        self.position = np.array(position)
        self.mass = mass
        self.children = []
        self.parent = parent
        if parent:
            parent.add_child(self)

        self.total_mass = mass
        self.center_of_mass = np.zeros(3)

    def add_child(self, child):
        self.children.append(child)
        child.parent = self

    def get_level(self):
        if self.parent:
            return self.parent.get_level() + 1
        else:
            return 0

    def get_com_str(self):
        return "center_of_mass: " + str(self.center_of_mass) + "     total_mass: " + str(self.total_mass)

    def __str__(self):
        pre = ""
        for i in range(self.get_level()): 
            pre +="-"
        pre += "position: " + str(self.position) + "     mass: " + str(self.mass) + "     center_of_mass: " + str(self.center_of_mass) + "     total_mass: " + str(self.total_mass) + "\n"
        for child in self.children:
            pre += str(child)
        return pre

    def calculate_optimized(self):
        for child in self.children:
            child.calculate_optimized()

        self.total_mass = self.mass
        self.center_of_mass = np.zeros(3)
        
        for child in self.children:
            self.total_mass += child.total_mass
            self.center_of_mass += (child.position + child.center_of_mass) * child.total_mass
        
        self.center_of_mass /= self.total_mass


    def calculate_classic(self, relative_pos=None):
        m_list = []
        derived_pos = np.zeros(3)
        if relative_pos is not None:
            derived_pos += relative_pos + self.position
    
        m_list.append([derived_pos, self.mass])
        for child in self.children:
            m_list.extend(child.calculate_classic(derived_pos))

        self.center_of_mass = np.zeros(3)
        self.total_mass = 0
        for pos in m_list:
            self.center_of_mass += pos[0] * pos[1]
            self.total_mass += pos[1]
        self.center_of_mass /= self.total_mass

        return m_list

def generate_random_node(gen_range=15, parent=None, pos_range=3.0, mass_range=[1.0, 10.0]):
    node = Node(position=np.random.uniform(-pos_range, pos_range, 3), mass = random.uniform(mass_range[0], mass_range[1]), parent=parent)
    if random.uniform(0, 1.0) < 0.1:
        for i in range(random.randint(0, gen_range)):
            generate_random_node(gen_range=gen_range, parent=node, pos_range=pos_range, mass_range=mass_range)
    return node


print("======== Running test ========")

root = Node(position=[10.0, 30.0, 40.0], mass = 30.0)

for i in range(5):
    generate_random_node(parent=root)

print(root)




m_pos = root.calculate_classic()
for m in m_pos:
    print(m)
print(root.get_com_str())

root.calculate_optimized()
print(root.get_com_str())



    
