#!/usr/bin/python

from modules.utils import enum_enhanced
from modules.utils import tk_util
from modules.utils import tk_informative_int_label
from modules.utils import tk_frame_disableable
from modules.indices import indices
from modules.indices import indices_generator
import sys
import os
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.colorchooser


class debug_mode(enum_enhanced):
    NO_DEBUG = 0,
    ONLY_GENERATOR = 1


class builder:
    def __init__(self):
        self._file_list = None
        self._generator = indices_generator()

    def set_file_list(self, file_list):
        self._file_list = file_list
        self._generator.set_num_elements(self.get_num_total_images())

    def get_num_total_images(self):
        return len(self._file_list)

    def get_num_cards(self):
        return self._generator.num_series

    def get_num_images_per_card(self):
        return self._generator.num_elements

    def is_ready(self):
        return self._file_list is not None and self.get_num_cards() > 0 and self.get_num_images_per_card() > 0

    def build(self, name, is_clear, max_num_shuffles) -> bool:
        if self.is_ready() is False:
            return False

        self._generator.set_max_shuffles(max_num_shuffles)
        idx = self._generator.generate()
        if idx is None or len(idx.sets) != self.get_num_cards():
            return False
        
        idx.print()

        return True


class window(tkinter.Tk):
    def __init__(self):
        super().__init__()

        self._width = 640
        self._height = 260

        self.title("Dobble Generator")
        self.resizable(False, False)
        self.geometry("{}x{}".format(self._width, self._height))
        tk_util.bind_enter_esc(self, None, self._conditional_exit)
        self.protocol("WM_DELETE_WINDOW", self._conditional_exit)
        self._build_controls()
        tk_util.window_set_on_cursor(self)

        self._builder = builder()

    def _build_controls(self):
        self._w_lbl_num_files = tk_informative_int_label(self, "Selected {} images.", 0)
        tk_util.place_in_grid(self._w_lbl_num_files, 0, 0, w=2)
        self._w_lbl_warning = tk_informative_int_label(self, "WARNING. Selected number of images will be truncated to nearest fitting number ({}).", 0)
        tk_util.place_in_grid(self._w_lbl_warning, 0, 1, w=2)
        self._w_lbl_warning.grid_remove()
        self._w_lbl_numbers = tk_informative_int_label(self, "Result: {} cards with {} images on each.", 0, 0)
        tk_util.place_in_grid(self._w_lbl_numbers, 0, 2, w=2)

        main_button_width = 25

        self._w_btn_select = tkinter.Button(self, text="Select", command=self._on_select_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_select, 0, 3, w=2)

        self._w_frame_options = tk_frame_disableable(self, pady=6)
        tk_util.place_in_grid(self._w_frame_options, 0, 4, w=2)
        self._w_edit_name = tk_util.w_create_pair_text_edit(self._w_frame_options, "Name:", "", 0, 0)
        self._w_edit_name.configure(width=16)
        self._var_is_clear = tkinter.BooleanVar(self, False)
        self._w_edit_clear_dir = tk_util.w_create_pair_checkbox(self._w_frame_options, "Clear:", self._var_is_clear, 0, 1)
        self._var_max_shuffles = tkinter.IntVar(self, 3)
        self._w_edit_max_shuffles = tk_util.w_create_pair_num_edit(self._w_frame_options, "Max shuffles:", self._var_max_shuffles, 0, 2, max_val=64)
        self._w_frame_options.disable()

        self._w_btn_generate = tkinter.Button(self, text="Generate", command=self._on_generate_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_generate, 0, 5, w=2)
        self._w_btn_generate.configure(state=tkinter.DISABLED)


    def _conditional_exit(self):
        if tkinter.messagebox.askyesno("Are you sure?", "Are you sure you want to exit?"):
            self.destroy()

    def _on_select_clicked(self):
        files_to_open = tkinter.filedialog.askopenfilenames(filetypes=[("Image files", "*.jpg *.jpeg *.png *.svg *.tga *.gif")])
        if len(files_to_open) > 0:
            self._builder.set_file_list(files_to_open)
            self._w_lbl_num_files.update(self._builder.get_num_total_images())
            self._w_lbl_numbers.update(self._builder.get_num_cards(), self._builder.get_num_images_per_card())
            if self._is_warning():
                self._w_lbl_warning.update(self._builder.get_num_images_per_card())
                self._w_lbl_warning.grid()
            else:
                self._w_lbl_warning.grid_remove()
            if self._builder.is_ready():
                self._w_frame_options.enable()
                self._w_btn_generate.configure(state=tkinter.NORMAL)
            else:
                self._w_frame_options.disable()
                self._w_btn_generate.configure(state=tkinter.DISABLED)

    def _on_generate_clicked(self):
        ret_val = self._builder.build(self._get_output_name(), self._var_is_clear.get(), self._var_max_shuffles.get())
        if ret_val:
            tkinter.messagebox.showinfo("Success", "Success.")
        else:
            tkinter.messagebox.showerror("Failure", "Failure. Please see the console output.")

    def _is_warning(self):
        return self._builder.get_num_total_images() != self._builder.get_num_images_per_card()

    def _is_generation_possible(self):
        # TODO
        return False

    def _get_output_name(self):
        base = os.path.join("dobble_generated", "{}")
        name_val = self._w_edit_name.get(1.0, tkinter.END)
        if not tk_util.check_file_name(name_val):
            name_val = "unnamed"
        return base.format(name_val)


def main():
    debug = debug_mode.NO_DEBUG

    if debug == debug_mode.NO_DEBUG:
        wnd = window()
        wnd.mainloop()
    elif debug == debug_mode.ONLY_GENERATOR:
        print("Dobble!")

        # Like in real-life dobble set. Configurable later.
        num_elements_per_serie = 8
        max_shuffles = 8

        generator = indices_generator(num_elements_per_serie, max_shuffles)
        idc = generator.generate()
        idc.print()

    return 0


if __name__ == "__main__":
    sys.exit(main())