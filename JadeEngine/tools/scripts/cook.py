#!/usr/bin/env python
import sys
import os
import subprocess
import json
import shutil
from modules import utils
from abc import ABCMeta, abstractmethod
from glob import glob


# Helper classes.
class cooker_paths:
    def __init__(self, path_in, path_out):
        self.path_in = path_in
        self.path_out = path_out


class cooker_action(utils.enum_enhanced):
    COOK = 0
    RECOOK = 1
    CLEAN = 2


# Common utility functions for cooking process.
class cooker_base(metaclass=ABCMeta):
    cooked_extension = ".jcd"
    base_output_dir = os.path.join("bin", "data")
    base_input_dir = "resources"

    def __init__(self, arguments):
        self.common_args = arguments
        self.stat_num_cooked = 0

    def process(self):
        input_dir = self._get_input_base_dir()
        output_dir = self._build_output_base_dir()
        extensions = self._get_file_supported_extensions()
        names = self._get_file_supported_names()
        subfolders, files_in = self._get_files_in_directory(input_dir, names, extensions)
        if files_in is not None and len(files_in) > 0:
            paths_list = self._build_cooker_paths(files_in, input_dir, output_dir)
            for paths in paths_list:
                self._process_file(paths)
        print("Finished {}. Cooked [{}] assets.".format(self.__class__.__name__, self.stat_num_cooked))
    
    @abstractmethod
    def _get_input_base_dir(self):
        pass

    @abstractmethod
    def _get_output_base_dir(self):
        pass

    @abstractmethod
    def _process_file_internal(self, paths):
        pass

    # This method specifies the filter for files.
    # By default there's no filter - all files in input dir are taken into account.
    def _get_file_supported_extensions(self):
        return None

    def _get_file_supported_names(self):
        return None

    # Modify file name for cook purposes. By default, "jcd" extension is added to mark that the file is cooked.
    def _modify_cooked_file_name(self, file_name):
        return file_name + cooker_base.cooked_extension

    def _process_file(self, paths):
        if paths is None or paths.path_in is None or paths.path_out is None:
            return
        if not self._is_file_needs_update(paths):
            return
        if self._process_file_internal(paths):
            self.stat_num_cooked += 1

    def _is_file_needs_update(self, paths):
        if os.path.isfile(paths.path_out) is False:
            return True
        input_mtime = os.path.getmtime(paths.path_in)
        output_mtime = os.path.getmtime(paths.path_out)
        return input_mtime > output_mtime

    def _get_files_in_directory(self, directory, names=None, extensions=None):
        subfolders, files = [], []
        for f in os.scandir(directory):
            if f.is_dir():
                subfolders.append(f.path)
            if f.is_file():
                ext = os.path.splitext(f.name)[1].lower()
                name_only = os.path.splitext(f.name)[0]
                if len(ext) > 1 and ext.startswith("."):
                    ext = ext[1:]
                ext_approve = extensions is None or ext in extensions
                name_approve = names is None or name_only in names
                if ext_approve and name_approve:
                    files.append(f.path)
        for dir in list(subfolders):
            sf, f = self._get_files_in_directory(dir, names, extensions)
            subfolders.extend(sf)
            files.extend(f)
        return subfolders, files

    def _build_cooker_paths(self, files_in, base_in_dir, base_out_dir):
        paths = []
        for file in files_in:
            file_name_with_ext = os.path.basename(file)
            modified_file_name_with_ext = self._modify_cooked_file_name(file_name_with_ext)
            # Subtract common part so we can have directory-like file prefixes to avoid overwriting.
            trailing_part_dir = os.path.dirname(os.path.relpath(file, base_in_dir))
            trailing_part_dir_split = os.path.split(trailing_part_dir)
            modified_file_name_final = ""
            for split_part in trailing_part_dir_split:
                if len(split_part) > 0:
                    modified_file_name_final += split_part + "_"
            modified_file_name_final += modified_file_name_with_ext
            final_in_path = os.path.abspath(file)
            final_out_path = os.path.abspath(os.path.join(base_out_dir, modified_file_name_final))

            # Make sure the output directory exists on drive.
            final_out_dir = os.path.dirname(final_out_path)
            self._create_dirs_if_necessary(final_out_dir)

            path = cooker_paths(final_in_path, final_out_path)
            paths.append(path)
        return paths

    def _build_output_base_dir(self):
        platform_part = "_" + str(self.common_args.platform).lower()
        config_part = "_" + str(self.common_args.configuration).lower()
        return os.path.join(cooker_base.base_output_dir + platform_part + config_part, self._get_output_base_dir())

    def _create_dirs_if_necessary(self, dir):
        if not os.path.isdir(dir):
            os.makedirs(dir)


