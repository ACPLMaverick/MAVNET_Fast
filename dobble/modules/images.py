#!/usr/bin/python
# https://pillow.readthedocs.io/en/stable/handbook/tutorial.html

from .utils import enum_enhanced
import os
import random
from PIL import Image, ImageDraw
from math import cos, pi, sin, sqrt, degrees

import PIL


class background_shape(enum_enhanced):
    CIRCLE = 0,
    POLYGON = 1


class constants:
    image_mode = "RGBA"


class vec2:
    def __init__(self, x:int, y:int):
        self.x = x
        self.y = y

    def assign(self, other):
        self.x = other.x
        self.y = other.y

    def compare(self, other):
        return self.x == other.x and self.y == other.y

    def min(self, scalar:float):
        self.x = min(self.x, scalar)
        self.y = min(self.y, scalar)

    def max(self, scalar:float):
        self.x = max(self.x, scalar)
        self.y = max(self.y, scalar)

    def add(self, scalar:float):
        self.x = int(self.x + scalar)
        self.y = int(self.y + scalar)

    def add_vec(self, other):
        self.x = self.x + other.x
        self.y = self.y + other.y

    def mul(self, scalar):
        self.x = int(self.x * scalar)
        self.y = int(self.y * scalar)

    def normalize(self):
        length = sqrt(self.x * self.x + self.y * self.y)
        if length >= 0.0000005:
            self.x = self.x / length
            self.y = self.y / length

    def create_normalized(self):
        other = vec2(0, 0)
        other.assign(self)
        other.normalize()
        return other

    def rotate_by_angle(self, angle_rad):
        # Clockwise rotation.
        old_x = float(self.x)
        old_y = float(self.y)
        angle_sin = sin(angle_rad)
        angle_cos = cos(angle_rad)
        self.x = (old_x * angle_cos + old_y * angle_sin)
        self.y = (-old_x * angle_sin + old_y * angle_cos)

    def truncate(self):
        self.x = int(self.x)
        self.y = int(self.y)

    def __str__(self):
        return "[{}, {}]".format(self.x, self.y)


class canvas_params:
    min_num_cards_on_canvas = 3

    def __init__(self, config_data:dict=None, num_cards_on_canvas:int=5):
        self.num_cards_on_canvas = max(num_cards_on_canvas, canvas_params.min_num_cards_on_canvas)
        if config_data is not None:
            self.outer_ring_bias = config_data["outer_ring_bias"]
            self.inner_disc_bias = config_data["inner_disc_bias"]
            self.scale_multiplier = config_data["scale_multiplier"]
            self.scale_variation = config_data["scale_variation"]
            self.rotation_variation = config_data["rotation_variation"]
            self.position_variation_x = config_data["position_variation_x"]
            self.position_variation_y = config_data["position_variation_y"]
            self.position_multiplier = config_data["position_multiplier"]
            self.background_color = config_data["background_color"]
            self.outer_ring_color = config_data["outer_ring_color"]
            self.inner_disc_color = config_data["inner_disc_color"]
            self.outer_ring_width = config_data["outer_ring_width"]
            self.background_shape_type = background_shape[background_shape.reconvert_name(config_data["enum_background_shape"])]
            num_cards_override = config_data["num_sides_override"]
            if num_cards_override >= canvas_params.min_num_cards_on_canvas:
                self.num_cards_on_canvas = num_cards_override
        else:
            self.outer_ring_bias = 0.0
            self.inner_disc_bias = 0.0
            self.scale_multiplier = 1.0
            self.scale_variation = 0.0
            self.rotation_variation = 0.0
            self.position_variation_x = 0.0
            self.position_variation_y = 0.0
            self.position_multiplier = 1.0
            self.background_color = 0xFFFFFFFF
            self.outer_ring_color = 0xFF000000
            self.inner_disc_color = 0xFF000000
            self.outer_ring_width = 1
            self.background_shape_type = background_shape.CIRCLE


