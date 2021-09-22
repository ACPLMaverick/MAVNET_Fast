#!/usr/bin/python
# https://pillow.readthedocs.io/en/stable/handbook/tutorial.html

import os
from PIL import Image
from math import cos, pi, sin, sqrt

import PIL

class vec2:
    def __init__(self, x:int, y:int):
        self.x = x
        self.y = y

    def assign(self, other):
        self.x = other.x
        self.y = other.y

    def compare(self, other):
        return self.x == other.x and self.y == other.y

    def rotate_by_angle(self, angle_rad):
        # Clockwise rotation.
        old_x = float(self.x)
        old_y = float(self.y)
        angle_sin = sin(angle_rad)
        angle_cos = cos(angle_rad)
        self.x = int(old_x * angle_cos + old_y * angle_sin)
        self.y = int(-old_x * angle_sin + old_y * angle_cos)

    def __str__(self):
        return "[{}, {}]".format(self.x, self.y)


class canvas:
    def __init__(self, size:int, outer_ring_bias:float, inner_ring_bias:float,
                 scale_variation:float, rotation_variation:float,
                 background_color:int, ordeal_number:int):
        self.size = size
        self.outer_ring_bias = outer_ring_bias
        self.inner_ring_bias = inner_ring_bias
        self.scale_variation = scale_variation
        self.rotation_variation = rotation_variation
        self.background_color = background_color
        self.ordeal_number = ordeal_number
        self._create_image()

    def distribute_images(self, images):
        num_images = len(images)
        if num_images <= 0:
            return

        r_outer = self.size * 0.5 * (1.0 - self.outer_ring_bias)
        r_inner = self.size * 0.5 * self.inner_ring_bias
        area_outer = pi * r_outer * r_outer
        area_inner = pi * r_inner * r_inner
        work_area = max(area_outer - area_inner, 0.0)
        if work_area <= 0.0:
            return
        
        tile_size = int(sqrt(work_area) / float(num_images))
        current_position = vec2(0, int((r_outer + r_inner) * 0.5))
        angle_per_image = 2.0 * pi / float(num_images)
        current_angle = 0.0

        for image in images:
            image.size = tile_size
            image.position.assign(current_position)
            # Rotate towards the center of the canvas, so (180 - angle).
            image.rotation = self._get_variation(self.rotation_variation, pi / 2.0 - current_angle)
            image.scale = self._get_variation(self.scale_variation, 1.0)

            current_position.rotate_by_angle(angle_per_image)
            current_angle += angle_per_image

    def apply_images(self, images):
        for img in images:
            img.apply_to_canvas(self)

    def save(self, directory):
        self._image.save(os.path.join(directory, "card_{}.png".format(self.ordeal_number)))

    def _create_image(self):
        self._image = Image.new("RGB", (self.size, self.size), self.background_color)
        # TODO Apply circle border and wheel interior as per ring biases.
    
    def _get_variation(self, variation, initial_value):
        # TODO Will apply variations after testing if everything works correctly.
        return initial_value


# Dims - Initial square size of image (in pixels) that would be later transformed to final position.
# Scale - The final dimensions will be multiplied by this value.
# Rotation - CENTER of image is a pivot point.
# Position - CENTER of image is a pivot point, so we need to offset by half size when copying.
class image:
    def __init__(self, file_path:str):
        self.file_path = file_path

        self.size = 0
        self.scale = 1.0
        self.rotation = 0.0
        self.position = vec2(0, 0)

    def apply_to_canvas(self, canv:canvas):
        pass
