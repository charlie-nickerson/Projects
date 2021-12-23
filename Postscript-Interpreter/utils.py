"""Parts of this code was adopted from https://composingprograms.com/. 
The code has been changed according to Postscript syntax. 
https://creativecommons.org/licenses/by-sa/3.0/
"""
def comma_separated(xs):
    """Convert each value in the sequence xs to a string, and separate them
    with commas.

    >>> comma_separated(['spam', 5, False])
    'spam, 5, False'
    >>> comma_separated([5])
    '5'
    >>> comma_separated([])
    ''
    """
    return ', '.join([str(x) for x in xs])

