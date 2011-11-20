from gi.repository import Gio

class FileManager:
  def __init__(self):
    "nothing yet"

  def open_files(self, icon_view):
    def open_files_aux(file_open_menu_item):
      tree_model = icon_view.get_model()
      selected_files = icon_view.get_selected_items()
      for selected_file in selected_files:
        tree_iter = tree_model.get_iter(selected_file)
        (file,) = tree_model.get(tree_iter, 1)
        Gio.app_info_launch_default_for_uri(file.get_uri(), None)
    return open_files_aux
