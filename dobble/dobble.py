#!/usr/bin/python

from copy import Error
from math import degrees, sqrt
from modules.utils import enum_enhanced
from modules.utils import tk_util
from modules.utils import tk_informative_int_label
from modules.utils import tk_frame_disableable
from modules.indices import indices, indices_calculator
from modules.indices import indices_generator
from modules.images import canvas, canvas_params, image
from modules.config import config, config_category
import sys
import os
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.colorchooser
import glob


class debug_mode(enum_enhanced):
    NO_DEBUG = 0,
    ONLY_GENERATOR_ELEMS_PER_SERIE = 1,
    ONLY_GENERATOR_NUM_SERIES = 2,
    ONLY_DISTRIBUTOR = 3


class builder:
    def __init__(self):
        self._file_list = None
        self._calculator = indices_calculator()
        self._generator = indices_generator()
        self._num_cards_to_cut_away = 0

    def set_file_list(self, file_list):
        self._file_list = file_list
        self._calculator.calculate_for_num_series(self.get_num_total_images())

    def get_num_total_images(self):
        return len(self._file_list)

    def set_num_cards(self, new_num_cards:int):
        assert(new_num_cards <= self._get_num_cards())
        if new_num_cards == self._get_num_cards():
            return
        self._num_cards_to_cut_away = self._get_num_cards() - new_num_cards

    def get_num_cards(self):
        return self._get_num_cards() - self._num_cards_to_cut_away

    def get_num_images_per_card(self):
        return self._calculator.num_elements_per_serie

    def is_wanted_num_satisfied(self):
        return self._calculator.is_number_different_than_wanted() is False

    def is_ready(self):
        return self._file_list is not None and self.get_num_cards() > 0 and self.get_num_images_per_card() > 0

    def build(self, name, is_clear, max_num_shuffles) -> bool:
        if self.is_ready() is False:
            return False

        self._generator.set_calculator(self._calculator)
        self._generator.set_max_shuffles(max_num_shuffles)
        idx = self._generator.generate()
        if idx is None or len(idx.sets) != self._get_num_cards():
            return False
        
        # Discard requested number of sets.
        if self._num_cards_to_cut_away > 0:
            idx.discard(self._num_cards_to_cut_away)

        idx.print()

        return True

    def _get_num_cards(self):
        return self._calculator.num_series


# Makes sure changes in var are reflected in the dict itself, so we can easily save the data afterwards.
class tk_dict_var_wrapper:
    def __init__(self, tk_var:tkinter.Variable, tk_widget:tkinter.Widget, data:dict, key:str):
        self._tk_var = tk_var
        self._tk_widget = tk_widget
        self._data = data
        self._key = key
        # TODO register for variable changes.


class tk_control_set(tk_frame_disableable):
    def __init__(self, data:dict, *args, **kwargs):
        self._data:dict = data
        self._vars = dict() # It will be filled by build_controls.
        super().__init__(*args, **kwargs)
        self._build_controls()

    def _build_controls(self):
        coord_x = 0
        coord_y = 0
        col_span = 2
        num_items = len(self._data)
        num_cols = max(int(sqrt(num_items)), 1)
        num_items_per_col = int(num_items / num_cols)

        for key, value in self._data.items():
            label = key.replace("_", " ").capitalize() + ":"
            control, var = self._create_control_and_var_for_type(label, value, coord_x, coord_y)
            assert(key not in self._vars)   # Crash on duplicates.
            self._vars[key] = tk_dict_var_wrapper(var, control, self._data, key)
            coord_y = coord_y + 1
            if coord_y >= num_items_per_col:
                coord_y = 0
                coord_x = coord_x + col_span
            
    def _create_control_and_var_for_type(self, label:str, data, coord_x:int, coord_y:int):
        control = None
        variable = None
        data_type = type(data)
        if data_type is int or data_type is float:
            variable = tkinter.IntVar(self, value=data)
            # Proceed as usual number.
            min_val = 0
            max_val = 99
            divisor = 1
            if data_type is float:
                max_val = 5
                divisor = 0.01
            control = tk_util.w_create_pair_num_edit(self, label, variable, coord_x, coord_y, min_val, max_val, divisor)
        elif data_type is str:
            variable = tkinter.StringVar(self, value=data)
            # HUGE HACK !!!
            if "color" in label.lower():
                # Make color control!
                control = tk_util.w_create_pair_color_picker(self, label, variable, coord_x, coord_y, False)
            else:
                control = tk_util.w_create_pair_text_edit(self, label, variable, coord_x, coord_y)
        elif data_type is bool:
            variable = tkinter.BooleanVar(self, value=data)
            control = tk_util.w_create_pair_checkbox(self, label, variable, coord_x, coord_y)
        else:
            raise Error("Not supported value type in config.")
        return control, variable