class canvas:
    def __init__(self, params:canvas_params, size:int, ordeal_number:int):
        self.params = params
        self.size = size
        self.ordeal_number = ordeal_number
        self._create_image()

    def distribute_cards(self, cards:list):
        num_images = len(cards)
        if num_images <= 0:
            return

        r_outer = self.size * 0.5 * (1.0 - self.params.outer_ring_bias)
        r_inner = self.size * 0.5 * self.params.inner_disc_bias
        area_outer = pi * r_outer * r_outer
        area_inner = pi * r_inner * r_inner
        work_area = max(area_outer - area_inner, 0.0)
        if work_area <= 0.0:
            return
        
        # Multiply by 2 as a magic number, there will be a possibility to rescale them anyway.
        tile_size = int(sqrt(work_area) / float(num_images)) * 2
        current_position = vec2(0, int((r_outer + r_inner) * 0.5))
        angle_per_image = 2.0 * pi / float(num_images)
        current_angle = 0.0

        # TODO This can be multi-threaded.
        for card in cards:
            card.size = tile_size

            card.position.assign(current_position)
            card.position.mul(self.params.position_multiplier)
            self._create_position_variation(card.position, tile_size)

            # Rotate towards the center of the canvas, so (180 - angle).
            card.rotation = self._create_variation(self.params.rotation_variation, current_angle)
            card.scale = self._create_variation(self.params.scale_variation, self.params.scale_multiplier)

            current_position.rotate_by_angle(angle_per_image)
            current_position.truncate()
            current_angle += angle_per_image

    def apply_cards(self, cards:list):
        for card in cards:
            img = card.create_transformed_card(self)
            canv_half_size = self.size / 2
            upper_left = (int(card.position.x - img.width / 2 + canv_half_size),
                          int(card.position.y - img.height / 2 + canv_half_size))
            # Using the same image as mask allows us to paste with regard to src alpha.
            self.image.paste(img, upper_left, mask=img)

    def save(self, directory:str):
        self.image.save(os.path.join(directory, "card_{}.png".format(self.ordeal_number)))

    def _create_image(self):
        self.image = Image.new(constants.image_mode, (self.size, self.size), 0)
        draw = ImageDraw.Draw(self.image)
        if self.params.background_shape_type is background_shape.CIRCLE:
            draw.ellipse(self._create_draw_bounding_box(1.0 - self.params.outer_ring_bias),
                        outline=self.params.outer_ring_color,
                        fill=self.params.background_color, width=self.params.outer_ring_width)
            if self.params.inner_disc_bias > 0.0:
                draw.ellipse(self._create_draw_bounding_box(self.params.inner_disc_bias), 
                            fill=self.params.inner_disc_color)
        elif self.params.background_shape_type is background_shape.POLYGON:
            rotation = 180.0 / self.params.num_cards_on_canvas
            # TODO Support outline width somehow.
            draw.regular_polygon(self._create_draw_circle(1.0 - self.params.outer_ring_bias),
                                 n_sides=self.params.num_cards_on_canvas,
                                 outline=self.params.outer_ring_color,
                                 fill=self.params.background_color, rotation=rotation)
        else:
            raise ValueError("Unsupported background shape!")
    
    def _create_position_variation(self, position:vec2, tile_size:int):
        magic_multiplier = tile_size / 4
        unit_y = position.create_normalized()
        unit_x = vec2(unit_y.x, unit_y.y)
        unit_x.rotate_by_angle(pi * 0.5)
        unit_x.mul((random.random() * 2.0 - 1.0) * self.params.position_variation_x * magic_multiplier)
        unit_y.mul((random.random() * 2.0 - 1.0) * self.params.position_variation_y * magic_multiplier)
        position.add_vec(unit_y)
        position.add_vec(unit_x)

    def _create_variation(self, variation:float, initial_value:float):
        if variation <= 0.0:
            return initial_value
        variation_coeff = 1.0
        min_value = initial_value - variation_coeff * variation
        max_value = initial_value + variation_coeff * variation
        alpha = random.random()
        return min_value * (1.0 - alpha) + max_value * alpha

    def _create_draw_bounding_box(self, size_mul:float):
        size = int(float(self.size) * size_mul)
        half_size_diff = int((self.size - size) / 2)
        end = half_size_diff + size
        return (half_size_diff, half_size_diff, end, end)

    def _create_draw_circle(self, size_mul:float):
        center_point = self.size / 2
        r = center_point * size_mul
        return (center_point, center_point, r)


# Dims - Initial square size of image (in pixels) that would be later transformed to final position.
# Scale - The final dimensions will be multiplied by this value.
# Rotation - Specified in radians. CENTER of image is a pivot point.
# Position - CENTER of image is a pivot point, so we need to offset by half size when copying.
class card:
    def __init__(self, file_path:str=None):
        if file_path is not None:
            self.img = Image.open(file_path)
            self._set_proper_image_mode()
        else:
            self.img = None

        self.size = 0
        self.scale = 1.0
        self.rotation = 0.0
        self.position = vec2(0, 0)

    def create_copy(self):
        new_image = card()
        new_image.img = self.img
        return new_image

    def create_transformed_card(self, canv:canvas):
        assert(self.img is not None)
        img = self._apply_size_and_scale(self.img)
        img = self._apply_rotation(img)
        return img

    def _apply_size_and_scale(self, img:Image):
        dims_original = vec2(img.width, img.height)
        dims_wanted = vec2(self.size, self.size)
        if dims_original.x != dims_original.y:
            if dims_original.y > dims_original.x:
                scale_factor = dims_original.x / dims_original.y
                dims_wanted.x = max(int(dims_wanted.x * scale_factor), 1)
            else:
                scale_factor = dims_original.y / dims_original.x
                dims_wanted.y = max(int(dims_wanted.y * scale_factor), 1)
        dims_wanted.mul(self.scale)
        dims_wanted.max(1)
        if dims_wanted.compare(dims_original) is False:
            return img.resize((dims_wanted.x, dims_wanted.y), Image.BILINEAR)
        else:
            return img
        
    def _apply_rotation(self, img:Image):
        if self.rotation != 0.0:
            return img.rotate(degrees(self.rotation), resample=Image.BILINEAR, expand=True)
        else:
            return img

    def _set_proper_image_mode(self):
        if self.img.mode != constants.image_mode:
            self.img = self.img.convert(constants.image_mode)
