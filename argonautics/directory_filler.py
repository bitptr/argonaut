from gi.repository import Gio

class DirectoryFiller:
  def __init__(self, storage_factory, directory):
    self._storage_factory = storage_factory
    self._directory = directory
    self._directory_store = None

  def get_store(self):
    self._directory_store = self._storage_factory(str,Gio.File)
    self._fill_store()
    return self._directory_store

  def refill_store(self):
    if self._directory_store is not None:
      self._directory_store.clear()
      self._fill_store()

  def _fill_store(self):
    child_enumerator = self._directory.enumerate_children("standard::*",
        Gio.FileQueryInfoFlags.NONE, None)
    while True:
      file_info = child_enumerator.next_file(None)
      if file_info == None:
        break

      self._directory_store.append([file_info.get_display_name(),
        self._directory.get_child(file_info.get_name())])

    child_enumerator.close(None)
