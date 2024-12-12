import os
import subprocess


def run_test(command):
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result


def Test():
    print("Running lexers tests")
    for file in sorted(os.listdir("tests")):
        print(f"Testing {file}")
        result = run_test(
            f"./tests/backend tests/lexerTests/{file}/code.us tests/lexerTests/{file}/workword"
        )
        if result.returncode == 1:
            print("Test failed")
            print(result.stderr.decode())
            exit(1)
        else:
            print("Test passed")


if __name__ == "__main__":
    Test()
