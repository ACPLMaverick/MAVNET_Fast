#!/usr/bin/env python

import os
import json
import yaml
from modules import utils

dir_vscode = ".vscode"
file_name_launch = os.path.join(dir_vscode, "launch.json")
file_name_settings = os.path.join(dir_vscode, "settings.json")
file_name_variants = os.path.join(dir_vscode, "cmake-variants.yaml")
file_name_properties = os.path.join(dir_vscode, "c_cpp_properties.json")


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
    conf = {
        "type": "cppvsdbg" if utils.platform == utils.platform_type.WINDOWS else "cppdbg",
        "request": "launch",
        "name": conf_name.capitalize(),
        "program": "${{workspaceFolder}}/bin/JadeEngine_{}_{}".format(utils.platform_str, conf_name),
        "args": [],
        "cwd": "${workspaceFolder}/bin"
    }

    return conf


# It's for intellisense to work properly, it has no relation to build commands whatsoever.
def create_properties():
    props = {
        "configurations": [
            {
                "name": utils.platform_str,
                "cStandard": "c11",
                "cppStandard": "c++17",
                "compilerPath": "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x64/cl.exe" if utils.platform == utils.platform_type.WINDOWS else "/usr/bin/clang++",
                "intelliSenseMode": "msvc-x64" if utils.platform == utils.platform_type.WINDOWS else "clang-x64",
                "compileCommands": "${workspaceFolder}/build/compile_commands.json"
            }
        ]
    }
    return props


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

    # It's for intellisense to work properly, it has no relation to build commands whatsoever.
    dict_properties = create_properties()
    save_dict_json(dict_properties, file_name_properties)


if __name__ == "__main__":
    main()
