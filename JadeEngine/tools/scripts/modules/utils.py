#!/usr/bin/env python

import os
import sys
import argparse
from enum import Enum


class enum_enhanced(Enum):
    def __str__(self):
        return self.name.replace("_", " ").capitalize()

    @staticmethod
    def get_names(enum_class):
        return [str(e) for e in enum_class]

    @staticmethod
    def from_str(label, enum_class):
        names = enum_enhanced.get_names(enum_class)
        idx = 0
        for name in names:
            if label in name:
                return enum_class(idx)
            else:
                idx += 1
        raise ValueError("Enum could not be parsed.")


class platform_type(enum_enhanced):
    WINDOWS = 0
    LINUX = 1


class configuration_type(enum_enhanced):
    DEBUG = 0
    PROFILE = 1
    RELEASE = 2


class common_args:
    def __init__(self, platform, configuration, action):
        self.platform = platform
        self.configuration = configuration
        self.action = action


platform = platform_type.LINUX if sys.platform.startswith("linux") else platform_type.WINDOWS if sys.platform.startswith("win32") else None
platform_str = str(platform)


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


def parse_common_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("platform", type=str, default=platform_str, choices=[str(e) for e in platform_type])
    parser.add_argument("configuration", type=str, default=str(configuration_type.DEBUG), choices=[str(e) for e in configuration_type])
    parser.add_argument("action", type=str, default=None)
    args = parser.parse_args()
    return common_args(platform_type.from_str(args.platform, platform_type), configuration_type.from_str(args.configuration, configuration_type), args.action)

