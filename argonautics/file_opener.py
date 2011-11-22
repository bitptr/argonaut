from gi.repository import Gio

class FileOpener:
  def __init__(self, selected_files_iterator):
    self._selected_files_iterator = selected_files_iterator

  def run(self, _):
    for file in self._selected_files_iterator:
      Gio.app_info_launch_default_for_uri(file.get_uri(), None)
