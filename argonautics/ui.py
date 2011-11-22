from gi.repository import Gtk

class UI:
  def __init__(self, builder, file_opener_factory, menu_sensitizer_factory):
    builder.add_from_file("argonaut.ui")

    self.directory_window = builder.get_object("directory-window")
    self._file_icons = builder.get_object("file-icons")
    self._directory_close_menu_item = builder.get_object("directory-close-menu-item")
    self._file_open_menu_item = builder.get_object("file-open-menu-item")
    self._file_uncompress_menu_item = builder.get_object("file-uncompress-menu-item")
    self._file_rename_menu_item = builder.get_object("file-rename-menu-item")
    self._file_copy_menu_item = builder.get_object("file-copy-menu-item")
    self._file_link_menu_item = builder.get_object("file-link-menu-item")
    self._file_properties_menu_item = builder.get_object("file-properties-menu-item")
    self._file_delete_menu_item = builder.get_object("file-delete-menu-item")
    self._edit_cut_menu_item = builder.get_object("edit-cut-menu-item")

    self._file_opener = file_opener_factory(self._file_icons)
    self._menu_sensitizer = menu_sensitizer_factory([
        self._file_open_menu_item,
        self._file_uncompress_menu_item,
        self._file_rename_menu_item,
        self._file_copy_menu_item,
        self._file_link_menu_item,
        self._file_properties_menu_item,
        self._file_delete_menu_item,
        self._edit_cut_menu_item])

  def setup_signals(self):
    self.directory_window.connect("delete-event", Gtk.main_quit)
    self._directory_close_menu_item.connect("activate", Gtk.main_quit)
    self._file_icons.connect("selection-changed",
        self._menu_sensitizer.change_sensitivity)
    self._file_open_menu_item.connect("activate",
        self._file_opener.run)

  def setup_file_icons(self, directory_store):
    self._file_icons.set_model(directory_store)
    self._file_icons.set_text_column(0)

  def desensitize_file_menu(self):
    self._menu_sensitizer.set_sensitivity_to(False)
