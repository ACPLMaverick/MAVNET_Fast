#!/usr/bin/python

import sys
import random

class indices:
    def __init__(self):
        self.sets = []

    def print(self):
        set_index = 0
        global_length = -1
        for set in self.sets:
            print(set_index, end=" : [ ")
            for index in set:
                print(index, end=" ")
            set_index += 1
            print("]")
            if global_length == -1:
                global_length = len(set)
                assert(global_length > 0)
            self._validate(set, global_length)

    def _validate(self, this_set, global_length):
        assert(len(this_set) == global_length)
        for other_set in self.sets:
            if this_set == other_set:
                continue
            num_matches = 0
            for other_element in other_set:
                for this_element in this_set:
                    if other_element == this_element:
                        num_matches = num_matches + 1
            assert(num_matches == 1)


class indices_generator:
    def __init__(self, num_elements:int, max_shuffles:int) -> None:
        self.num_elements = num_elements
        self._n = self.num_elements - 1
        # TODO assert is prime number.
        self.num_series = self._n * self._n + self._n + 1
        self.max_shuffles = max_shuffles

    def generate(self) -> indices:

        # https://math.stackexchange.com/questions/1303497/what-is-the-algorithm-to-generate-the-cards-in-the-game-dobble-known-as-spo

        idc = indices()

        first_set = []
        for i in range(self._n + 1):
            first_set.append(i)
        idc.sets.append(first_set)

        # n following sets
        for i in range(self._n):
            this_set = []
            this_set.append(0)
            for j in range(self._n):
                this_set.append(self._n + 1 + self._n * i + j)
            idc.sets.append(this_set)

        # n * n following sets
        for i in range(self._n):
            for j in range(self._n):
                this_set = []
                this_set.append(i + 1)
                for k in range(self._n):
                    this_set.append(self._n + 1 + self._n * k + ((i * k + j) % self._n))
                idc.sets.append(this_set)

        # Shuffle indices in each set.
        for this_set in idc.sets:
            num_shuffles = random.randint(1, self.max_shuffles)
            for i in range(num_shuffles):
                random.shuffle(this_set)

        # Shuffle generated sets.
        num_shuffles = random.randint(1, self.max_shuffles)
        for i in range(num_shuffles):
            random.shuffle(idc.sets)

        assert(len(idc.sets) == self.num_series)

        return idc


def main():
    print("Dobble!")

    # Like in real-life dobble set. Configurable later.
    num_elements_per_serie = 8
    max_shuffles = 8

    generator = indices_generator(num_elements_per_serie, max_shuffles)
    idc = generator.generate()
    idc.print()

    return 0


if __name__ == "__main__":
    sys.exit(main())