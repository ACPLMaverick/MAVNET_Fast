#!/usr/bin/python

from enum import Enum
import tkinter

class enum_enhanced(Enum):
    def __str__(self):
        return self.name.replace("_", " ").capitalize()

    def get_names(enum_class):
        return [str(e) for e in enum_class]

    def reconvert_name(name):
        return name.upper().replace(" ", "_")


class tk_util:
    def create_num_input(root, var_value, min_val, max_val, divisor=1):
        def validate_num_input(val, widget):
            int_val = 0
            try:
                int_val = int(val)
            except Exception:
                try:
                    is_zero_length_string = len(val) == 0
                    return is_zero_length_string
                except Exception:
                    return False
                return False
            return int_val >= widget["from"] and int_val <= widget["to"] and int_val % divisor == 0
        min_val = min_val + (min_val % divisor)
        max_val = max_val + (max_val % divisor)
        sb = tkinter.Spinbox(root, textvariable=var_value, width=5,
                             from_=min_val, to=max_val, increment=divisor, validate="all")
        cmd = root.register(lambda val: validate_num_input(val, sb))
        sb.configure(validatecommand=(cmd, "%P"))
        return sb

    def place_in_grid(widget, x, y, w=1, h=1, weight_w=1, weight_h=0, orientation="", nopadding=False):
        root = widget._root()
        padding = 3 if nopadding is False else 0
        widget.grid(row=y, column=x, rowspan=h, columnspan=w, padx=padding, pady=padding, sticky=orientation)
        root.columnconfigure(x, weight=weight_w)
        root.rowconfigure(y, weight=weight_h)

    def window_set_on_cursor(window):
        rt = window._root()
        x = rt.winfo_pointerx()
        y = rt.winfo_pointery()
        window.geometry("+{}+{}".format(x, y))

    def window_set_on_root(window, orientation=tkinter.CENTER, offset_x=0, offset_y=0):
        rt = window._root()
        x = int(rt.winfo_x()) + offset_x
        y = int(rt.winfo_y()) + offset_y
        if orientation == tkinter.CENTER:
            x += int(rt.winfo_width() / 2)
            y += int(rt.winfo_height() / 2)
        window.geometry("+{}+{}".format(x, y))

    def bind_enter_esc(window, func_enter=None, func_esc=None):
        if func_enter is not None:
            window.bind("<Return>", lambda event: func_enter())
        if func_esc is not None:
            window.bind("<Escape>", lambda event: func_esc())