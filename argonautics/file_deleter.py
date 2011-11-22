class FileDeleter:
  def __init__(self, selected_files_iterator):
    self._selected_files_iterator = selected_files_iterator

  def run(self):
    [file.delete(None) for file in self._selected_files_iterator]
