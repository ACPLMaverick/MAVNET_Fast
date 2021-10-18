#!/usr/bin/python

from enum import Enum
import tkinter
from tkinter import ttk

class enum_enhanced(Enum):
    def __str__(self):
        return self.name.replace("_", " ").capitalize()

    def get_names(enum_class):
        return [str(e) for e in enum_class]

    def reconvert_name(name):
        return name.upper().replace(" ", "_")


class tk_enum_var(tkinter.StringVar):
    def __init__(self, enum_class, *args, **kwargs):
        self.enum_class = enum_class
        super().__init__(*args, **kwargs)

    def get_enum(self):
        return self.enum_class[self.enum_class.reconvert_name(self.get())]


class tk_util:
    def w_create_pair_label(root, text_name, var_value, base_x, base_y):
        return tk_util._w_create_pair(root, tkinter.Label(root, textvariable=var_value),
                                     text_name, base_x, base_y)

    def w_create_pair_text_edit(root, text_name, initial_text, base_x, base_y):
        txt = tkinter.Entry(root)
        if type(initial_text) is tkinter.StringVar:
            txt.configure(textvariable=initial_text)
        else:
            txt.insert(0, initial_text)
        return tk_util._w_create_pair(root, txt, text_name, base_x, base_y)

    def w_create_pair_num_edit(root, text_name, var_value, base_x, base_y, min_val=1, max_val=9999, divisor=1):
        dim = tk_util.w_create_num_input(root, var_value, min_val, max_val, divisor)
        return tk_util._w_create_pair(root, dim, text_name, base_x, base_y)

    def w_create_pair_dim_edit(root, text_name, var_dim, base_x, base_y, min_val=1, max_val=9999, divisor=1):
        inter_frame = tkinter.Frame(root)
        dim_x = tk_util.create_num_input(inter_frame, var_dim.x, min_val, max_val, divisor)
        dim_y = tk_util.create_num_input(inter_frame, var_dim.y, min_val, max_val, divisor)
        sep = tkinter.Label(inter_frame, text="x")
        tk_util.place_in_grid(dim_x, 0, 0, nopadding=True)
        tk_util.place_in_grid(sep, 1, 0, nopadding=True)
        tk_util.place_in_grid(dim_y, 2, 0, nopadding=True)
        return tk_util._w_create_pair(root, inter_frame, text_name, base_x, base_y)

    def w_create_pair_combobox(root, text_name, options, var_value, base_x, base_y):
        def func_validate():
            return False
        cmd = root.register(func_validate)
        combo_box = ttk.Combobox(root, values=options, textvariable=var_value,
                                 validate="key", validatecommand=cmd)
        if(len(options) > 0):
            if isinstance(var_value, tk_enum_var):
                combo_box.current(var_value.get_enum().value)
            else:
                combo_box.current(0)
        return tk_util._w_create_pair(root, combo_box, text_name, base_x, base_y)

    def w_create_pair_checkbox(root, text_name, var_value, base_x, base_y):
        cb = tkinter.Checkbutton(root, variable=var_value)
        return tk_util._w_create_pair(root, cb, text_name, base_x, base_y)

    def w_create_pair_color_picker(root, text_name, var_value, base_x, base_y, can_be_disabled=True):
        picker = tk_color_picker(root, var_value, can_be_disabled=can_be_disabled)
        return tk_util._w_create_pair(root, picker, text_name, base_x, base_y)

    def w_create_num_input(root, var_value, min_val, max_val, divisor=1):
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

    def window_set_on_cursor(window, offset=(0, 0)):
        rt = window._root()
        x = max(rt.winfo_pointerx() + int(offset[0]), 0)
        y = max(rt.winfo_pointery() + int(offset[1]), 0)
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

    def check_file_name(file_name):
        return file_name is not None and len(file_name) > 0

    def _w_create_pair(root, element, text_name, base_x, base_y):
        label_name = tkinter.Label(root, text=text_name)
        tk_util.place_in_grid(label_name, base_x, base_y, orientation=tkinter.W)
        tk_util.place_in_grid(element, base_x + 1, base_y, orientation=tkinter.E)
        return element


class tk_informative_int_label(tkinter.Label):
    def __init__(self, master, text:str, *args) -> None:
        self._text = text
        self._str_var = tkinter.StringVar(master, value=self._text.format(*args))
        super().__init__(master, textvariable=self._str_var)

    def update(self, text=None, *args):
        if text is not None:
            self._text = text
        self._str_var.set(self._text.format(*args))


class tk_frame_disableable(tkinter.Frame):
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


class tk_color_picker(tkinter.Frame):
    def __init__(self, master, var_color, width=60, height=21, command=None, can_be_disabled=True):
        super().__init__(master, width=width, height=height)
        self._can_be_disabled = can_be_disabled
        width_btn = int(0.8 * width) if can_be_disabled else width
        width_cb = width - width_btn
        if can_be_disabled:
            self._var_check = tkinter.IntVar(value=0)
            self.cb = tkinter.Checkbutton(self, variable=self._var_check, width=width_cb, height=height)
        self.btn = tkinter.Button(self, command=self._on_clicked, bg=var_color.get(), fg=var_color.get())
        if can_be_disabled:
            self.cb.place(x=0, y=0, width=width_cb, height=height)
        self.btn.place(x=width_cb, y=0, width=width_btn, height=height)
        self.var_color = var_color
        self.var_color.trace("w", lambda name, index, mode, self=self: self._on_color_changed())
        if can_be_disabled:
            self._var_check.trace("w", lambda name, index, mode, self=self: self._on_checkbox_changed())
        self.command = command
        self._var_color_init_val = self.var_color.get()

    def is_enabled(self):
        if self._can_be_disabled:
            return self._var_check.get() == 1
        else:
            return True

    def _on_clicked(self):
        new_color_tuple = tkinter.colorchooser.askcolor(self.var_color.get(), title="Select color")
        if len(new_color_tuple) > 1:
            new_color = new_color_tuple[1]
            if new_color is not None and new_color != self.var_color.get():
                self.var_color.set(new_color)
                if self.command is not None:
                    self.command(new_color)
                if self._can_be_disabled:
                    if self._var_check.get() == 0:
                        self._var_check.set(1)

    def _on_color_changed(self):
        self.btn["bg"] = self.var_color.get()
        self.btn["fg"] = self.var_color.get()

    def _on_checkbox_changed(self):
        if self.is_enabled() is False:
            # Reset to default.
            self.var_color.set(self._var_color_init_val)