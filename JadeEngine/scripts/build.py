import sys
import os
import shutil


def clean():
    print("Cleaning build and binaries.")
    shutil.rmtree("bin")
    shutil.rmtree("build")
    print("Cleaned.")
    return 0


def callCmake(platform, config):
    return os.system("cmake . -DJE_PLATFORM={0} -DJE_CONFIGURATION={1} -B build/{0}/{1} && ninja -C build/{0}/{1}".format(platform, config))


def main():
    argc = len(sys.argv)
    if argc <= 1:
        return -1

    command = sys.argv[1]
    if command == "build":
        if argc < 4:
            return -1
        else:
            platform = sys.argv[2]
            config = sys.argv[3]
            return callCmake(platform, config)

    elif command == "clean":
        return clean()

    else:
        return -1


if __name__ == "__main__":
    errorCode = main()
    if errorCode != 0:
        print("Error running build script! [{0}]".format(errorCode))
