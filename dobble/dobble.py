#!/usr/bin/python

from modules.utils import enum_enhanced
from modules.utils import tk_util
from modules.utils import tk_informative_int_label
from modules.utils import tk_frame_disableable
from modules.indices import indices, indices_calculator
from modules.indices import indices_generator
from modules.images import canvas, canvas_params, card
from modules.config import config, config_category
from pydoc import locate
from pathlib import Path
from PIL import Image, ImageTk
import sys
import os
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.colorchooser
import glob
import shutil


class debug_mode(enum_enhanced):
    NO_DEBUG = 0,
    ONLY_GENERATOR_ELEMS_PER_SERIE = 1,
    ONLY_GENERATOR_NUM_SERIES = 2,
    ONLY_DISTRIBUTOR = 3


class output_mode(enum_enhanced):
    PNG = 0,
    PDF = 1,
    PNG_PDF = 2


# TODO Don't want to have constants in final version.
class constants:
    image_size = 512


# Contains all original images, to avoid re-loading them by different system.
# To make any change in an image, we should copy it - do not modify these.
class card_depot:
    def __init__(self):
        self._cards = []

    def load(self, image_file_paths:list):
        if self.get_num() > 0:
            self._cards.clear()
        for file_path in image_file_paths:
            self._cards.append(card(file_path))

    def get_num(self) -> int:
        return len(self._cards)

    def obtain_image(self, index:int) -> card:
        return self._cards[index].create_copy()

    def obtain_all_images(self) -> list:
        copies = []
        for img in self._cards:
            copies.append(img.create_copy())
        return copies


# Generates dobble indices and builds dobble cards based on them. Outputs png files.
class builder:
    def __init__(self, img_db:card_depot, data_dict:dict):
        self._img_db = img_db
        self._data_dict = data_dict
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

    def build(self, mode:output_mode, name:str, is_clear:bool, max_num_shuffles:int) -> bool:
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

        images = self._create_images(idx)

        should_pdf = mode == output_mode.PDF or mode == output_mode.PNG_PDF
        should_png = mode == output_mode.PNG or mode == output_mode.PNG_PDF
        is_success = True
        if should_pdf:
            is_success = is_success and self._build_pdf(name, is_clear, images)
        if should_png:
            is_success = is_success and self._build_png(name, is_clear, images)

        return is_success

    # Returns a list of PIL Images, ready to be further saved.
    def _create_images(self, idx:indices) -> list:
        # Create canvas params, as they're common to all canvases.
        params = canvas_params(self._data_dict)
        images = []
        current_ordeal = 1
        for index_set in idx.sets:
            # Build image list from indices in this set.
            cards = []
            for index in index_set:
                cards.append(self._img_db.obtain_image(index))
            # Create a new canvas with proper params.
            canv = canvas(params, constants.image_size, current_ordeal)
            canv.distribute_cards(cards)        
            canv.apply_cards(cards)
            # We have an image ready to go.
            images.append(canv.image)
            # Increment ordeal counter.
            current_ordeal = current_ordeal + 1
        return images

    # images -> list of PIL images.
    def _build_png(self, name:str, is_clear:bool, images:list) -> bool:
        ext_name = ".png"
        ext_type = "PNG"
        if is_clear:
            shutil.rmtree(name)
        os.makedirs(name, exist_ok=True)
        image_name_format = name.split(os.path.sep)[-1] + "_{}" + ext_name
        ordeal = 1
        result = True
        for image in images:
            this_path = os.path.join(name, image_name_format.format(ordeal))
            try:
                image.save(this_path, ext_type)
            except IOError as e:
                print("Error saving", ordeal, "th card:", e)
                result = False
            ordeal = ordeal + 1
        return result

    # images -> list of PIL images.
    def _build_pdf(self, name:str, is_clear:bool, images:list) -> bool:
        name_pdf = name + ".pdf"
        if is_clear and os.path.isfile(name_pdf):
            os.remove(name_pdf)
        dir_name = os.path.dirname(name)
        os.makedirs(dir_name, exist_ok=True)

        if len(images) == 0:
            return False
        else:
            # This is so damn wasteful but as for now I don't have a better solution.
            ref_image = images[0]
            dummy_white_image = Image.new(ref_image.mode, ref_image.size, (255, 255, 255, 255))
            rgb_images = []
            for img in images:
                converted = dummy_white_image.copy()
                converted.paste(img, mask=img)
                converted = converted.convert("RGB")
                rgb_images.append(converted)

            if len(rgb_images) == 1:
                try:
                    rgb_images[0].save(name_pdf)
                except IOError as e:
                    print("Error saving to PDF.", e)
            else:
                first_img = rgb_images.pop(0)
                try:
                    first_img.save(name_pdf, save_all=True, append_images=rgb_images)
                except IOError as e:
                    print("Error saving to PDF.", e)

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
        self._callback_name = self.tk_var.trace_add("write", self._on_variable_changed)
        self._trace_callback = None
        self._trace_silent_flag = False

    def __del__(self):
        self.tk_var.trace_remove("write", self._callback_name)

    def trace_add(self, callback:callable):
        assert(self._trace_callback is None)    # Can register only one callback per wrapper!
        self._trace_callback = callback

    def trace_remove(self, object, callback:callable):
        self._trace_callback = None

    def set_var_silent(self, new_val, max_val=None):
        if self.tk_var.get() != new_val:
            # TODO This is so shitty...
            self._trace_silent_flag = True
            self.tk_var.set(new_val)
            if max_val is not None:
                self.tk_widget.configure(to=max_val)
            self._trace_silent_flag = False

    def _on_variable_changed(self, p0:str, p1:str, p2:str):
        if self._trace_silent_flag:
            return
        try:
            new_value = self.tk_var.get()
        except tkinter.TclError as e:
            return
        old_value = self._data[self._key]
        if type(new_value) != type(old_value):
            return
        if new_value == old_value:
            return
        self._data[self._key] = new_value
        if self._trace_callback is not None:
            self._trace_callback(self)


