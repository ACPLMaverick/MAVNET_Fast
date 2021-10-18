#!/usr/bin/python

from enum import Enum
import json
import copy
from json.decoder import JSONDecodeError
import os


class config_category(Enum):
    generator = 0,
    visual = 1,
    viewer = 2


class config:
    default_data = {
        str(config_category.generator): {
            "name": "output",
            "num_cards": 0,
            "is_clear": False,
            "max_shuffles": 3,
            "enum_output_mode": "Png"
        },
        str(config_category.visual): {
            "outer_ring_bias": 0.0,
            "inner_disc_bias": 0.15,
            "rotation_variation": 0.0,
            "scale_multiplier": 1.0,
            "scale_variation": 0.0,
            "position_variation_x": 0.0,
            "position_variation_y": 0.0,
            "position_multiplier": 1.0,
            "background_color": "#FFFFFF",
            "outer_ring_color": "#000000",
            "inner_disc_color": "#000000",
            "outer_ring_width": 2,
            "enum_background_shape": "Polygon",
            "num_sides_override": 0
        },
        str(config_category.viewer): {
            "is_randomize_on_refresh": False
        }
    }

    def __init__(self, file_path:str="config.json"):
        self._data = None
        self._data_immutable = None
        self._file_path = file_path
        self._load()

    def reset(self):
        self._data = copy.copy(self._data_immutable)

    def restore_default(self, new_file_path:str=None):
        if new_file_path is not None:
            self._file_path = new_file_path
        self._data_immutable = copy.copy(config.default_data)
        self.reset()

    def save(self, new_file_path:str=None):
        if new_file_path is not None:
            self._file_path = new_file_path
        self._data_immutable = copy.copy(self._data)
        dirs = os.path.dirname(self._file_path)
        if len(dirs) > 0 and not os.path.isdir(dirs):
            os.makedirs(dirs)
        with open(self._file_path, "w") as f:
            json.dump(self._data_immutable, f, indent=4)

    def get_category_data(self, category:config_category):
        return self._data[str(category)]

    def _load(self):
        is_loaded = False
        if os.path.isfile(self._file_path):
            with open(self._file_path, "r") as f:
                try:
                    self._data_immutable = json.load(f)
                    is_loaded = config._validate_json(self._data_immutable)
                except JSONDecodeError as e:
                    print("Failed reading config: ", e.msg)
                    is_loaded = False
        if not is_loaded:
            print("Will use default config.")
            self._data_immutable = copy.copy(config.default_data)
        self._data = copy.copy(self._data_immutable)

    def _validate_json(obj:dict):
        def check(json_a:dict, json_b:dict):
            num_keys_a = len(json_a.keys())
            num_keys_b = len(json_b.keys())
            if num_keys_a != num_keys_b:
                return False
            else:
                for key_a in json_a.keys():
                    if key_a not in json_b.keys():
                        return False
                    else:
                        value_a = json_a[key_a]
                        value_b = json_b[key_a]
                        if type(value_a) != type(value_b):
                            return False
                        elif type(value_a) is dict:
                            return check(value_a, value_b)
            return True
        json_a = obj
        json_b = config.default_data
        ret_val = check(json_a, json_b)
        if not ret_val:
            print("JSON loaded but failed validation.")
        return ret_val
