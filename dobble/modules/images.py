#!/usr/bin/python
# https://pillow.readthedocs.io/en/stable/handbook/tutorial.html

import os
import random
from PIL import Image, ImageDraw
from math import cos, pi, sin, sqrt, degrees

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

    def mul(self, scalar):
        self.x = int(self.x * scalar)
        self.y = int(self.y * scalar)

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


class canvas_params:
    def __init__(self):
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


class canvas:
    def __init__(self, params:canvas_params, size:int, ordeal_number:int):
        self.params = params
        self.size = size
        self.ordeal_number = ordeal_number
        self._create_image()

    def distribute_images(self, images):
        num_images = len(images)
        if num_images <= 0:
            return

        r_outer = self.size * 0.5 * (1.0 - self.params.outer_ring_bias)
        r_inner = self.size * 0.5 * self.params.inner_disc_bias
        area_outer = pi * r_outer * r_outer
        area_inner = pi * r_inner * r_inner
        work_area = max(area_outer - area_inner, 0.0)
        if work_area <= 0.0:
            return
        
        tile_size = int(sqrt(work_area) / float(num_images)) * 2
        current_position = vec2(0, int((r_outer + r_inner) * 0.5))
        angle_per_image = 2.0 * pi / float(num_images)
        current_angle = 0.0

        for image in images:
            image.size = tile_size

            image.position.assign(current_position)
            image.position.x = self._create_variation(self.params.position_variation_x, image.position.x)
            image.position.y = self._create_variation(self.params.position_variation_y, image.position.y)
            image.position.mul(self.params.position_multiplier)

            # Rotate towards the center of the canvas, so (180 - angle).
            image.rotation = self._create_variation(self.params.rotation_variation, pi / 2.0 - current_angle)
            image.scale = self._create_variation(self.params.scale_variation, self.params.scale_multiplier)

            current_position.rotate_by_angle(angle_per_image)
            current_angle += angle_per_image

    def apply_images(self, images):
        for mimg in images:
            img = mimg.create_transformed_image(self)
            canv_half_size = self.size / 2
            upper_left = (int(mimg.position.x - img.width / 2 + canv_half_size),
                          int(mimg.position.y - img.height / 2 + canv_half_size))
            # Using the same image as mask allows us to paste with regard to src alpha.
            self._image.paste(img, upper_left, mask=img)

    def save(self, directory):
        self._image.save(os.path.join(directory, "card_{}.png".format(self.ordeal_number)))

    def _create_image(self):
        self._image = Image.new("RGBA", (self.size, self.size), 0)
        draw = ImageDraw.Draw(self._image)
        draw.ellipse(self._create_draw_bounding_box(1.0 - self.params.outer_ring_bias),
                     outline=self.params.outer_ring_color,
                     fill=self.params.background_color, width=self.params.outer_ring_width)
        if self.params.inner_disc_bias > 0.0:
            draw.ellipse(self._create_draw_bounding_box(self.params.inner_disc_bias), 
                         fill=self.params.inner_disc_color)
    
    def _create_variation(self, variation, initial_value):
        if variation <= 0.0:
            return initial_value
        min_value = initial_value - initial_value * variation
        max_value = initial_value + initial_value * variation
        alpha = random.random()
        return min_value * (1.0 - alpha) + max_value * alpha

    def _create_draw_bounding_box(self, size_mul):
        size = int(float(self.size) * size_mul)
        half_size_diff = int((self.size - size) / 2)
        end = half_size_diff + size
        return (half_size_diff, half_size_diff, end, end)


# Dims - Initial square size of image (in pixels) that would be later transformed to final position.
# Scale - The final dimensions will be multiplied by this value.
# Rotation - Specified in radians. CENTER of image is a pivot point.
# Position - CENTER of image is a pivot point, so we need to offset by half size when copying.
class image:
    def __init__(self, file_path:str):
        self.file_path = file_path

        self.size = 0
        self.scale = 1.0
        self.rotation = 0.0
        self.position = vec2(0, 0)

    def create_transformed_image(self, canv:canvas):
        img = Image.open(self.file_path)
        img = self._apply_size_and_scale(img)
        img = self._apply_rotation(img)
        return img

    def _apply_size_and_scale(self, img:Image):
        dims_original = vec2(img.width, img.height)
        dims_wanted = vec2(self.size, self.size)
        if dims_original.x != dims_original.y:
            if dims_original.y > dims_original.x:
                scale_factor = dims_original.x / dims_original.y
                dims_wanted.x = int(dims_wanted.x * scale_factor)
            else:
                scale_factor = dims_original.y / dims_original.x
                dims_wanted.y = int(dims_wanted.y * scale_factor)
        dims_wanted.mul(self.scale)
        if dims_wanted.compare(dims_original) is False:
            return img.resize((dims_wanted.x, dims_wanted.y), Image.BILINEAR)
        else:
            return img
        
    def _apply_rotation(self, img:Image):
        if self.rotation != 0.0:
            return img.rotate(degrees(self.rotation), resample=Image.BILINEAR, expand=True)
        else:
            return img
