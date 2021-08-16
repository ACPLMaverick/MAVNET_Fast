#!/usr/bin/python

import sys

class indices:
    def __init__(self):
        self.sets = []

    def print(self):
        set_index = 0
        for set in self.sets:
            print(set_index, end=" : [ ")
            for index in set:
                print(index, end=" ")
            set_index += 1
            print("]")


class indices_generator:
    def __init__(self, num_series:int, num_elements:int) -> None:
        self.num_series = num_series
        self.num_elements = num_elements

    def generate(self) -> indices:
        idc = indices()
        for i in range(self.num_series):
            set = []
            for j in range(self.num_elements):
                set.append(j)
            idc.sets.append(set)
        return idc


def main():
    print("Dobble!")

    # Like in real-life dobble set. Configurable later.
    num_series = 55
    num_elements = 8

    generator = indices_generator(num_series, num_elements)
    idc = generator.generate()
    idc.print()

    return 0


if __name__ == "__main__":
    sys.exit(main())