class app(tkinter.Tk):
    def __init__(self):
        super().__init__()

        # Pre-setup.
        self._width = 800
        self._height = 600

        self.title("Dobble Generator")
        self.resizable(False, False)
        self.geometry("{}x{}".format(self._width, self._height))
        tk_util.bind_enter_esc(self, None, self._conditional_exit)
        self.protocol("WM_DELETE_WINDOW", self._conditional_exit)

        # Config load.
        self._conf = config()

        # Control build.
        self._build_controls()
        tk_util.window_set_on_cursor(self)

        self._builder = builder()

    def _build_controls(self):
        self._w_lbl_tip = self._create_tip()
        tk_util.place_in_grid(self._w_lbl_tip, 0, 0, w=2)

        self._w_lbl_num_files = tk_informative_int_label(self, "Selected {} images.", 0)
        tk_util.place_in_grid(self._w_lbl_num_files, 0, 1, w=2)

        self._w_lbl_warning = tk_informative_int_label(self, "WARNING. Selected number of images will be truncated to nearest fitting number ({}).", 0)
        tk_util.place_in_grid(self._w_lbl_warning, 0, 2, w=2)
        self._w_lbl_warning.grid_remove()
        self._w_lbl_numbers = tk_informative_int_label(self, "Result: {} cards with {} images on each.", 0, 0)
        tk_util.place_in_grid(self._w_lbl_numbers, 0, 3, w=2)

        main_button_width = 25

        self._w_btn_select = tkinter.Button(self, text="Select files", command=self._on_select_files_clicked, width=int(main_button_width / 2))
        tk_util.place_in_grid(self._w_btn_select, 0, 4, orientation=tkinter.E)

        self._w_btn_select = tkinter.Button(self, text="Select dir", command=self._on_select_dir_clicked, width=int(main_button_width / 2))
        tk_util.place_in_grid(self._w_btn_select, 1, 4, orientation=tkinter.W)

        self._w_frame_options = tk_frame_disableable(self, pady=6)
        tk_util.place_in_grid(self._w_frame_options, 0, 5, w=2)
        self._w_edit_name = tk_util.w_create_pair_text_edit(self._w_frame_options, "Name:", "", 0, 0)
        self._w_edit_name.configure(width=16)
        self._var_is_clear = tkinter.BooleanVar(self._w_frame_options, False)
        self._w_edit_clear_dir = tk_util.w_create_pair_checkbox(self._w_frame_options, "Clear:", self._var_is_clear, 0, 1)
        self._var_num_cards = tkinter.IntVar(self._w_frame_options, 0)
        self._var_max_shuffles = tkinter.IntVar(self._w_frame_options, 3)

        self._w_edit_output_cards = tk_util.w_create_pair_num_edit(self._w_frame_options, "Num cards:", self._var_num_cards, 0, 2, max_val=999)
        self._var_num_cards.trace_add("write", self._on_num_images_changed)
        self._hack_is_var_num_cards_set_externally = False

        self._w_edit_max_shuffles = tk_util.w_create_pair_num_edit(self._w_frame_options, "Max shuffles:", self._var_max_shuffles, 0, 3, max_val=64)
        self._w_frame_options.disable()

        self._w_btn_generate = tkinter.Button(self, text="Generate", command=self._on_generate_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_generate, 0, 6, w=2)
        self._w_btn_generate.configure(state=tkinter.DISABLED)

        self._w_frame_canvas_config = tk_control_set(data=self._conf.get_category_data(config_category.visual), master=self, pady=6)
        tk_util.place_in_grid(self._w_frame_canvas_config, 0, 7, w=2)


    def _conditional_exit(self):
        if tkinter.messagebox.askyesno("Are you sure?", "Are you sure you want to exit?"):
            self.destroy()

    def _on_select_files_clicked(self):
        self._on_select_clicked(False)

    def _on_select_dir_clicked(self):
        self._on_select_clicked(True)

    def _on_select_clicked(self, is_dir_dialog:bool):
        files_to_open = None
        if is_dir_dialog:
            directory = tkinter.filedialog.askdirectory()
            files_to_open = glob.glob(os.path.join(directory, "*.jpg"))
            files_to_open.extend(glob.glob(os.path.join(directory, "*.png")))
            files_to_open.extend(glob.glob(os.path.join(directory, "*.svg")))
            files_to_open.extend(glob.glob(os.path.join(directory, "*.tga")))
            files_to_open.extend(glob.glob(os.path.join(directory, "*.gif")))
        else:
            files_to_open = tkinter.filedialog.askopenfilenames(filetypes=[("Image files", "*.jpg *.jpeg *.png *.svg *.tga *.gif")])

        if files_to_open is not None and len(files_to_open) > 0:
            self._builder.set_file_list(files_to_open)
            self._w_lbl_num_files.update(self._builder.get_num_total_images())
            self._update_num_cards()

    def _update_num_cards(self):
        self._w_lbl_numbers.update(self._builder.get_num_cards(), self._builder.get_num_images_per_card())

        self._hack_is_var_num_cards_set_externally = True
        self._var_num_cards.set(self._builder.get_num_cards())
        self._w_edit_output_cards.configure(to=self._builder.get_num_cards())
        self._hack_is_var_num_cards_set_externally = False

        if self._is_warning():
            self._w_lbl_warning.update(self._builder.get_num_cards())
            self._w_lbl_warning.grid()
        else:
            self._w_lbl_warning.grid_remove()
        if self._builder.is_ready():
            self._w_frame_options.enable()
            self._w_btn_generate.configure(state=tkinter.NORMAL)
        else:
            self._w_frame_options.disable()
            self._w_btn_generate.configure(state=tkinter.DISABLED)

    def _on_num_images_changed(self, p0:str, p1:str, p2:str):
        if self._hack_is_var_num_cards_set_externally:
            return
        self._builder.set_num_cards(self._var_num_cards.get())
        self._w_lbl_numbers.update(self._builder.get_num_cards(), self._builder.get_num_images_per_card())

    def _on_generate_clicked(self):
        ret_val = self._builder.build(self._get_output_name(), self._var_is_clear.get(), self._var_max_shuffles.get())
        if ret_val:
            tkinter.messagebox.showinfo("Success", "Success.")
        else:
            tkinter.messagebox.showerror("Failure", "Failure. Please see the console output.")

    def _is_warning(self):
        return self._builder.is_wanted_num_satisfied() is False

    def _is_generation_possible(self):
        # TODO
        return False

    def _get_output_name(self):
        base = os.path.join("dobble_generated", "{}")
        name_val = self._w_edit_name.get(1.0, tkinter.END)
        if not tk_util.check_file_name(name_val):
            name_val = "unnamed"
        return base.format(name_val)

    def _create_tip(self):
        first_prime_numbers = (2, 3, 5, 7, 11)
        tip_text = "Optimal numbers of images: "
        is_first = True
        for prime_number in first_prime_numbers:
            optimal_num = indices_calculator.compute_num_series(prime_number)
            if is_first:
                tip_text += "{}".format(optimal_num)
                is_first = False
            else:
                tip_text += ", {}".format(optimal_num)
        tip_text += "."
        return tkinter.Label(self, text=tip_text)


