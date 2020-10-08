#!/usr/bin/env python

import os
import json
import yaml
from modules import utils

dir_vscode = ".vscode"
file_name_launch = os.path.join(dir_vscode, "launch.json")
file_name_settings = os.path.join(dir_vscode, "settings.json")
file_name_variants = os.path.join(dir_vscode, "cmake-variants.yaml")


def yaml_noop(self, *args, **kw):
    pass


yaml.emitter.Emitter.process_tag = yaml_noop


def save_dict_yaml(dictionary, file_name):
    with open(file_name, "w") as file:
        yaml.dump(dictionary, file, sort_keys=False)


def save_dict_json(dictionary, file_name):
    with open(file_name, "w") as file:
        json.dump(dictionary, file, indent=4)


def create_launch_configuration(conf_name):
    conf_name = conf_name.capitalize()
    conf = dict()
    conf["type"] = "cppvsdbg" if utils.platform == utils.platform_type.WINDOWS else "cppdbg"
    conf["request"] = "launch"
    conf["name"] = conf_name
    conf["program"] = "${{workspaceFolder}}/bin/JadeEngine_{}_{}".format(utils.platform_str, conf_name)
    conf["args"] = []
    conf["cwd"] = "${workspaceFolder}/bin"
    return conf


class variant_buildType_choices:
    def __init__(self):
        variant_debug_desc = "No optimizations, debug symbols, logging, CPU/GPU markers for profiling."
        variant_profile_desc = "CPU/GPU markers for profiling."
        variant_release_desc = "Full release build."
        self.debug = variant_buildType_choices.create_choice("Debug", variant_debug_desc)
        self.profile = variant_buildType_choices.create_choice("Profile", variant_profile_desc)
        self.release = variant_buildType_choices.create_choice("Release", variant_release_desc)

    def create_choice(choice_name, choice_desc):
        choice_name_cap = choice_name.capitalize()
        choice = {"short": choice_name_cap,
                  "long": choice_desc,
                  "buildType": choice_name_cap,
                  "settings": {"JE_CONFIGURATION": choice_name_cap}}
        return choice


class variant_platform_choices:
    def __init__(self):
        self.windows = variant_platform_choices.create_choice("Windows")
        self.linux = variant_platform_choices.create_choice("Linux")

    def create_choice(choice_name):
        choice_name_cap = choice_name.capitalize()
        choice = {"short": choice_name_cap,
                  "long": choice_name_cap,
                  "settings": {"JE_PLATFORM": choice_name_cap}}
        return choice


def main():
    utils.return_to_project_root()

    if os.path.isdir(dir_vscode) is False:
        os.mkdir(dir_vscode)

    dict_launch = {"version": "0.2.0", "configurations": [create_launch_configuration("Debug"),
                                                          create_launch_configuration("Profile"),
                                                          create_launch_configuration("Release")]}
    save_dict_json(dict_launch, file_name_launch)

    dict_settings = {"cmake.configureOnOpen": True,
                     "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"}
    save_dict_json(dict_settings, file_name_settings)

    dict_variants = {"buildType": {"default": "debug", "choices": variant_buildType_choices()},
                     "platform": {"default": utils.platform_str.lower(), "choices": variant_platform_choices()}}
    save_dict_yaml(dict_variants, file_name_variants)


if __name__ == "__main__":
    main()
