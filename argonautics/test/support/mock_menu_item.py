class MockMenuItem:
  def __init__(self, test_case):
    self._test_case = test_case
    self._is_sensitive = True

  def set_sensitive(self, sensitive):
    self._is_sensitive = sensitive

  def assertSensitized(self):
    self._test_case.assertTrue(self._is_sensitive)

  def assertDesensitized(self):
    self._test_case.assertFalse(self._is_sensitive)
