"""Parts of this code was adopted from https://composingprograms.com/. 
The code has been changed according to Postscript syntax. 
https://creativecommons.org/licenses/by-sa/3.0/
"""
class Buffer(object):
    """A Buffer provides a way of accessing a sequence one at a time.
    Its constructor takes a sequence, called the "source".
    The Buffer supplies elements from source one at a time through its pop_first()
    method. In addition, Buffer provides a current() method to look at the
    next item to be supplied, without moving past it.
    >>> buf = Buffer('{ /sq {dup mul} def')
    >>> buf.pop_first()
    '{'
    >>> buf.pop_first()
    ' '
    >>> buf.current()
    '/'
    >>> buf.pop_first()
    '/'
    >>> buf.current()
    's'
    >>> buf.pop_first()
    's'
    # returns None when reaches end of the sequence. 
    """
    def __init__(self, source):
        self.index = 0
        self.source = source

    def pop_first(self):
        """Remove the next item from self and return it. If self has
        exhausted its source, returns None."""
        current = self.current()
        self.index += 1
        return current

    def current(self):
        """Return the current element, or None if none exists."""
        if self.index >= len(self.source):
            return None
        else:
            return self.source[self.index]

    def expect(self, expected):
        actual = self.pop_first()
        if expected != actual:
            raise SyntaxError("expected '{}' but got '{}'".format(expected, actual))
        else:
            return actual

    def __str__(self):
        return str(self.source[self.index:])