def get_debug():
    if len(sys.argv) < 2:
        return None
    arg = sys.argv[1]
    debug_value = debug_mode.NO_DEBUG
    try:
        debug_value = debug_mode[arg]
    except:
        print("WARNING. A debug argument was provided but it wasn't recognized:", arg)
    return debug_value


def debug_generator(debug:debug_mode):
        print("Dobble!")

        # Like in real-life dobble set. Configurable later.
        num_elements_per_serie = 8
        num_total_elements = 27
        max_shuffles = 0

        calculator = indices_calculator()
        if debug == debug_mode.ONLY_GENERATOR_ELEMS_PER_SERIE:
            calculator.calculate_for_num_elements_per_serie(num_elements_per_serie)
        elif debug == debug_mode.ONLY_GENERATOR_NUM_SERIES:
            calculator.calculate_for_num_series(num_total_elements)

        generator = indices_generator(calculator, max_shuffles)
        idc = generator.generate()
        idc.print()

        if calculator.is_number_different_than_wanted():
            if debug == debug_mode.ONLY_GENERATOR_ELEMS_PER_SERIE:
                print("Number is different than wanted : ", calculator.wanted_num_elements_per_serie, "vs", calculator.num_elements_per_serie)
            elif debug == debug_mode.ONLY_GENERATOR_NUM_SERIES:
                print("Number is different than wanted : ", calculator.wanted_num_series, "vs", calculator.num_series)


