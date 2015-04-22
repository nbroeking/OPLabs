""" This module contains some helper functions for easily aggregating
    lists for display within the template code.

    Author: Zach Anders
    Date; 04/21/15
    """

def average(*iterables):
    running_sum, running_len = 0, 0
    for lst in iterables:
        if lst:
            running_sum += sum(lst)
            running_len += len(lst)
    if running_len == 0:
        return None 
    return running_sum / running_len

def round_float(value, prec=2):
    if value == None:
        return None
    return ("{0:0." + str(prec) + "f}").format(round(value, prec))
