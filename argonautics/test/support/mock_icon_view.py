class MockIconView:
  def __init__(self, test_case, get_selected_items=[]):
    self._test_case = test_case
    self._get_selected_items = get_selected_items

  def get_selected_items(self):
    return self._get_selected_items
