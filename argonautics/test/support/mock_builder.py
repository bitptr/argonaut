from argonautics.test.support.mock_widget import *

class MockBuilder:
  def __init__(self, test_case):
    self._test_case = test_case
    self.added_from_file = None
    self.got_object = []

  def add_from_file(self,filename):
    self.added_from_file = filename

  def get_object(self, widget_name):
    self.got_object.append(widget_name)
    return MockWidget(self._test_case, widget_name)

  def did_add_from_file(self, filename):
    return self.added_from_file == filename

  def did_get_object(self, widget_name):
    return widget_name in self.got_object

  def assertDidGetObject(self, widget_name):
    self._test_case.assertTrue(
        self.did_get_object(widget_name),
        "did not get the widget %s" % widget_name)

  def assertDidAddFromFile(self, filename):
    self._test_case.assertTrue(
        self.did_add_from_file(filename),
        "did not add the UI file to the builder")
