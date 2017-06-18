o = 100

class Ang:
    _a = 10
    def GetA(self):
        print("Call Ang Method GetA()")
        return self._a

    def SetA(self, a):
        print("Call Ang Method SetA()")
        self._a = a
        return

def Case1():
    print("Call Case1")
    return

def Case2():
    print("Call Case2")
    return 1

def Case3():
    print("Call Case3")
    return 1, "Test"

def Add(a, b):
    print("Call Add")
    return a + b