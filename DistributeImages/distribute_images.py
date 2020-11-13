#!/usr/bin/python

import math
import glob
from enum import Enum
import os
import pptx
import pptx.util
import pptx.dml.color
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.colorchooser
from PIL import ImageTk, Image
from tkinter import ttk


# Known bugs:
# - Spinboxes don't allow changing the most-significant digit of a number
# - Hang when loading images
# - Reload presentation state after slide creation.

# Add possibility to drag-drop files.
# Add option for overwriting or not output files.
# Make program remember last used paths

# ###### LOGIC

class enum_enhanced(Enum):
    def __str__(self):
        return self.name.replace("_", " ").capitalize()

    def get_names(enum_class):
        return [str(e) for e in enum_class]

    def reconvert_name(name):
        return name.upper().replace(" ", "_")


class word_orientation(enum_enhanced):
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_RIGHT = 2,
    BOTTOM_LEFT = 3


class mode_distribution(enum_enhanced):
    ALL_IN_ONE_SLIDE = 0,
    MULTIPLE_SLIDES = 1


class mode_work_slide(enum_enhanced):
    NEW_SLIDE = 0,
    SOURCE_SLIDE = 1


class input_data_load_result(Enum):
    FAILURE = 0,
    OK = 1,
    OK_NON_UNIFORM_DIMENSIONS = 2


class dimensions(tkinter.StringVar):
    def __init__(self, x, y):
        self.x = tkinter.IntVar(value=x)
        self.x.trace("w", lambda var, index, mode: self.set(self._get_str()))
        self.y = tkinter.IntVar(value=y)
        self.y.trace("w", lambda var, index, mode: self.set(self._get_str()))
        super().__init__(value=self._get_str())

    def _get_str(self):
        return "{} x {}".format(self.x.get(), self.y.get())

    def set_dims(self, new_x, new_y):
        self.x.set(new_x)
        self.y.set(new_y)


class enum_var(tkinter.StringVar):
    def __init__(self, enum_class, *args, **kwargs):
        self.enum_class = enum_class
        super().__init__(*args, **kwargs)

    def get_enum(self):
        return self.enum_class[self.enum_class.reconvert_name(self.get())]


class color_picker(tkinter.Frame):
    def __init__(self, master, var_color, bg_color, width=144, height=20, command=None):
        super().__init__(master, width=width, height=height, bg=bg_color)
        width_btn = int(0.8 * width)
        width_cb = width - width_btn
        self._var_check = tkinter.IntVar(value=0)
        self.cb = tkinter.Checkbutton(self, variable=self._var_check, width=width_cb, height=height,
                                      bg=bg_color, highlightcolor=bg_color, activebackground=bg_color)
        self.btn = tkinter.Button(self, command=self._on_clicked, bg=var_color.get(), fg=var_color.get())
        self.cb.place(x=0, y=0, width=width_cb, height=height)
        self.btn.place(x=width_cb, y=0, width=width_btn, height=height)
        self.var_color = var_color
        self.var_color.trace("w", lambda name, index, mode, self=self: self._on_color_changed())
        self._var_check.trace("w", lambda name, index, mode, self=self: self._on_checkbox_changed())
        self.command = command
        self._var_color_init_val = self.var_color.get()

    def is_enabled(self):
        return self._var_check.get() == 1

    def _on_clicked(self):
        new_color_tuple = tkinter.colorchooser.askcolor(self.var_color.get(), title="Pick a color")
        if len(new_color_tuple) > 1:
            new_color = new_color_tuple[1]
            if new_color is not None and new_color != self.var_color.get():
                self.var_color.set(new_color)
                if self.command is not None:
                    self.command(new_color)
                if self._var_check.get() == 0:
                    self._var_check.set(1)

    def _on_color_changed(self):
        self.btn["bg"] = self.var_color.get()
        self.btn["fg"] = self.var_color.get()

    def _on_checkbox_changed(self):
        if self.is_enabled() is False:
            # Reset to default.
            self.var_color.set(self._var_color_init_val)


