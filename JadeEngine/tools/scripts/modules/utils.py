#!/usr/bin/env python

import os
import sys
from enum import Enum


class platform_type(Enum):
    UNKNOWN = 0
    WINDOWS = 1
    LINUX = 2

    def __str__(self):
        return self.name.capitalize()


platform = platform_type.LINUX if sys.platform.startswith("linux") else platform_type.WINDOWS if sys.platform.startswith("win32") else platform_type.UNKNOWN
platform_str = str(platform) if platform is not platform_type.UNKNOWN else None


def return_to_project_root():
    files_to_contain = ("CMakeLists.txt")
    max_num_returns = 32
    num_returns = 0
    while num_returns < max_num_returns:
        for subdir, dirs, files in os.walk("."):
            for file in files:
                if file in files_to_contain:
                    return
        os.chdir("..")
        num_returns += 1
    assert(False, "Could not find the project root.")
