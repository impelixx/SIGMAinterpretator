import os
import subprocess

def run_test(command):
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result

def lexerTest():
    print("Running lexers tests")
    for file in sorted(os.listdir("tests/lexerTests")):
        print(f"Testing {file}")
        result = run_test(f"./tests/backend tests/lexerTests/{file}/code.us tests/lexerTests/{file}/workword")
        if result.returncode == 1:
            print("Test failed")
            print(result.stderr.decode())
            exit(1)
        else:
            print("Test passed")

def semanticTest():
    print("Running semantic tests")
    for file in reversed(os.listdir("tests/semanticTests")):
        print(f"Testing {file}")
        result = run_test(f"./tests/backend tests/semanticTests/{file}/code.us tests/semanticTests/{file}/workword")
        if result == 2:
            print("Test failed")
            exit(1)
        else:
            print("Test passed")

if __name__ == "__main__":
    lexerTest()
    # semanticTest()