class tk_control_set(tk_frame_disableable):
    def __init__(self, data:dict, num_cols:int, *args, **kwargs):
        self.data:dict = data
        self._num_cols = max(num_cols, 1)
        self._vars = dict() # It will be filled by build_controls.
        super().__init__(*args, **kwargs)
        self._build_controls()
        self._trace_callback = None

    def __del__(self):
        for value in self._vars.values():
            value.trace_remove(self._on_any_variable_changed)
        self._num_cols = 1
        self._vars.clear()
        self._trace_callback = None

    def get_var(self, key) -> tk_dict_var_wrapper:
        return self._vars[key]

    def update_vars(self):
        # TODO
        assert(False)

    def trace_add(self, callback:callable):
        assert(self._trace_callback is None)    # Can register only one callback per wrapper!
        self._trace_callback = callback

    def trace_remove(self, callback:callable):
        self._trace_callback = None

    def _build_controls(self):
        coord_x = 0
        coord_y = 0
        col_span = 2
        num_items = len(self.data)
        num_items_per_col = int(num_items / self._num_cols)

        for key, value in self.data.items():
            control, var = self._create_control_and_var_for_type(key, value, coord_x, coord_y)
            assert(key not in self._vars)   # Crash on duplicates.
            wrapper = tk_dict_var_wrapper(var, control, self.data, key)
            wrapper.trace_add(self._on_any_variable_changed)
            self._vars[key] = wrapper

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
            variable = None
            # Proceed as usual number.
            min_val = 0
            max_val = 99
            divisor = 1
            if data_type is float:
                min_val = 0.0
                max_val = 5.0
                divisor = 0.01
                variable = tkinter.DoubleVar(self, value=data)
            else:
                variable = tkinter.IntVar(self, value=data)
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

    def _on_any_variable_changed(self, var:tk_dict_var_wrapper):
        if self._trace_callback is not None:
            self._trace_callback(self)


