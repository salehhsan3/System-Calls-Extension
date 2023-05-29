import signal, time, sys, os
import ctypes
import random
libc = ctypes.CDLL(None)
syscall = libc.syscall
ECHILD = 10 #according to linux

def set_weight(weight: int):
    return syscall(334, weight)

def get_weight():
    return syscall(335)

def get_ancestor_sum():
    return syscall(336)

def get_heaviest_descendant():
    return syscall(337)

def set_weight_unit_test():
    random_weight = random.randint(-15,50)
    result = set_weight(random_weight)
    if random_weight >= 0:
        if (result == 0):
            print("SET_WEIGHT: PASSED NON-NEGATIVE WEIGHT TEST!")
        else:
            print("SET_WEIGHT: FAILED NON-NEGATIVE WEIGHT TEST!")
    else: # random_weight < 0
        if (result == -ECHILD):
            print("SET_WEIGHT: PASSED NEGATIVE WEIGHT TEST!")
        else:
            print("SET_WEIGHT: FAILED NEGATIVE WEIGHT TEST!")

def get_weight_unit_test():
    random_weight = random.randint(0,50)
    set_weight(random_weight) # make sure you pass those tests before trying this
    result = get_weight() 
    if (result == random_weight):
        print("GET_WEIGHT: PASSED!")
    else:
        print("GET_WEIGHT: FAILED!")
        
def get_ancestor_sum_unit_test():
    random_weight = random.randint(0,50)
    set_weight(random_weight) # make sure you pass those tests before trying this
    

def main():
    function_dict = {get_weight, set_weight, get_ancestor_sum, get_heaviest_descendant}
    