class FileSelection:
  def __init__(self, menu_items):
    self._menu_items = menu_items

  def set_sensitivity_to(self, sensitive):
    [item.set_sensitive(sensitive) for item in self._menu_items]

  def change_sensitivity(self, icon_view):
    self.set_sensitivity_to(len(icon_view.get_selected_items()) != 0)
