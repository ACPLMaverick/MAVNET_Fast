#!/usr/bin/python

from modules.utils import enum_enhanced
from modules.utils import tk_util
from modules.indices import indices
from modules.indices import indices_generator
import sys
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.colorchooser


class debug_mode(enum_enhanced):
    NO_DEBUG = 0,
    ONLY_GENERATOR = 1


class window(tkinter.Tk):
    def __init__(self):
        super().__init__()
        self._width = 400
        self._height = 300
        self.title("Dobble Generator")
        self.resizable(False, False)
        self.geometry("{}x{}".format(self._width, self._height))
        tk_util.bind_enter_esc(self, None, self._conditional_exit)
        self.protocol("WM_DELETE_WINDOW", self._conditional_exit)
        self._build_controls()
        tk_util.window_set_on_cursor(self)

    def _build_controls(self):
        pass

    def _conditional_exit(self):
        if tkinter.messagebox.askyesno("Are you sure?", "Are you sure you want to exit?"):
            self.destroy()


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