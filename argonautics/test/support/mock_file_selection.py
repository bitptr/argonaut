class MockFileSelection:
  def __init__(self, test_case):
    self._test_case = test_case
    self._sensitivity = True

  def factory(self, menu_items):
    self._menu_items = menu_items
    return self

  def change_sensitivity(self, icon_view):
    pass

  def set_sensitivity_to(self, sensitivity):
    self._sensitivity = sensitivity

  def assertSensitized(self):
    self._test_case.assertTrue(self._sensitivity)

  def assertDesensitized(self):
    self._test_case.assertFalse(self._sensitivity)
