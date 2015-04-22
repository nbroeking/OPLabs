""" This module contains some template helper functions for formatting
    output with specific units, suffixes, or other common tasks.

    Author: Zach Anders
    Date; 04/21/15
    """

def __mkunit__(div):
    def unit(value, places=0):
        if value == None:
            return None
        if places == 0:
            return int(value) / int(div)
        else:
            return round(float(value) / div, places)
    return unit
        
KILOBYTES = __mkunit__(2**10)
MEGABYTES = __mkunit__(2**20)
GIGABYTES = __mkunit__(2**30)

def BITS(value):
    if value == None:
        return None
    return value * 8

def PERCENT(value):
    if value == None:
        return None
    value = round(float(value)*100, 2)
    if value > 100.0:
        value = 100.0
    return str("%.02f" % value) + "%"
