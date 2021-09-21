#!/usr/bin/python

import random
from itertools import count, islice
from math import sqrt

class indices:
    def __init__(self):
        self.sets = []

    def get_num(self):
        return len(self.sets)

    def discard(self, num_to_discard:int):
        assert(num_to_discard <= self.get_num())
        if num_to_discard == self.get_num():
            self.sets.clear()
        elif num_to_discard > 0:
            for i in range(num_to_discard):
                random_index = random.randint(0, len(self.sets) - 1)
                self.sets.pop(random_index)

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

    def validate(self):
        global_length = -1
        for set in self.sets:
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


class indices_calculator:
    def __init__(self):
        self.wanted_num_elements_per_serie = 0
        self.num_elements_per_serie = 0
        self.wanted_num_series = 0
        self.n = 0
        self.num_series = 0

    def calculate_for_num_series(self, wanted_num_series):
        self.wanted_num_series = wanted_num_series
        # Reverse operation to self.n * self.n + self.n + 1
        n_approx = 0.5 * (sqrt(4.0 * float(self.wanted_num_series) - 3.0) - 1.0)
        self.n = indices_calculator._truncate_to_prime(int(n_approx))
        self.num_elements_per_serie = self.n + 1
        self.num_series = indices_calculator.compute_num_series(self.n)
        self._is_number_different_than_wanted = self.wanted_num_series != self.num_series

    def calculate_for_num_elements_per_serie(self, wanted_num_elements_per_serie:int):
        self.wanted_num_elements_per_serie = wanted_num_elements_per_serie
        self.num_elements_per_serie = indices_calculator._truncate_to_prime(wanted_num_elements_per_serie) + 1
        self._is_number_different_than_wanted = self.num_elements_per_serie != self.wanted_num_elements_per_serie
        self.n = max(self.num_elements_per_serie - 1, 0)
        self.num_series = indices_calculator.compute_num_series(self.n)

    def is_number_different_than_wanted(self):
        return self._is_number_different_than_wanted if self._is_number_different_than_wanted is not None else False

    def compute_num_series(n):
        return n * n + n + 1

    def _is_prime(n):
        return n > 1 and all(n % i for i in islice(count(2), int(sqrt(n) - 1)))

    def _truncate_to_prime(n):
        if n <= 1:
            return n

        while indices_calculator._is_prime(n) is False and n > 1:
            n = n - 1
        return n


class indices_generator:
    def __init__(self, calculator:indices_calculator=None, max_shuffles:int=0) -> None:
        self.calculator = calculator
        self.max_shuffles = max_shuffles

    def set_calculator(self, calculator:indices_calculator):
        self.calculator = calculator

    def set_max_shuffles(self, max_shuffles):
        self.max_shuffles = max_shuffles

    def generate(self) -> indices:

        assert(self.calculator is not None)
        assert(self.calculator.n > 1)
        assert(self.calculator.num_series > 1)

        # https://math.stackexchange.com/questions/1303497/what-is-the-algorithm-to-generate-the-cards-in-the-game-dobble-known-as-spo

        idc = indices()

        first_set = []
        for i in range(self.calculator.n + 1):
            first_set.append(i)
        idc.sets.append(first_set)

        # n following sets
        for i in range(self.calculator.n):
            this_set = []
            this_set.append(0)
            for j in range(self.calculator.n):
                this_set.append(self.calculator.n + 1 + self.calculator.n * i + j)
            idc.sets.append(this_set)

        # n * n following sets
        for i in range(self.calculator.n):
            for j in range(self.calculator.n):
                this_set = []
                this_set.append(i + 1)
                for k in range(self.calculator.n):
                    this_set.append(self.calculator.n + 1 + self.calculator.n * k + ((i * k + j) % self.calculator.n))
                idc.sets.append(this_set)

        # Shuffle indices in each set.
        if self.max_shuffles > 0:
            for this_set in idc.sets:
                num_shuffles = random.randint(1, self.max_shuffles)
                for i in range(num_shuffles):
                    random.shuffle(this_set)

            # Shuffle generated sets.
            num_shuffles = random.randint(1, self.max_shuffles)
            for i in range(num_shuffles):
                random.shuffle(idc.sets)

        assert(len(idc.sets) == self.calculator.num_series)

        return idc