def debug_distributor():
    print("Distribute!")
    directory = "png"
    files_to_open = glob.glob(os.path.join(directory, "*.jpg"))
    files_to_open.extend(glob.glob(os.path.join(directory, "*.png")))
    files_to_open.extend(glob.glob(os.path.join(directory, "*.svg")))
    files_to_open.extend(glob.glob(os.path.join(directory, "*.tga")))
    files_to_open.extend(glob.glob(os.path.join(directory, "*.gif")))
    num_files = len(files_to_open)
    
    if num_files <= 0:
        return

    calculator = indices_calculator()
    calculator.calculate_for_num_series(num_files)
    generator = indices_generator(calculator, 1)
    idc = generator.generate()
    idc.print()

    # Test only first card.
    images = []
    for index in idc.sets[0]:
        images.append(image(files_to_open[index]))
    
    # Dummy test params.
    params = canvas_params()
    canv_size = 512
    params.outer_ring_bias = 0.0
    params.inner_disc_bias = 0.15
    params.rotation_variation = 0.0
    params.scale_multiplier = 1.0
    params.scale_variation = 0.0
    params.position_variation_x = 0.0
    params.position_variation_y = 0.0
    params.position_multiplier = 1.0
    params.background_color = 0xFFFFFFFF
    params.outer_ring_color = 0xFF000000
    params.inner_disc_color = 0xFF000000
    params.outer_ring_width = 2
    canv_ordeal = 1
    canv = canvas(params, canv_size, canv_ordeal)
    canv.distribute_images(images)

    for img in images:
        print("Image: Size:", img.size, "Position:", img.position, "Rotation:", img.rotation, "Scale:", img.scale)
    
    canv.apply_images(images)

    out_dir = "out"
    os.makedirs(out_dir, exist_ok=True)
    # canv.save(out_dir)
    canv._image.show()  # Accessing protected member only for debugging.


def main():
    debug = get_debug()

    if debug == debug_mode.ONLY_GENERATOR_ELEMS_PER_SERIE or debug == debug_mode.ONLY_GENERATOR_NUM_SERIES:
        debug_generator(debug)
    elif debug == debug_mode.ONLY_DISTRIBUTOR:
        debug_distributor()
    else:   # Normal app flow.
        wnd = app()
        wnd.mainloop()

    return 0


if __name__ == "__main__":
    sys.exit(main())