class distribution_result:
    def __init__(self, grid_size_x, grid_size_y, scaled_dims_x, scaled_dims_y):
        self.grid_size_x = grid_size_x
        self.grid_size_y = grid_size_y
        self.scaled_dims_x = scaled_dims_x
        self.scaled_dims_y = scaled_dims_y


# slide_image's anchor is NW
class slide_image:
    def __init__(self, file_path):
        self._file_path = file_path
        self._img_ori = Image.open(file_path)
        self._img_scaled_init = None
        self._img = None
        self._canvas_id = 0
        self.original_width = self._img_ori.width
        self.original_height = self._img_ori.height
        self.scaled_width = 0
        self.scaled_height = 0
        self.x = 0
        self.y = 0
        self.name = "(no name)"  # TODO

    def distribute(images, screen_width, screen_height, image_width, image_height, padding_width, padding_height):
        # So at this point we have both presentation loaded, slide dimensions available (in points)
        # And also images loaded with their dimensions (in pixels).
        # Scaled image dimensions will be in POINTS, so they can be easily integrated in pptx.

        # ratio_screen = screen_width / screen_height
        # ratio_image = image_width / image_height

        # ratio_screen_to_image = ratio_screen / ratio_image

        # If ratio_screen_to_image > 1 there should be more images in width and less in height.
        # If ratio_screen_to_image < 1 there should be less images in width and more in height.

        # For now let's try to create an uniform grid, we'll worry about text later.

        num_images = len(images)
        if num_images == 0:
            return False

        num_rows = 1
        scale_factor = 1.0
        while(True):
            # Scale one image to fit exactly num_rows
            scale_factor = ((screen_height - (num_rows + 1) * padding_height) / image_height) / num_rows
            image_scaled_width = image_width * scale_factor
            # Check if all images will fit in this one row. If not, we'll try to fit them in two rows.
            total_images_width = (image_scaled_width + padding_width) * (num_images // num_rows)
            if total_images_width > screen_width:
                num_rows += 1
            else:
                break

        image_scaled_width = math.floor(image_width * scale_factor)
        image_scaled_height = math.floor(image_height * scale_factor)

        num_cols = math.ceil(num_images / num_rows)

        # print(scale_factor, num_rows, num_cols, image_scaled_width, image_scaled_height)

        # Now distribute the images
        for row in range(num_rows):
            for col in range(num_cols):
                image_idx = row * num_cols + col
                if image_idx >= num_images:
                    break
                img = images[image_idx]
                img.scaled_width = image_scaled_width
                img.scaled_height = image_scaled_height
                img.x = padding_width + col * (image_scaled_width + padding_width)
                img.y = padding_height + row * (image_scaled_height + padding_height)
                # print(image_idx, img.x, img.y, img.scaled_width, img.scaled_height)
                if not (img.x >= 0 and (img.x + img.scaled_width) < screen_width):
                    print("Image distribution exceeds X bounds:", screen_width, img.x, img.scaled_width)
                if not (img.y >= 0 and (img.y + img.scaled_height) < screen_height):
                    print("Image distribution exceeds Y bounds:", screen_height, img.y, img.scaled_height)

        return distribution_result(num_cols, num_rows, image_scaled_width, image_scaled_height)

    def _resize_img(self, canvas, new_width, new_height):
        img = None
        if self._img_scaled_init is None:
            # This is an optimization to prevent re-scaling the original big image on any parameter change.
            self._img_scaled_init = self._img_ori.resize((new_width, new_height), Image.ANTIALIAS)
            img = self._img_scaled_init
        else:
            img = self._img_scaled_init.resize((new_width, new_height), Image.ANTIALIAS)
        self._img = ImageTk.PhotoImage(img)
        canvas.itemconfig(self._canvas_id, image=self._img)

    def add_to_slide(self, slide):
        slide.shapes.add_picture(self._file_path,
                                 pptx.util.Pt(self.x),
                                 pptx.util.Pt(self.y),
                                 width=pptx.util.Pt(self.scaled_width),
                                 height=pptx.util.Pt(self.scaled_height))

    def init_draw(self, canvas, reference_width, reference_height):
        self._canvas_id = canvas.create_image(0, 0, image=self._img, anchor=tkinter.NW)
        self.draw(canvas, reference_width, reference_height)

    def draw(self, canvas, reference_width, reference_height):
        scale_x = canvas.winfo_width() / reference_width
        scale_y = canvas.winfo_height() / reference_height

        canv_x = int(self.x * scale_x)
        canv_y = int(self.y * scale_y)
        canv_w = int(self.scaled_width * scale_x)
        canv_h = int(self.scaled_height * scale_y)
        canvas.coords(self._canvas_id, canv_x, canv_y)

        self._resize_img(canvas, canv_w, canv_h)


class pdf_jpg_extractor:
    def __init__(self, file):
        self.file = file
        # TODO

    def extract(idx_start, idx_end):
        # TODO
        pass


class distributor:
    def __init__(self):
        self.pres = None
        self.input_dir = None
        self.canvas = None

        self.slide_names = ["(no slides)"]

        self.num_images = tkinter.IntVar(value=0)
        self.image_grid_dims = dimensions(0, 0)
        self.image_dims = dimensions(0, 0)
        self.image_scaled_dims = dimensions(0, 0)

        self.source_slide_name = tkinter.StringVar(value=self.slide_names[0])
        self.source_slide_name.trace("w", lambda name, index, mode, self=self: self._on_source_slide_name_changed())
        self.word_font = tkinter.StringVar(value="Impact")
        self.word_size = tkinter.IntVar(value=24)
        self.word_position = enum_var(enum_class=word_orientation, value=word_orientation.TOP_LEFT)
        self.operation = enum_var(enum_class=mode_work_slide, value=mode_work_slide.NEW_SLIDE)
        self.mode = enum_var(enum_class=mode_distribution, value=mode_distribution.ALL_IN_ONE_SLIDE)
        self.slide_background_color = tkinter.StringVar(value="#ffffff")
        self.image_padding = dimensions(32, 32)

        self._screen_dims = dimensions(960, 540)
        self._pres_file_path = None
        self._source_slide_idx = 0
        self._images = []

        self._trace_image_padding = None
        self._trace_mode = None
        self._trace_word_position = None
        self._trace_word_size = None
        self._trace_word_font = None

    def _extract_from_pdf(self):
        print("TODO Implement.")
        return ""

    def _load_pres_data(self):
        self._disable_param_traces()

        if len(self.pres.slides) > 0:
            self.slide_names.clear()
            ctr = 0
            for slide in self.pres.slides:
                name = slide.name
                if name is None or len(name) == 0:
                    self.slide_names.append("Slide {}".format(ctr))
                else:
                    self.slide_names.append(name)
                ctr += 1
            self.source_slide_name.set(self.slide_names[-1])
        else:
            return False

        slide_width = pptx.util.Emu(self.pres.slide_width)
        slide_height = pptx.util.Emu(self.pres.slide_height)
        self._screen_dims.set_dims(int(slide_width.pt), int(slide_height.pt))

        # last_slide = self.pres.slides[-1]
        # if last_slide.background is not None:
        #    if last_slide.background.fill is not None:
        #        if last_slide.background.fill.fore_color is not None:
        #            color = last_slide.background.fill.fore_color
        #            print(color.rgb)
        #            if hasattr(color, "rgb"):
        #                rgb = color.rgb
        #                if rgb is not None:
        #                    print(str(rgb))

        if self.is_all_set():
            if self._setup_images() is False:
                return False

        return True

    def _load_input_data(self):
        self._disable_param_traces()

        files = glob.glob(os.path.join(self.input_dir, "*.jpg"))
        files.extend(glob.glob(os.path.join(self.input_dir, "*.png")))
        files.extend(glob.glob(os.path.join(self.input_dir, "*.svg")))
        files.extend(glob.glob(os.path.join(self.input_dir, "*.tga")))
        files.extend(glob.glob(os.path.join(self.input_dir, "*.gif")))

        self._images.clear()

        self.num_images.set(len(files))

        if(self.num_images.get() == 0):
            self.image_grid_dims.set_dims(0, 0)
            self.image_dims.set_dims(0, 0)
            self.image_scaled_dims.set_dims(0, 0)
            return input_data_load_result.FAILURE

        return_code = input_data_load_result.OK
        average_width = 0
        average_height = 0
        reference_width = 0
        reference_height = 0
        for image_file in files:
            image = slide_image(image_file)
            self._images.append(image)
            image_width = image.original_width
            image_height = image.original_height
            if reference_width == 0:
                reference_width = image_width
            if reference_height == 0:
                reference_height = image_height
            if image_width != reference_width or image_height != reference_height:
                return_code = input_data_load_result.OK_NON_UNIFORM_DIMENSIONS
            average_width += image_width
            average_height += image_height
        average_width = average_width / self.num_images.get()
        average_height = average_height / self.num_images.get()
        self.image_dims.set_dims(average_width, average_height)

        if self.is_all_set():
            setup_result = self._setup_images()
            if setup_result is False:
                return_code = input_data_load_result.FAILURE

        return return_code

    def _setup_images(self):
        self._disable_param_traces()

        screen_width = self._screen_dims.x.get()
        screen_height = self._screen_dims.y.get()

        image_width = self.image_dims.x.get()
        image_height = self.image_dims.y.get()

        padding_width = self.image_padding.x.get()
        padding_height = self.image_padding.y.get()

        result = slide_image.distribute(self._images, screen_width, screen_height, image_width, image_height, padding_width, padding_height)

        if result is False:
            return False

        # Otherwise, result is distribution_result
        self.image_grid_dims.set_dims(result.grid_size_x, result.grid_size_y)
        self.image_scaled_dims.set_dims(result.scaled_dims_x, result.scaled_dims_y)

        self._enable_param_traces()

        return True

    def _enable_param_traces(self):
        self._trace_image_padding = self.image_padding.trace("w", lambda name, index, mode, self=self: self._on_edit_traced())
        self._trace_mode = self.mode.trace("w", lambda name, index, mode, self=self: self._on_edit_traced())
        self._trace_word_position = self.word_position.trace("w", lambda name, index, mode, self=self: self._on_edit_traced())
        self._trace_word_size = self.word_size.trace("w", lambda name, index, mode, self=self: self._on_edit_traced())
        self._trace_word_font = self.word_font.trace("w", lambda name, index, mode, self=self: self._on_edit_traced())

    def _disable_param_traces(self):
        if self._trace_image_padding is not None:
            self.image_padding.trace_vdelete("w", self._trace_image_padding)
            self._trace_image_padding = None
        if self._trace_mode is not None:
            self.mode.trace_vdelete("w", self._trace_mode)
            self._trace_mode = None
        if self._trace_word_position is not None:
            self.word_position.trace_vdelete("w", self._trace_word_position)
            self._trace_word_position = None
        if self._trace_word_size is not None:
            self.word_size.trace_vdelete("w", self._trace_word_size)
            self._trace_word_size = None
        if self._trace_word_font is not None:
            self.word_font.trace_vdelete("w", self._trace_word_font)
            self._trace_word_font = None

    def _on_edit_traced(self):
        self._setup_images()
        self.draw(self.canvas)

    def _acquire_slide(self):
        op = self.operation.get_enum()
        if op == mode_work_slide.NEW_SLIDE:
            source_slide_layout = self.pres.slides[self._source_slide_idx].slide_layout
            return self.pres.slides.add_slide(source_slide_layout)
        elif op == mode_work_slide.SOURCE_SLIDE:
            slide = self.pres.slides[self._source_slide_idx]
            # slide.shapes.clear()          # Not supported.
            # slide.placeholders.clear()    # Not supported.
            return slide
        else:
            print("Error:", "Unknown mode operation.")
            return None

    def _create_internal(self, use_background):
        slide = self._acquire_slide()
        if slide is None:
            return False

        for img in self._images:
            img.add_to_slide(slide)

        if use_background:
            slide.background.fill.solid()
            slide.background.fill.fore_color.rgb = pptx.dml.color.RGBColor.from_string(self.slide_background_color.get()[1:])

        self.pres.save(self._pres_file_path)

        return True

    def _on_source_slide_name_changed(self):
        # Update the idx.
        slide_name = self.source_slide_name.get()
        try:
            self._source_slide_idx = self.slide_names.index(slide_name)
        except ValueError as e:
            print(e)
            print("Not found slide name. Not setting the slide idx.")

    def _debug_print_options(self):
        print("distributor:")
        print("\tpres:", self.pres)
        print("\tinput_dir:", self.input_dir)
        print("\tnum_images:", self.num_images.get())
        print("\timage_grid_dims:", self.image_grid_dims._get_str())
        print("\timage_dims:", self.image_dims._get_str())
        print("\timage_scaled_dims:", self.image_scaled_dims._get_str())
        print("\tsource_slide_name:", self.source_slide_name.get())
        print("\tword_font:", self.word_font.get())
        print("\tword_size:", self.word_size.get())
        print("\tword_position:", self.word_position.get_enum())
        print("\tmode:", self.mode.get_enum())
        print("\t_screen_dims:", self._screen_dims._get_str())
        print("\timage_padding:", self.image_padding._get_str())
        print("\t_pres_file_path:", self._pres_file_path)
        print("\t_source_slide_idx:", self._source_slide_idx)

    def is_all_set(self):
        return self.pres is not None and self.input_dir is not None and len(self._images) > 0

    def set_pres(self, path):
        if path.lower().endswith(".pptx") or path.lower().endswith(".pptm"):
            self._pres_file_path = path
            self.pres = pptx.Presentation(path)
            return self._load_pres_data()
        else:
            return False

    def set_input(self, path):
        if os.path.isdir(path):
            self.input_dir = path
            return self._load_input_data()
        else:
            return input_data_load_result.FAILURE

    def create(self, use_background):
        # self._debug_print_options()

        if self.pres is None:
            return False

        if self.input_dir is None:
            return False

        return self._create_internal(use_background)

    def init_draw(self, canvas):
        self.canvas = canvas
        for img in self._images:
            img.init_draw(canvas, self._screen_dims.x.get(), self._screen_dims.y.get())

    def draw(self, canvas):
        if canvas is None:
            return
        for img in self._images:
            img.draw(canvas, self._screen_dims.x.get(), self._screen_dims.y.get())


# ####### UI

# https://stackoverflow.com/questions/51902451/how-to-enable-and-disable-frame-instead-of-individual-widgets-in-python-tkinter/52152773
class labelframe_disableable(tkinter.LabelFrame):
    def enable(self, status="normal"):
        def cstate(widget):
            # Is this widget a container?
            if widget.winfo_children:
                # It's a container, so iterate through its children
                for w in widget.winfo_children():
                    if len(w.winfo_children()) == 0:
                        w.configure(state=status)
                    else:
                        cstate(w)
        cstate(self)

    def disable(self):
        self.enable(status="disabled")


class panel_extract_jpgs:
    def __init__(self, extractor):
        # TODO
        pass


class panel_edit_words:
    def __init__(self, root, current_words):
        self.words = current_words
        self._create_widgets(root)

    def _create_widgets(self, root):
        # TODO
        pass


class window:
    _color_bg = "#fafafa"
    _str_select_pres = "Please select a presentation file."
    _str_select_input = "Please select a PDF file or an image directory."

    def __init__(self):
        self._create_top()
        self.distrib = distributor()
        self._create_widgets()
        self._lock_edit()

    def _create_top(self):
        self.width = 770
        self.height = 650

        self.top = tkinter.Tk()
        self.top.title("Distribute Images Tool")
        self.top.resizable(False, False)
        self.top.geometry("{}x{}".format(self.width, self.height))
        self.top.configure(bg=window._color_bg)
        # self.top.columnconfigure(0, minsize=width)

    def _create_widgets(self):
        # Directory with images (TODO: PDF file for automatic extraction)

        # After load:
        # # Either update selected slide or create a new one based on selected slide
        # # Total number of images and number n x n
        # # Spacing size between them (recalculates size and n x n)
        # # Recalculated (display warning when they are different and take average)
        # # Word positioning
        # # Word font
        # # Slide background
        # # Preview window? Display preview using simple rectangle shapes
        # # Create button

        #####################

        # File picker

        frame_pick_files = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(frame_pick_files, 0, 0, w=2)

        self.label_pres = tkinter.Label(frame_pick_files, width=92, text=window._str_select_pres,
                                        anchor=tkinter.W, bg=window._color_bg)
        window._w_place_in_grid(self.label_pres, 0, 0)

        self.btn_select_pres = tkinter.Button(frame_pick_files, width=12, text="Presentation", command=self._cmd_select_file_pptx)
        window._w_place_in_grid(self.btn_select_pres, 1, 0, orientation=tkinter.E)

        self.label_dir = tkinter.Label(frame_pick_files, width=92, text=window._str_select_input,
                                       anchor=tkinter.W, bg=window._color_bg)
        window._w_place_in_grid(self.label_dir, 0, 1)

        frame_two_button = tkinter.Frame(frame_pick_files, bg=window._color_bg)
        window._w_place_in_grid(frame_two_button, 1, 1, orientation=tkinter.E)

        self.btn_select_file = tkinter.Button(frame_two_button, text="PDF", width=5, command=self._cmd_select_file_pdf)
        window._w_place_in_grid(self.btn_select_file, 0, 0, orientation=tkinter.W, nopadding=True)

        self.btn_select_dir = tkinter.Button(frame_two_button, text="Images", command=self._cmd_select_dir)
        window._w_place_in_grid(self.btn_select_dir, 1, 0, orientation=tkinter.E, nopadding=True)

        #####################

        # Visual editor

        canvas_size_mul = 0.95
        self.canvas = tkinter.Canvas(self.top, bg=self.distrib.slide_background_color.get(),
                                     width=self.width * canvas_size_mul, height=(self.width * (9 / 16)) * canvas_size_mul)
        window._w_place_in_grid(self.canvas, 0, 1, w=2)
        self.distrib.slide_background_color.trace("w", lambda name, index, mode, self=self: self._on_slide_background_color_changed())

        #####################

        # Editing

        self.frame_edit = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(self.frame_edit, 0, 2)

        # Distribution mode
        # Source slide
        # Screen size
        # Image padding
        # Word positioning
        # Word font and size (latter adjusts automatically to distrib. mode)

        self.edit_source_slide = window._w_create_pair_combobox(self.frame_edit, "Source slide:", [self.distrib.source_slide_name.get()],
                                                                self.distrib.source_slide_name, 0, 0)
        # TODO Add more font support. For now let's support just one.
        self.edit_word_font = window._w_create_pair_combobox(self.frame_edit, "Word font:", [self.distrib.word_font.get()],
                                                             self.distrib.word_font, 0, 1)
        self.edit_word_font_size = window._w_create_pair_num_edit(self.frame_edit, "Word size:", self.distrib.word_size, 0, 2)
        self.edit_word_positioning = window._w_create_pair_combobox(self.frame_edit, "Word position:",
                                                                    enum_enhanced.get_names(word_orientation), self.distrib.word_position,
                                                                    0, 3)
        self.edit_operation = window._w_create_pair_combobox(self.frame_edit, "Work slide:", enum_enhanced.get_names(mode_work_slide),
                                                             self.distrib.operation, 2, 0)
        self.edit_mode = window._w_create_pair_combobox(self.frame_edit, "Distribution:", enum_enhanced.get_names(mode_distribution),
                                                        self.distrib.mode, 2, 1)
        self.edit_bg_color = window._w_create_pair_color_picker(self.frame_edit, "Background color:", self.distrib.slide_background_color, 2, 2)
        self.edit_padding = window._w_create_pair_dim_edit(self.frame_edit, "Image padding (pt):", self.distrib.image_padding, 2, 3)

        # #####################

        # Informations

        self.frame_info = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(self.frame_info, 1, 2)

        self.label_number = window._w_create_pair_label(self.frame_info, "Number of images:", self.distrib.num_images, 0, 0)
        self.label_number_nxn = window._w_create_pair_label(self.frame_info, "Grid dimensions (WxH):", self.distrib.image_grid_dims, 0, 1)
        self.label_dimensions_ori = window._w_create_pair_label(self.frame_info, "Ori. image dimensions (px):", self.distrib.image_dims, 0, 2)
        self.label_dimensions_rec = window._w_create_pair_label(self.frame_info, "Scl. image dimensions (pt):", self.distrib.image_scaled_dims, 0, 3)

        #####################

        # Create button at the very end.

        self.btn_create = tkinter.Button(self.top, text="Create", width=34, command=self._cmd_create)
        window._w_place_in_grid(self.btn_create, 0, 3, 2)

    def _lock_edit(self):
        self.frame_edit.disable()
        self.frame_info.disable()
        self.btn_create.configure(state="disabled")

    def _unlock_edit(self):
        self.frame_edit.enable()
        self.frame_info.enable()
        self.btn_create.configure(state="normal")

    def _all_set(self):
        self._unlock_edit()
        self.distrib.init_draw(self.canvas)

    def _on_slide_background_color_changed(self):
        self.canvas["bg"] = self.distrib.slide_background_color.get()

    def _w_place_in_grid(widget, x, y, w=1, h=1, weight_w=1, weight_h=0, orientation="", nopadding=False):
        root = widget._root()
        padding = 3 if nopadding is False else 0
        widget.grid(row=y, column=x, rowspan=h, columnspan=w, padx=padding, pady=padding, sticky=orientation)
        root.columnconfigure(x, weight=weight_w)
        root.rowconfigure(y, weight=weight_h)

    def _w_create_num_input(root, var_value):
        def validate_num_input(val):
            int_val = 0
            try:
                int_val = int(val)
            except Exception:
                return False
            return int_val >= 1 and int_val <= 9999
        cmd = root.register(validate_num_input)
        return tkinter.Spinbox(root, textvariable=var_value, width=5,
                               from_=1, to=9999, validate="all",
                               validatecommand=(cmd, "%P"))

    def _w_create_pair(root, element, text_name, base_x, base_y):
        label_name = tkinter.Label(root, text=text_name, bg=window._color_bg)
        window._w_place_in_grid(label_name, base_x, base_y, orientation=tkinter.W)
        window._w_place_in_grid(element, base_x + 1, base_y, orientation=tkinter.E)
        return element

    def _w_create_pair_label(root, text_name, var_value, base_x, base_y):
        return window._w_create_pair(root, tkinter.Label(root, textvariable=var_value, bg=window._color_bg),
                                     text_name, base_x, base_y)

    def _w_create_pair_num_edit(root, text_name, var_value, base_x, base_y):
        dim = window._w_create_num_input(root, var_value)
        return window._w_create_pair(root, dim, text_name, base_x, base_y)

    def _w_create_pair_dim_edit(root, text_name, var_dim, base_x, base_y):
        inter_frame = tkinter.Frame(root, bg=window._color_bg)
        dim_x = window._w_create_num_input(inter_frame, var_dim.x)
        dim_y = window._w_create_num_input(inter_frame, var_dim.y)
        sep = tkinter.Label(inter_frame, text="x", bg=window._color_bg)
        window._w_place_in_grid(dim_x, 0, 0, nopadding=True)
        window._w_place_in_grid(sep, 1, 0, nopadding=True)
        window._w_place_in_grid(dim_y, 2, 0, nopadding=True)
        return window._w_create_pair(root, inter_frame, text_name, base_x, base_y)

    def _w_create_pair_combobox(root, text_name, options, var_value, base_x, base_y):
        def func_validate():
            return False
        cmd = root.register(func_validate)
        combo_box = ttk.Combobox(root, values=options, textvariable=var_value,
                                 validate="key", validatecommand=cmd)
        if(len(options) > 0):
            combo_box.current(0)
        return window._w_create_pair(root, combo_box, text_name, base_x, base_y)

    def _w_create_pair_color_picker(root, text_name, var_value, base_x, base_y):
        picker = color_picker(root, var_value, window._color_bg)
        return window._w_create_pair(root, picker, text_name, base_x, base_y)

    def _check_file_name(file_name):
        return file_name is not None and len(file_name) > 0

    def _update_pres_data(self):
        if self.distrib.input_dir is not None:
            self._all_set()
        self.edit_source_slide["values"] = self.distrib.slide_names

    def _update_input_data(self):
        if self.distrib.pres is not None:
            self._all_set()

    def _display_not_implemented():
        tkinter.messagebox.showerror("Error", "This function is not yet implemented.")

    def _mark_as_loading(label):
        label["text"] = "Loading..."

    def _cmd_select_file_pptx(self):
        file_name = tkinter.filedialog.askopenfilename(filetypes=[("PowerPoint files", "*.pptx *.pptm")])
        if window._check_file_name(file_name):
            window._mark_as_loading(self.label_pres)
            res = self.distrib.set_pres(file_name)
            if res is True:
                self.label_pres["text"] = file_name
                self._update_pres_data()
            else:
                self.label_pres["text"] = window._str_select_input

    def _cmd_select_file_pdf(self):
        # TODO
        window._display_not_implemented()

        # file_name = tkinter.filedialog.askopenfilename(filetypes=[("PDF files", "*.pdf")])
        # if window._check_file_name(file_name):
        # self.distrib.set_input(file_name)
        # self.label_dir["text"] = file_name

    def _cmd_select_dir(self):
        file_name = tkinter.filedialog.askdirectory()
        if window._check_file_name(file_name):
            window._mark_as_loading(self.label_dir)
            res = self.distrib.set_input(file_name)
            if res is not input_data_load_result.FAILURE:
                if res is input_data_load_result.OK_NON_UNIFORM_DIMENSIONS:
                    tkinter.messagebox.showwarning("Warning", "Some images have different dimensions than others.\n"
                                                              "Will average the dimensions to provide an uniform grid.")
                self.label_dir["text"] = file_name
                self._update_input_data()
            else:
                self.label_dir["text"] = window._str_select_input

    def _cmd_create(self):
        use_bg = self.edit_bg_color.is_enabled()
        success = self.distrib.create(use_bg)
        if success:
            tkinter.messagebox.showinfo("Success", "Image distribution succeeded.")
        else:
            tkinter.messagebox.showerror("Error", "An error has occured.")

    def loop(self):
        self.top.mainloop()
        # while True:
        # self.top.update_idletasks()
        # self.top.update()
        # time.sleep(0.016)


def main():
    wnd = window()
    wnd.loop()


if __name__ == "__main__":
    main()
