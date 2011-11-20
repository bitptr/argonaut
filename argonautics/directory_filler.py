from gi.repository import Gtk, Gio

class DirectoryFiller:
  def __init__(self, directory_path):
    self.directory_path = directory_path

  def get_store(self):
    directory_store = Gtk.ListStore(str)

    directory = Gio.file_new_for_commandline_arg(self.directory_path)
    child_enumerator = directory.enumerate_children("standard::*", Gio.FileQueryInfoFlags.NONE, None)
    while True:
      file_info = child_enumerator.next_file(None)
      if file_info == None:
        break
      directory_store.append([file_info.get_name()])
    child_enumerator.close(None)

    return directory_store