# Specific cooker action utility (recook, clean) - conditionally purges data.
class action_utility(cooker_base):
    def process(self):
        action = cooker_action.from_str(self.common_args.action, cooker_action)
        output_dir = os.path.abspath(self._build_output_base_dir())
        is_cleanup_only = action is cooker_action.CLEAN
        is_recook = action is cooker_action.RECOOK or is_cleanup_only
        if is_recook:
            print("Purging data directory [{}].".format(output_dir))
            if os.path.exists(output_dir):
                shutil.rmtree(output_dir)
            if is_cleanup_only:
                print("Done. Exiting.")
                return False
        return True

    def _get_input_base_dir(self):
        return ""

    def _get_output_base_dir(self):
        return ""

    def _process_file_internal(self, paths):
        pass


# All functionality for shader compilation goes here.
class shader_compiler(cooker_base):
    cmd_line_d3d12 = "" # TODO
    cmd_line_vulkan = "glslc {} -o {}"
    cmd_line_gles = "" # TODO

    def _get_input_base_dir(self):
        return os.path.join("src", "draw", "shaders")

    def _get_output_base_dir(self):
        return os.path.join("shaders")

    def _process_file_internal(self, paths):
        print("Compiling shader [{}] -> [{}].".format(paths.path_in, paths.path_out))
        cmd_lines = self._build_compilation_command_lines(paths)
        # For now we will support only one compiled shader per platform, will add more when packages are introduced.
        if len(cmd_lines) == 0:
            return False
        for cmd_line in cmd_lines:
            result = subprocess.run(cmd_line, capture_output=True)
            if result.returncode != 0:
                return False
        return True

    def _get_file_supported_extensions(self):
        return ("vert", "geom", "frag", "tesc", "tese", "comp")

    def _build_compilation_command_lines(self, paths):
        cmd_lines = []
        if self.common_args.platform is utils.platform.LINUX:
            # For now we will support only one compiled shader per platform, will add more when packages are introduced.
            cmd_lines.append(self._build_compilation_command_line_vulkan(paths))
        elif self.common_args.platform is utils.platform.WINDOWS:
            cmd_lines.append(self._build_compilation_command_line_d3d12(paths))
        else:
            raise NotImplementedError("Unsupported platform!")
        return cmd_lines

    def _build_compilation_command_line_d3d12(self, paths):
        # TODO
        raise NotImplementedError("Not Implemented.")

    def _build_compilation_command_line_vulkan(self, paths):
        opt_flag = " -O"
        if self.common_args.configuration is utils.configuration_type.DEBUG:
            opt_flag = " -O0"
        cmd_line = shader_compiler.cmd_line_vulkan.format(paths.path_in, paths.path_out) + opt_flag
        return cmd_line.split(" ")

    def _build_compilation_command_line_gles(self, paths):
        # TODO
        raise NotImplementedError("Not Implemented.")


# Simple util to copy icon.
class icon_copier(cooker_base):
    def _get_input_base_dir(self):
        return os.path.join(cooker_base.base_input_dir, "misc")

    def _get_output_base_dir(self):
        return ""

    def _process_file_internal(self, paths):
        # Just copy over the file.
        print("Copying icon", paths.path_in, paths.path_out)
        shutil.copy2(paths.path_in, paths.path_out)
        return True

    def _get_file_supported_extensions(self):
        return ("tga")

    def _get_file_supported_names(self):
        return ("icon")


def main():
    utils.return_to_project_root()
    args = utils.parse_common_args()

    action_util = action_utility(args)
    action_ret_val = action_util.process()
    if action_ret_val is False:
        return 0

    print("Cooking started for [{}] [{}].".format(str(args.platform), str(args.configuration)))

    # Compile shaders (to separate files for now).
    shaders = shader_compiler(args)
    shaders.process()

    # Copy mesh and texture data (for now.)
    # TODO

    # Copy icon to output root.
    icon = icon_copier(args)
    icon.process()

    num_cooked = shaders.stat_num_cooked + icon.stat_num_cooked
    print("Cooking finished successfully. Total assets cooked: [{}]".format(num_cooked))
    return 0


if __name__ == "__main__":
    sys.exit(main())