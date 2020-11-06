#!/usr/bin/python

import sys
import time
import glob
from enum import Enum
import os
import pptx
import pptx.util
import tkinter
import tkinter.filedialog
import tkinter.messagebox
from PIL import ImageTk, Image
from tkinter import ttk


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


class distributor_mode(enum_enhanced):
    ALL_IN_ONE_SLIDE = 0,
    MULTIPLE_SLIDES = 1


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


# slide_image's anchor is CENTER
class slide_image:
    def __init__(self, x, y, width, height):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def draw(self, canvas):
        # TODO
        pass


class slide_visualizer:
    def __init__(self):
        self.images = ()

    def draw(self, canvas):
        # TODO
        pass


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
        self.mode = enum_var(enum_class=distributor_mode, value=distributor_mode.ALL_IN_ONE_SLIDE)
        self.screen_dims = dimensions(1920, 1080)
        self.image_padding = dimensions(32, 32)

        self._pres_file_path = None
        self._source_slide_idx = 0
        self._images = []

    def _extract_from_pdf(self):
        print("TODO Implement.")
        return ""

    def _load_pres_data(self):
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
            self.source_slide_name.set(self.slide_names[0])
        else:
            return False

        slide_width = pptx.util.Emu(self.pres.slide_width)
        slide_height = pptx.util.Emu(self.pres.slide_height)
        self.screen_dims.x.set(int(slide_width.pt))
        self.screen_dims.y.set(int(slide_height.pt))

        return True

    def _load_input_data(self):
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
        for image_file in files:
            image = ImageTk.PhotoImage(Image.open(image_file))
            self._images.append(image)
            image_width = image.width()
            image_height = image.height()
            if (average_width != 0 and average_width != image_width) or (average_height != 0 and average_height != image_height):
                return_code = input_data_load_result.OK_NON_UNIFORM_DIMENSIONS
            average_width += image_width
            average_height += image_height
        average_width = average_width / self.num_images.get()
        average_height = average_height / self.num_images.get()
        self.image_dims.set_dims(average_width, average_height)

        return return_code

    def _create_internal(self):
        title_slide_layout = self.pres.slides[self._source_slide_idx].slide_layout
        slide = self.pres.slides.add_slide(title_slide_layout)
        title = slide.shapes.title
        subtitle = slide.placeholders[1]

        title.text = "Hello, World!"
        subtitle.text = "python-pptx was here!"

        self.pres.save(self._pres_file_path)

        return True

    def __create_internal(self):
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
        print("\tscreen_dims:", self.screen_dims._get_str())
        print("\timage_padding:", self.image_padding._get_str())
        print("\t_pres_file_path:", self._pres_file_path)
        print("\t_source_slide_idx:", self._source_slide_idx)

    def recalculate(self):
        pass

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

    def create(self):
        self._debug_print_options()

        if self.pres is None:
            return False

        if self.input_dir is None:
            return False

        return self._create_internal()


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
    _color_canvas_bg = "#ffffff"
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
        self.canvas = tkinter.Canvas(self.top, bg=window._color_canvas_bg,
                                     width=self.width * canvas_size_mul, height=(self.width * (9 / 16)) * canvas_size_mul)
        window._w_place_in_grid(self.canvas, 0, 1, w=2)

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
        self.edit_mode = window._w_create_pair_combobox(self.frame_edit, "Mode:", enum_enhanced.get_names(distributor_mode),
                                                        self.distrib.mode, 2, 0)
        self.edit_ssize = window._w_create_pair_dim_edit(self.frame_edit, "Screen dimensions:", self.distrib.screen_dims, 2, 1)
        self.edit_padding = window._w_create_pair_dim_edit(self.frame_edit, "Image padding:", self.distrib.image_padding, 2, 2)
        self.btn_edit_words = tkinter.Button(self.frame_edit, text="Edit words", width=34, command=self._cmd_edit_words)
        window._w_place_in_grid(self.btn_edit_words, 2, 3, w=2)

        # #####################

        # Informations

        self.frame_info = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(self.frame_info, 1, 2)

        self.label_number = window._w_create_pair_label(self.frame_info, "Number of images:", self.distrib.num_images, 0, 0)
        self.label_number_nxn = window._w_create_pair_label(self.frame_info, "Grid dimensions (WxH):", self.distrib.image_grid_dims, 0, 1)
        self.label_dimensions_ori = window._w_create_pair_label(self.frame_info, "Original image dimensions:", self.distrib.image_dims, 0, 2)
        self.label_dimensions_rec = window._w_create_pair_label(self.frame_info, "Scaled image dimensions:", self.distrib.image_scaled_dims, 0, 3)

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

    def _check_file_name(file_name):
        return file_name is not None and len(file_name) > 0

    def _update_pres_data(self):
        if self.distrib.input_dir is not None:
            self._unlock_edit()
        self.edit_source_slide["values"] = self.distrib.slide_names

    def _update_input_data(self):
        if self.distrib.pres is not None:
            self._unlock_edit()

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

    def _cmd_edit_words(self):
        # TODO
        window._display_not_implemented()

    def _cmd_create(self):
        success = self.distrib.create()
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
