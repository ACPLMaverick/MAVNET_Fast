import sys
import os
from pathlib import Path
from enum import Enum
import music_tag


g_file_extensions = ("*.mp3", "*.ogg", "*.wav", "*.flac")
g_supported_tags = ("title", "artist", "album")
g_conversion_map = {
    "ę": "e",
    "ó": "o",
    "ą": "a",
    "ś": "s",
    "ł": "l",
    "£": "L",
    "ż": "z",
    "ź": "z",
    "ć": "c",
    "ń": "n",
    "": "s",
    "³": "l",
    "ê": "e",
    "¹": "a"
}

class work_result_type(Enum):
    k_failure = 0,
    k_worked = 1,
    k_not_needed = 2

class work_result:
    def __init__(self, text:str, res_type:work_result_type):
        self.value = text
        self.type = res_type

def include_uppercase_in_conversion_map():
    uppercase_map = dict()
    for key, value in g_conversion_map.items():
        key_upper = key.upper()
        value_upper = value.upper()
        if key != key_upper or value != value_upper:
            uppercase_map[key.upper()] = value.upper()
    g_conversion_map.update(uppercase_map)

def get_work_dir() -> str:
    work_dir = sys.argv[1] if len(sys.argv) > 1 else os.path.curdir
    work_dir = os.path.abspath(work_dir)
    return work_dir

def gather_files_and_dirs(work_dir:str) -> list:
    files_and_dirs = []
    work_path = Path(work_dir)

    # First add files.
    for extension in g_file_extensions:
        local_paths = work_path.rglob(extension)
        for local_path in local_paths:
            files_and_dirs.append(str(local_path))

    # Then add all child directories.
    dir_walk = os.walk(work_dir)
    for dir in dir_walk:
        this_dir = dir[0]
        if this_dir == work_dir:
            continue
        files_and_dirs.append(this_dir)

    return files_and_dirs

def convert_chars(text:str) -> work_result:
    result_type = work_result_type.k_not_needed
    new_text = ""
    for char in text:
        new_char = char
        if char in g_conversion_map:
            new_char = g_conversion_map[char]
            result_type = work_result_type.k_worked
        new_text = new_text + new_char
    if result_type == work_result_type.k_worked:
        return work_result(new_text, result_type)
    else:
        return work_result(text, result_type)

def convert_chars_in_path(file_path:str) -> work_result:
    dir_name = os.path.dirname(file_path)
    file_name = os.path.basename(file_path)
    result = convert_chars(file_name)
    return work_result(os.path.join(dir_name, result.value), result.type)

def work_file(path:str) -> work_result_type:
    file = None
    try:
        file = music_tag.load_file(path)
    except:
        return work_result_type.k_failure
    
    is_modification = False
    is_failure = False
    for tag in g_supported_tags:
        if tag not in file:
            continue

        item = file[tag]
        value = item.first
        result = convert_chars(value)
        file[tag] = result.value
        is_modification |= result.type == work_result_type.k_worked
        is_failure |= result.type == work_result_type.k_failure

    # Save file only if any modification was made.
    if is_modification:
        file.save()
    
    # Check if we need to modify path itself to avoid forbidden characters there.
    converted_path_result = convert_chars_in_path(path)
    if converted_path_result.type == work_result_type.k_failure:
        return work_result_type.k_failure
    elif converted_path_result.type == work_result_type.k_worked:
        os.rename(path, converted_path_result.value)
    # else do nothing and return.
    
    if is_failure:
        return work_result_type.k_failure
    elif is_modification:
        return work_result_type.k_worked
    else:
        return work_result_type.k_not_needed

def work_dir(path:str) -> work_result_type:
    converted_path_result = convert_chars_in_path(path)
    if converted_path_result.type == work_result_type.k_worked:
        os.rename(path, converted_path_result.value)
    return converted_path_result.type

def work(path:str) -> work_result_type:
    if os.path.isfile(path):
        return work_file(path)
    elif os.path.isdir(path):
        return work_dir(path)
    else:
        return work_result_type.k_failure

def main():
    work_dir = get_work_dir()
    files = gather_files_and_dirs(work_dir)
    
    total_num_files = len(files)
    if total_num_files <= 0:
        print("No items in this directory. Aborting.")
        return 1

    worked_num_files = 0
    failed_num_files = 0

    # This is done in runtime because I'm lazy.
    include_uppercase_in_conversion_map()

    for file_path in files:
        res = work(file_path)
        if res == work_result_type.k_worked:
            worked_num_files = worked_num_files + 1
        elif res == work_result_type.k_failure:
            failed_num_files = failed_num_files + 1

    print("Processed [{}] items out of [{}] total.".format(worked_num_files, total_num_files))
    if failed_num_files > 0:
        print("WARNING. Processing of [{}] items failed.".format(failed_num_files))
    return 0

if __name__ == "__main__":
    sys.exit(main())