from gi.repository import Gtk

class UI:
  def __init__(self, builder, file_manager_factory, file_selection_factory):
    builder.add_from_file("argonaut.ui")

    self.directory_window = builder.get_object("directory-window")
    self.file_icons = builder.get_object("file-icons")
    self.directory_close_menu_item = builder.get_object("directory-close-menu-item")
    self.file_open_menu_item = builder.get_object("file-open-menu-item")
    self.file_uncompress_menu_item = builder.get_object("file-uncompress-menu-item")
    self.file_rename_menu_item = builder.get_object("file-rename-menu-item")
    self.file_copy_menu_item = builder.get_object("file-copy-menu-item")
    self.file_link_menu_item = builder.get_object("file-link-menu-item")
    self.file_properties_menu_item = builder.get_object("file-properties-menu-item")
    self.file_delete_menu_item = builder.get_object("file-delete-menu-item")
    self.edit_cut_menu_item = builder.get_object("edit-cut-menu-item")

    self._file_manager = file_manager_factory(self.file_icons)
    self._file_selection = file_selection_factory([
        self.file_open_menu_item,
        self.file_uncompress_menu_item,
        self.file_rename_menu_item,
        self.file_copy_menu_item,
        self.file_link_menu_item,
        self.file_properties_menu_item,
        self.file_delete_menu_item,
        self.edit_cut_menu_item])

  def setup_signals(self):
    self.directory_window.connect("delete-event", Gtk.main_quit)
    self.directory_close_menu_item.connect("activate", Gtk.main_quit)
    self.file_icons.connect("selection-changed",
        self._file_selection.change_sensitivity)
    self.file_open_menu_item.connect("activate",
        self._file_manager.open_files)

  def setup_file_icons(self, directory_store):
    self.file_icons.set_model(directory_store)
    self.file_icons.set_text_column(0)

  def desensitize_file_menu(self):
    self._file_selection.set_sensitivity_to(False)
