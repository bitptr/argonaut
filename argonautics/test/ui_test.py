from argonautics.ui import UI
from argonautics.test.support.mock_builder import *
from argonautics.test.support.mock_file_manager import *
import unittest

class TestUI(unittest.TestCase):
  def testBuilderUsesTheAppropriateFile(self):
    mock_builder = MockBuilder(self)
    mock_file_manager = MockFileManager()
    ui = UI(mock_builder, mock_file_manager)
    self.assertDidAddFromFile(mock_builder, "argonaut.ui")

  def testDataAttributesPulledFromBuilder(self):
    mock_builder = MockBuilder(self)
    mock_file_manager = MockFileManager()
    ui = UI(mock_builder, mock_file_manager)
    widgets = ["directory-window",
        "file-icons",
        "directory-close-menu-item",
        "file-open-menu-item",
        "file-uncompress-menu-item",
        "file-rename-menu-item",
        "file-copy-menu-item",
        "file-link-menu-item",
        "file-properties-menu-item",
        "file-delete-menu-item",
        "edit-cut-menu-item"]
    widget_accessors = [ "directory_window",
        "file_icons",
        "directory_close_menu_item",
        "file_open_menu_item",
        "file_uncompress_menu_item",
        "file_rename_menu_item",
        "file_copy_menu_item",
        "file_link_menu_item",
        "file_properties_menu_item",
        "file_delete_menu_item",
        "edit_cut_menu_item"]

    self.assertSubList(widget_accessors, dir(ui))
    for widget in widgets:
      mock_builder.assertDidGetObject(widget)


  def assertDidAddFromFile(self, builder, filename):
    self.assertTrue(builder.did_add_from_file(filename),
        "did not add the UI file to the builder")

  def assertSubList(self, smaller, larger):
    for element in smaller:
      self.assertTrue(element in larger,
          "failed to find %s in %s" % (element, larger))

if __name__ == "__main__":
  unittest.main()
