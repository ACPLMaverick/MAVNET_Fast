#!/usr/bin/python

import sys
from enum import Enum
import os
from pptx import Presentation
import tkinter
import tkinter.filedialog
import tkinter.messagebox
from tkinter import ttk


# Add possibility to drag-drop files.
# Add option for overwriting or not output files.
# Make program remember last used paths

# ###### LOGIC

class word_orientation(Enum):
    TOP_LEFT = 0,
    TOP_RIGHT = 1,
    BOTTOM_RIGHT = 2,
    BOTTOM_LEFT = 3


class distributor_mode(Enum):
    ALL_IN_ONE_SLIDE = 0,
    MULTIPLE_SLIDES = 1


class dimensions:
    def __init__(self, x, y):
        self.x = x
        self.y = y


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
        self.pres_file = None
        self.pdf_file = None
        self.image_dir = None
        self.is_image_dir_temp = False

        self.num_images = 0
        self.image_grid_dims = dimensions(0, 0)
        self.image_dims = dimensions(0, 0)
        self.image_scaled_dims = dimensions(0, 0)

        self.source_slide = None
        self.word_font = "Impact"
        self.word_size = 24
        self.word_position = word_orientation.TOP_LEFT
        self.mode = distributor_mode.ALL_IN_ONE_SLIDE
        self.screen_dims = dimensions(1920, 1080)
        self.image_padding = dimensions(32, 32)

    def _extract_from_pdf(self):
        print("TODO Implement.")
        return ""

    def _create_internal(self):
        prs = Presentation(self.pres_file)
        title_slide_layout = prs.slide_layouts[0]
        slide = prs.slides.add_slide(title_slide_layout)
        title = slide.shapes.title
        subtitle = slide.placeholders[1]

        title.text = "Hello, World!"
        subtitle.text = "python-pptx was here!"

        prs.save(self.pres_file)

        return True

    def recalculate(self):
        pass

    def set_pres(self, path):
        if path.lower().endswith(".pptx") or path.lower().endswith(".pptm"):
            self.pres_file = path

    def set_input(self, path):
        print("File or dir:", path)
        if os.path.isdir(path):
            self.image_dir = path
        elif path.lower().endswith(".pdf"):
            self.pdf_file = path

    def create(self):
        if self.pres_file is None:
            return False

        if self.pdf_file is not None:
            self.is_image_dir_temp = True
            self.image_dir = self._extract_from_pdf()

        if self.image_dir is None:
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

    def __init__(self, distrib):
        self.distrib = distrib
        self._create_widgets()
        self._lock_edit()

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

        width = 770
        height = 650

        self.top = tkinter.Tk()
        self.top.title("Distribute Images Tool")
        self.top.resizable(False, False)
        self.top.geometry("{}x{}".format(width, height))
        self.top.configure(bg=window._color_bg)
        # self.top.columnconfigure(0, minsize=width)

        #####################

        # File picker

        frame_pick_files = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(frame_pick_files, 0, 0, w=2)

        self.label_pres = tkinter.Label(frame_pick_files, width=92, text="Please select a presentation file.",
                                        anchor=tkinter.W, bg=window._color_bg)
        window._w_place_in_grid(self.label_pres, 0, 0)

        self.btn_select_pres = tkinter.Button(frame_pick_files, width=12, text="Presentation", command=self._cmd_select_file_pptx)
        window._w_place_in_grid(self.btn_select_pres, 1, 0, orientation=tkinter.E)

        self.label_dir = tkinter.Label(frame_pick_files, width=92, text="Please select a PDF file or an image directory.",
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
                                     width=width * canvas_size_mul, height=(width * (9 / 16)) * canvas_size_mul)
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

        self.edit_source_slide = window._w_create_pair_combobox(self.frame_edit, "Source slide:", [], 0, 0)
        # TODO Add more font support. For now let's support just one.
        self.edit_word_font = window._w_create_pair_combobox(self.frame_edit, "Word font:", ["Impact"], 0, 1)
        self.edit_word_font_size = window._w_create_pair_num_edit(self.frame_edit, "Word size:", 24, 0, 2)
        self.edit_word_positioning = window._w_create_pair_combobox(self.frame_edit, "Word position:",
                                                                    ["Top-left", "Top-right", "Bottom-right", "Bottom-left"],
                                                                    0, 3)
        self.edit_mode = window._w_create_pair_combobox(self.frame_edit, "Mode:", ["All in one slide", "Multiple slides"], 2, 0)
        self.edit_ssize = window._w_create_pair_dim_edit(self.frame_edit, "Screen dimensions:", 1920, 1080, 2, 1)
        self.edit_padding = window._w_create_pair_dim_edit(self.frame_edit, "Image padding:", 32, 32, 2, 2)
        self.btn_edit_words = tkinter.Button(self.frame_edit, text="Edit words", width=34, command=self._cmd_edit_words)
        window._w_place_in_grid(self.btn_edit_words, 2, 3, w=2)

        # #####################

        # Informations

        self.frame_info = labelframe_disableable(self.top, bg=window._color_bg)
        window._w_place_in_grid(self.frame_info, 1, 2)

        self.label_number = window._w_create_pair_label(self.frame_info, "Number of images:", "0000", 0, 0)
        self.label_number_nxn = window._w_create_pair_label(self.frame_info, "Grid dimensions (WxH):", "0000 x 0000", 0, 1)
        self.label_dimensions_ori = window._w_create_pair_label(self.frame_info, "Original image dimensions:", "0000 x 0000", 0, 2)
        self.label_dimensions_rec = window._w_create_pair_label(self.frame_info, "Scaled image dimensions:", "0000 x 0000", 0, 3)

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

    def _w_create_pair(root, element, text_name, base_x, base_y):
        label_name = tkinter.Label(root, text=text_name, bg=window._color_bg)
        window._w_place_in_grid(label_name, base_x, base_y, orientation=tkinter.W)
        window._w_place_in_grid(element, base_x + 1, base_y, orientation=tkinter.E)
        return element

    def _w_create_pair_label(root, text_name, text_value, base_x, base_y):
        var = tkinter.StringVar(value=text_value)
        return window._w_create_pair(root, tkinter.Label(root, textvariable=var, bg=window._color_bg),
                                     text_name, base_x, base_y)

    def _w_create_pair_num_edit(root, text_name, init, base_x, base_y):
        var = tkinter.IntVar(value=init)
        dim = tkinter.Spinbox(root, textvariable=var, width=5, from_=1, to=100)
        return window._w_create_pair(root, dim, text_name, base_x, base_y)

    def _w_create_pair_dim_edit(root, text_name, init_a, init_b, base_x, base_y):
        inter_frame = tkinter.Frame(root, bg=window._color_bg)
        var_x = tkinter.IntVar(value=init_a)
        var_y = tkinter.IntVar(value=init_b)
        dim_x = tkinter.Spinbox(inter_frame, textvariable=var_x, width=5, from_=1, to=3840)
        dim_y = tkinter.Spinbox(inter_frame, textvariable=var_y, width=5, from_=1, to=2160)
        sep = tkinter.Label(inter_frame, text="x", bg=window._color_bg)
        window._w_place_in_grid(dim_x, 0, 0, nopadding=True)
        window._w_place_in_grid(sep, 1, 0, nopadding=True)
        window._w_place_in_grid(dim_y, 2, 0, nopadding=True)
        return window._w_create_pair(root, inter_frame, text_name, base_x, base_y)

    def _w_create_pair_combobox(root, text_name, options, base_x, base_y):
        combo_box = ttk.Combobox(root, values=options)
        if(len(options) > 0):
            combo_box.current(0)
        return window._w_create_pair(root, combo_box, text_name, base_x, base_y)

    def _check_file_name(file_name):
        return file_name is not None and len(file_name) > 0

    def _update_file_or_dir_name(self, path):
        self.label_dir["text"] = path

    def _select_file(self, func, label, ftypes):
        file_name = tkinter.filedialog.askopenfilename(filetypes=ftypes)
        if window._check_file_name(file_name):
            func(file_name)
            label["text"] = file_name

    def _cmd_select_file_pptx(self):
        self._select_file(self.distrib.set_pres, self.label_pres, [("PowerPoint files", "*.pptx *.pptm")])

    def _cmd_select_file_pdf(self):
        self._select_file(self.distrib.set_input, self.label_dir, [("PDF files", "*.pdf")])

    def _cmd_select_dir(self):
        file_name = tkinter.filedialog.askdirectory()
        if window._check_file_name(file_name):
            self.distrib.set_input(file_name)
            self.label_dir["text"] = file_name

    def _cmd_edit_words(self):
        # TODO
        tkinter.messagebox.showerror("Error", "This function is not yet implemented.")

    def _cmd_create(self):
        success = self.distrib.create()
        if success:
            tkinter.messagebox.showinfo("Success", "Image distribution succeeded.")
        else:
            tkinter.messagebox.showerror("Error", "An error has occured.")

    def loop(self):
        self.top.mainloop()


def main():
    wnd = window(distributor())
    wnd.loop()


if __name__ == "__main__":
    main()
