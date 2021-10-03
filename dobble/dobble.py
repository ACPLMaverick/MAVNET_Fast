#!/usr/bin/python

from modules.utils import enum_enhanced
from modules.utils import tk_util
from modules.utils import tk_informative_int_label
from modules.utils import tk_frame_disableable
from modules.indices import indices, indices_calculator
from modules.indices import indices_generator
from modules.images import canvas, canvas_params, image
from modules.config import config, config_category
from pydoc import locate
from pathlib import Path
from PIL import ImageTk
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


class output_mode(enum_enhanced):
    PNG = 0,
    PDF = 1,
    PNG_PDF = 2


# Contains all original images, to avoid re-loading them by different system.
# To make any change in an image, we should copy it - do not modify these.
class image_depot:
    def __init__(self):
        self._images = []

    def load(self, image_file_paths:list):
        if self.get_num() > 0:
            self._images.clear()
        for file_path in image_file_paths:
            self._images.append(image(file_path))

    def get_num(self) -> int:
        return len(self._images)

    def obtain_image(self, index:int) -> image:
        return self._images[index].create_copy()

    def obtain_all_images(self) -> list:
        copies = []
        for img in self._images:
            copies.append(img.create_copy())
        return copies


# Generates dobble indices and builds dobble cards based on them. Outputs png files.
class builder:
    def __init__(self, img_db:image_depot):
        self._img_db = img_db
        self._calculator = indices_calculator()
        self._generator = indices_generator()
        self._num_cards_to_cut_away = 0

    def image_db_changed(self):
        self._calculator.calculate_for_num_series(self.get_num_total_images())

    def get_num_total_images(self):
        return self._img_db.get_num()

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
        return self.get_num_total_images() > 0 and self.get_num_cards() > 0 and self.get_num_images_per_card() > 0

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
        self.tk_var = tk_var
        self.tk_widget = tk_widget
        self._data = data
        self._key = key
        # TODO register for variable changes.


class tk_control_set(tk_frame_disableable):
    def __init__(self, data:dict, num_cols:int, *args, **kwargs):
        self.data:dict = data
        self._num_cols = max(num_cols, 1)
        self._vars = dict() # It will be filled by build_controls.
        super().__init__(*args, **kwargs)
        self._build_controls()

    def get_var(self, key) -> tk_dict_var_wrapper:
        return self._vars[key]

    def update_vars(self):
        # TODO
        pass

    def _build_controls(self):
        coord_x = 0
        coord_y = 0
        col_span = 2
        num_items = len(self.data)
        num_items_per_col = int(num_items / self._num_cols)

        for key, value in self.data.items():
            control, var = self._create_control_and_var_for_type(key, value, coord_x, coord_y)
            assert(key not in self._vars)   # Crash on duplicates.
            self._vars[key] = tk_dict_var_wrapper(var, control, self.data, key)
            # TODO Register for tk_dict_var_wrapper change to be able to emit our own event.

            coord_y = coord_y + 1
            if coord_y >= num_items_per_col:
                coord_y = 0
                coord_x = coord_x + col_span
            
    def _create_control_and_var_for_type(self, key:str, data, coord_x:int, coord_y:int):
        control = None
        variable = None
        data_type = type(data)
        label = self._make_label(key)

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
            elif "enum" in label.lower():
                enum_name = key.split("enum_")[-1]
                prefix = Path(__file__).stem + "."
                enum_type = locate(prefix + enum_name)
                values = enum_type.get_names(enum_type)
                control = tk_util.w_create_pair_combobox(self, self._make_label(enum_name), values, variable, coord_x, coord_y)
            else:
                control = tk_util.w_create_pair_text_edit(self, label, variable, coord_x, coord_y)
        elif data_type is bool:
            variable = tkinter.BooleanVar(self, value=data)
            control = tk_util.w_create_pair_checkbox(self, label, variable, coord_x, coord_y)
        else:
            print("ERROR: Not supported value type in config.")
            assert(False)
        return control, variable

    def _make_label(self, key):
        return key.replace("_", " ").capitalize() + ":"


class tk_canvas_viewer(tkinter.Canvas):
    def __init__(self, image_db:image_depot, control_set:tk_control_set, *args, **kwargs):
        self._image_db = image_db
        # TODO We should auto-refresh when control set changes!
        self._control_set = control_set
        self._canv_image_tk = None
        super().__init__(*args, **kwargs)

    def refresh(self):
        calculator = indices_calculator()
        calculator.calculate_for_num_series(self._image_db.get_num())
        generator = indices_generator(calculator, 1)
        idc = generator.generate()

        # Visualize only first card.
        images = []
        for index in idc.sets[0]:
            images.append(self._image_db.obtain_image(index))
        
        canv_size = min(self.winfo_width(), self.winfo_height())
        canv_ordeal = 1
        params = canvas_params(self._control_set.data)
        canv = canvas(params, canv_size, canv_ordeal)
        canv.distribute_images(images)        
        canv.apply_images(images)

        # Copy to canvas.
        self._canv_image_tk = ImageTk.PhotoImage(canv.image)
        self.create_image(0, 0, image=self._canv_image_tk, anchor=tkinter.NW)


