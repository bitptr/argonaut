from gi.repository import Gtk

class DeletionPrompter:
  def __init__(self, selected_files_iterator, file_deleter_factory):
    self._selected_files_iterator = selected_files_iterator
    self._file_deleter = file_deleter_factory(selected_files_iterator)

  def run(self, _):
    dialog = Gtk.MessageDialog(message_type=Gtk.MessageType.WARNING)
    dialog.add_buttons(Gtk.STOCK_CANCEL, Gtk.ResponseType.CLOSE,
                       Gtk.STOCK_DELETE, Gtk.ResponseType.ACCEPT)
    dialog.set_property("text", self._primary_text())
    dialog.set_property("secondary-text",
        "If you delete an item, it will be permanently lost.")
    result = dialog.run()
    dialog.destroy()

    if result == Gtk.ResponseType.ACCEPT:
      self._file_deleter.run()

  def _primary_text(self):
    number_of_files = self._selected_files_iterator.len()
    if number_of_files > 1:
      return ("Are you sure you want to permanently delete the %d selected items?" % number_of_files)
    else:
      only_file = iter(self._selected_files_iterator).next()
      return ('Are you sure you want to permanently delete "%s"?' % only_file.get_basename())
