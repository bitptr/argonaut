from gi.repository import Gtk

class UI:
  def __init__(self, file_manager):
    builder = Gtk.Builder()
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

    self.file_manager = file_manager

  def setup_signals(self):
    self.directory_window.connect("delete-event", Gtk.main_quit)
    self.directory_close_menu_item.connect("activate", Gtk.main_quit)
    self.file_icons.connect("selection-changed", self.change_file_menu_sensitivity)
    self.file_open_menu_item.connect("activate",
        self.file_manager.open_files(self.file_icons))

  def change_file_menu_sensitivity(self, icon_view):
    if len(icon_view.get_selected_items()) == 0:
      self.desensitize_file_menu()
    else:
      self.sensitize_file_menu()

  def setup_file_icons(self, directory_store):
    self.file_icons.set_model(directory_store)
    self.file_icons.set_text_column(0)

  def desensitize_file_menu(self):
    self.set_file_menu_sensitivity_to(False)

  def sensitize_file_menu(self):
    self.set_file_menu_sensitivity_to(True)

  def set_file_menu_sensitivity_to(self, sensitive):
    file_sensitive_menu_items = [
        self.file_open_menu_item,
        self.file_uncompress_menu_item,
        self.file_rename_menu_item,
        self.file_copy_menu_item,
        self.file_link_menu_item,
        self.file_properties_menu_item,
        self.file_delete_menu_item,
        self.edit_cut_menu_item]
    [item.set_sensitive(sensitive) for item in file_sensitive_menu_items]