class app(tkinter.Tk):
    def __init__(self):
        super().__init__()

        # Pre-setup.
        self._width = 960
        self._height = 680

        self.title("Dobble Generator")
        self.resizable(False, False)
        self.geometry("{}x{}".format(self._width, self._height))
        tk_util.bind_enter_esc(self, None, self._conditional_exit)
        self.protocol("WM_DELETE_WINDOW", self._conditional_exit)

        # Config load.
        self._conf = config()

        # Subcomponents creation.
        self._img_db = image_depot()
        self._builder = builder(self._img_db)

        # Control build.
        self._build_top_menu()
        self._build_controls()
        tk_util.window_set_on_cursor(self)

    ### Widget builds

    def _build_top_menu(self):
        # TODO Support these key commands.

        menubar = tkinter.Menu(self)  
        file = tkinter.Menu(menubar, tearoff=0)  
        file.add_command(label="New config (CTRL+N)", command=self._on_new_config_clicked)
        file.add_command(label="Load config (CTRL+O)", command=self._on_load_config_clicked)
        file.add_command(label="Save config (CTRL+S)", command=self._on_save_config_clicked)
        file.add_command(label="Save config as... (CTRL+SHIFT+S)", command=self._on_save_config_as_clicked)
        file.add_command(label="Restore config (CTRL+R)", command=self._on_restore_config_clicked)
        file.add_separator()  
        file.add_command(label="Open image files (CTRL+F)", command=self._on_select_files_clicked)
        file.add_command(label="Open image directory (CTRL+D)", command=self._on_select_dir_clicked)
        file.add_separator()
        file.add_command(label="Exit (Esc)", command=self._conditional_exit)
        menubar.add_cascade(label="File", menu=file)

        op = tkinter.Menu(menubar, tearoff=0)  
        op.add_command(label="Refresh (F5)", command=self._on_refresh_viewer_clicked)
        op.add_command(label="Generate (CTRL+G)", command=self._on_generate_clicked)
        menubar.add_cascade(label="Operation", menu=op)

        help = tkinter.Menu(menubar, tearoff=0)
        help.add_command(label="User manual")
        help.add_command(label="About")
        menubar.add_cascade(label="Help", menu=help)
        
        self.config(menu=menubar)

    def _build_controls(self):
        self._w_lbl_tip = self._create_tip()
        tk_util.place_in_grid(self._w_lbl_tip, 0, 0, w=2)

        self._w_lbl_num_files = tk_informative_int_label(self, "Selected {} images.", 0)
        tk_util.place_in_grid(self._w_lbl_num_files, 0, 1, w=2)

        self._w_lbl_numbers = tk_informative_int_label(self, "Result: {} cards with {} images on each.", 0, 0)
        tk_util.place_in_grid(self._w_lbl_numbers, 0, 2, w=2)
        self._w_lbl_warning = tk_informative_int_label(self, "", 0)
        tk_util.place_in_grid(self._w_lbl_warning, 0, 3, w=2)

        main_button_width = 25

        self._w_btn_select = tkinter.Button(self, text="Select files", command=self._on_select_files_clicked, width=int(main_button_width / 2))
        tk_util.place_in_grid(self._w_btn_select, 0, 4, orientation=tkinter.E)

        self._w_btn_select = tkinter.Button(self, text="Select dir", command=self._on_select_dir_clicked, width=int(main_button_width / 2))
        tk_util.place_in_grid(self._w_btn_select, 1, 4, orientation=tkinter.W)

        self._w_frame_options = tk_control_set(data=self._conf.get_category_data(config_category.generator), num_cols=1, master=self, pady=6)
        tk_util.place_in_grid(self._w_frame_options, 0, 5, w=2)
        self._var_edit_name = self._w_frame_options.get_var("name")
        self._var_is_clear = self._w_frame_options.get_var("is_clear")
        self._var_num_cards = self._w_frame_options.get_var("num_cards")
        self._var_max_shuffles = self._w_frame_options.get_var("max_shuffles")
        self._hack_is_var_num_cards_set_externally = False
        self._w_frame_options.disable()

        visual_data = self._conf.get_category_data(config_category.visual)
        self._w_frame_canvas_control_set = tk_control_set(data=visual_data, num_cols=2, master=self, pady=6)
        tk_util.place_in_grid(self._w_frame_canvas_control_set, 0, 6, w=2)
        self._w_frame_canvas_control_set.disable()

        self._w_frame_viewer = tk_frame_disableable(self, padx=0, pady=0)
        tk_util.place_in_grid(self._w_frame_viewer, 2, 0, h=7, orientation=tkinter.N)
        viewer_size = 512
        self._w_canvas_viewer = tk_canvas_viewer(image_db=self._img_db, control_set=self._w_frame_canvas_control_set,
                                                 master=self._w_frame_viewer, bg="gray", width=viewer_size, height=viewer_size)
        tk_util.place_in_grid(self._w_canvas_viewer, 0, 0)

        self._w_btn_refresh_viewer = tkinter.Button(self._w_frame_viewer, text="Refresh", command=self._on_refresh_viewer_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_refresh_viewer, 0, 1)

        self._w_frame_viewer.disable()

        self._w_btn_generate = tkinter.Button(self, text="Generate", command=self._on_generate_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_generate, 0, 7, w=4, weight_h=10)
        self._w_btn_generate.configure(state=tkinter.DISABLED)

    ### Button actions

    def _on_new_config_clicked(self):
        if self._are_you_sure("create a new config"):
            self._conf.restore_default(tkinter.filedialog.asksaveasfilename(confirmoverwrite=True, filetypes=[("Config files", "*.json")]))
            self._on_config_changed()

    def _on_load_config_clicked(self):
        file_to_open = tkinter.filedialog.askopenfile(filetypes=[("Config files", "*.json")])
        if file_to_open is not None:
            self._conf = config(file_to_open)
            self._on_config_changed()

    def _on_save_config_clicked(self):
        self._conf.save()

    def _on_save_config_as_clicked(self):
        file_to_create = tkinter.filedialog.asksaveasfilename(confirmoverwrite=True, filetypes=[("Config files", "*.json")])
        self._conf.save(file_to_create)

    def _on_restore_config_clicked(self):
        if self._are_you_sure("restore current config to saved version"):
            self._conf.reset()
            self._on_config_changed()

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
            self._img_db.load(files_to_open)
            self._builder.image_db_changed()
            self._w_lbl_num_files.update(None, self._builder.get_num_total_images())
            self._update_num_cards()
            self._w_canvas_viewer.refresh()

    def _on_refresh_viewer_clicked(self):
        if self._img_db.get_num() > 0:
            self._w_canvas_viewer.refresh()

    def _conditional_exit(self):
        if self._are_you_sure("exit"):
            self.destroy()

    ### Button action helper functions.

    def _are_you_sure(self, what:str):
        return tkinter.messagebox.askyesno("Are you sure?", "Are you sure you want to {}?".format(what))

    def _on_config_changed(self):
        # TODO
        pass

    def _update_num_cards(self):
        self._w_lbl_numbers.update(None, self._builder.get_num_cards(), self._builder.get_num_images_per_card())

        self._hack_is_var_num_cards_set_externally = True
        self._var_num_cards.tk_var.set(self._builder.get_num_cards())
        self._var_num_cards.tk_widget.configure(to=self._builder.get_num_cards())
        self._hack_is_var_num_cards_set_externally = False

        if self._is_warning():
            self._w_lbl_warning.update("!!! Number of cards will be truncated ({}) !!!",
                                       self._builder.get_num_cards())
        else:
            self._w_lbl_warning.update("")
        if self._builder.is_ready():
            self._w_frame_options.enable()
            self._w_frame_viewer.enable()
            self._w_frame_canvas_control_set.enable()
            self._w_btn_generate.configure(state=tkinter.NORMAL)
        else:
            self._w_frame_options.disable()
            self._w_frame_viewer.disable()
            self._w_frame_canvas_control_set.disable()
            self._w_btn_generate.configure(state=tkinter.DISABLED)

    def _on_num_images_changed(self, p0:str, p1:str, p2:str):
        if self._hack_is_var_num_cards_set_externally:
            return
        self._builder.set_num_cards(self._var_num_cards.tk_var.get())
        self._w_lbl_numbers.update(None, self._builder.get_num_cards(), self._builder.get_num_images_per_card())
        self._conf.get_category_data(config_category.generator)["recent_num_cards"] = self._var_num_cards.tk_var.get()

    def _on_generate_clicked(self):
        ret_val = self._builder.build(self._get_output_name(), self._var_is_clear.tk_var.get(), self._var_max_shuffles.tk_var.get())
        if ret_val:
            tkinter.messagebox.showinfo("Success", "Success.")
        else:
            tkinter.messagebox.showerror("Failure", "Failure. Please see the console output.")

    ### Helper functions.

    def _is_warning(self):
        return self._builder.is_wanted_num_satisfied() is False

    def _is_generation_possible(self):
        # TODO
        return False

    def _get_output_name(self):
        base = os.path.join("dobble_generated", "{}")
        name_val = self._var_edit_name.tk_var.get()
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
    canv.image.show()  # Accessing protected member only for debugging.


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