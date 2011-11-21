from gi.repository import Gio

class DirectoryFiller:
  def __init__(self, storage_factory, directory):
    self._storage_factory = storage_factory
    self._directory = directory

  def get_store(self):
    directory_store = self._storage_factory(str,Gio.File)

    child_enumerator = self._directory.enumerate_children("standard::*",
        Gio.FileQueryInfoFlags.NONE, None)
    while True:
      file_info = child_enumerator.next_file(None)
      if file_info == None:
        break

      directory_store.append([file_info.get_display_name(),
        self._directory.get_child(file_info.get_name())])

    child_enumerator.close(None)

    return directory_store
