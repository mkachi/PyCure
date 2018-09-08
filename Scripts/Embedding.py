sample = 100

class SampleClass:
	_a = 99
	
	def getA(self):
		print("Call SampleClass getA()")
		return self._a;

	def setA(self, a):
		print("Call SampleClass setA()")
		self._a = a;
		return

	def getMultiValue(self):
		print("Call SampleClass getMultiValue() has return value 2");
		return 1, 2

def function1():
	print("Call function1()")
	return

def function2():
	print("Call function2() has return value 1")
	return 1

def function3():
	print("Call function3() has return value 2")
	return 1, "Test"