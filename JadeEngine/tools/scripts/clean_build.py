#!/usr/bin/env python


import shutil
from modules import utils


def main():
    utils.return_to_project_root()

    paths = ("bin", "build")

    for path in paths:
        try:
            shutil.rmtree(path)
        except IOError as e:
            if e.errno != 2:  # Do not print errors in case of non-exisitng files/dirs.
                print("{} : {}".format(e.errno, e.strerror))


if __name__ == "__main__":
    main()
