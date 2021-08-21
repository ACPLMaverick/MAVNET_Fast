#!/usr/bin/python

import random
from itertools import count, islice
from math import sqrt

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
    def __init__(self, num_elements:int=0, max_shuffles:int=0) -> None:
        self.set_num_elements(num_elements)
        self.max_shuffles = max_shuffles

    def set_num_elements(self, num_elements):
        num_elements = indices_generator._truncate_to_prime_plus_one(num_elements)
        self.num_elements = num_elements
        self._n = max(self.num_elements - 1, 0)
        self.num_series = self._n * self._n + self._n + 1

    def set_max_shuffles(self, max_shuffles):
        self.max_shuffles = max_shuffles

    def generate(self) -> indices:

        assert(self._n > 1)
        assert(self.num_series > 1)
        assert(self.max_shuffles > 0)

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

    def _is_prime(n):
        return n > 1 and all(n % i for i in islice(count(2), int(sqrt(n) - 1)))

    def _truncate_to_prime_plus_one(num_elements):
        if num_elements <= 1:
            return num_elements

        n = num_elements - 1
        while indices_generator._is_prime(n) is False and n > 1:
            n = n - 1
        return n + 1
