from gi.repository import Gio

class FileOpener:
  def __init__(self, icon_view):
    self._icon_view = icon_view

  def run(self, file_open_menu_item):
    tree_model = self._icon_view.get_model()
    selected_files = self._icon_view.get_selected_items()
    for selected_file in selected_files:
      tree_iter = tree_model.get_iter(selected_file)
      (file,) = tree_model.get(tree_iter, 1)
      Gio.app_info_launch_default_for_uri(file.get_uri(), None)
