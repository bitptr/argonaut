class SelectedFilesIterator:
  def __init__(self, icon_view):
    self._icon_view = icon_view

  def __iter__(self):
    tree_model = self._icon_view.get_model()
    selected_files = self._icon_view.get_selected_items()
    for selected_file in selected_files:
      tree_iter = tree_model.get_iter(selected_file)
      (file,) = tree_model.get(tree_iter, 1)
      yield file