class tk_canvas_viewer(tkinter.Canvas):
    def __init__(self, image_db:card_depot, control_set:tk_control_set, *args, **kwargs):
        self._image_db = image_db
        # TODO We should auto-refresh when control set changes!
        self._control_set = control_set
        self._control_set.trace_add(self._on_control_set_variable_changed)
        self._canv_image_tk = None
        self._last_indices = None
        self._last_image_num = 0
        self.is_shuffle_on_refresh = False
        super().__init__(*args, **kwargs)

    def __del__(self):
        if self._control_set is not None:
            self._control_set.trace_remove(self._on_control_set_variable_changed)

    def refresh(self):
        idc = self._get_indices()

        # Visualize only first card.
        images = []
        for index in idc.sets[0]:
            images.append(self._image_db.obtain_image(index))
        
        canv_size = min(self.winfo_width(), self.winfo_height())
        canv_ordeal = 1
        params = canvas_params(self._control_set.data)
        canv = canvas(params, canv_size, canv_ordeal)
        canv.distribute_cards(images)        
        canv.apply_cards(images)

        # Copy to canvas.
        self._canv_image_tk = ImageTk.PhotoImage(canv.image)
        self.create_image(0, 0, image=self._canv_image_tk, anchor=tkinter.NW)

    def _get_indices(self) -> indices:
        image_num = self._image_db.get_num()
        if self._last_indices is None or self.is_shuffle_on_refresh or self._last_image_num != image_num:
            calculator = indices_calculator()
            calculator.calculate_for_num_series(self._image_db.get_num())
            generator = indices_generator(calculator, 1)
            self._last_indices = generator.generate()
            self._last_image_num = image_num
        return self._last_indices


    def _on_control_set_variable_changed(self, control_set:tk_control_set):
        self.refresh()


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
        self._img_db = card_depot()
        self._builder = builder(self._img_db, self._conf.get_category_data(config_category.visual))

        # Control build.
        self._build_top_menu()
        self._build_controls()
        tk_util.window_set_on_cursor(self, (-self._width / 2, -self._height / 2))

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
        self._var_output_mode = self._w_frame_options.get_var("enum_output_mode")
        self._hack_is_var_num_cards_set_externally = False
        self._w_frame_options.disable()

        visual_data = self._conf.get_category_data(config_category.visual)
        self._w_frame_canvas_control_set = tk_control_set(data=visual_data, num_cols=2, master=self, pady=6)
        tk_util.place_in_grid(self._w_frame_canvas_control_set, 0, 6, w=2)
        self._w_frame_canvas_control_set.disable()

        self._w_frame_viewer = tk_frame_disableable(self, padx=0, pady=0)
        tk_util.place_in_grid(self._w_frame_viewer, 2, 0, h=7, orientation=tkinter.N)
        self._w_canvas_viewer = tk_canvas_viewer(image_db=self._img_db, control_set=self._w_frame_canvas_control_set,
                                                 master=self._w_frame_viewer, bg="gray", width=constants.image_size, height=constants.image_size)
        tk_util.place_in_grid(self._w_canvas_viewer, 0, 0)

        self._w_frame_viewer_controls = tk_frame_disableable(self._w_frame_viewer, padx=2, pady=2)
        tk_util.place_in_grid(self._w_frame_viewer_controls, 0, 1)
        self._var_is_randomize_images_on_refresh = tkinter.IntVar(self, value=int(self._conf.get_category_data(config_category.viewer)["is_randomize_on_refresh"]))
        self._var_is_randomize_images_on_refresh.trace_add("write", self._on_shuffle_on_refresh_changed)
        self._w_cb_randomize_on_refresh = tk_util.w_create_pair_checkbox(self._w_frame_viewer_controls, "Shuffle on refresh", self._var_is_randomize_images_on_refresh, 0, 0)
        self._w_btn_refresh_viewer = tkinter.Button(self._w_frame_viewer_controls, text="Refresh", command=self._on_refresh_viewer_clicked, width=main_button_width)
        tk_util.place_in_grid(self._w_btn_refresh_viewer, 2, 0, w=2)

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

    def _on_shuffle_on_refresh_changed(self, p0:str, p1:str, p2:str):
        current_val = self._conf.get_category_data(config_category.viewer)["is_randomize_on_refresh"]
        new_val = bool(self._var_is_randomize_images_on_refresh.get())
        if current_val != new_val:
            self._conf.get_category_data(config_category.viewer)["is_randomize_on_refresh"] = new_val
            self._w_canvas_viewer.is_shuffle_on_refresh = new_val

    def _on_refresh_viewer_clicked(self):
        if self._img_db.get_num() > 0:
            self._w_canvas_viewer.refresh()

    def _on_num_images_changed(self, p0:str, p1:str, p2:str):
        if self._hack_is_var_num_cards_set_externally:
            return
        self._builder.set_num_cards(self._var_num_cards.tk_var.get())
        self._w_lbl_numbers.update(None, self._builder.get_num_cards(), self._builder.get_num_images_per_card())
        self._conf.get_category_data(config_category.generator)["recent_num_cards"] = self._var_num_cards.tk_var.get()

    def _on_generate_clicked(self):
        ret_val = self._builder.build(output_mode[output_mode.reconvert_name(self._var_output_mode.tk_var.get())], self._get_output_name(), self._var_is_clear.tk_var.get(), self._var_max_shuffles.tk_var.get())
        if ret_val:
            tkinter.messagebox.showinfo("Success", "Success.")
        else:
            tkinter.messagebox.showerror("Failure", "Failure. Please see the console output.")

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
        self._var_num_cards.set_var_silent(self._builder.get_num_cards(), self._builder.get_num_cards())
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
        images.append(card(files_to_open[index]))
    
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
    canv.distribute_cards(images)

    for img in images:
        print("Image: Size:", img.size, "Position:", img.position, "Rotation:", img.rotation, "Scale:", img.scale)
    
    canv.apply_cards(images)

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