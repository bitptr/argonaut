from argonautics.ui import UI
from argonautics.test.support.mock_builder import *
from argonautics.test.support.mock_file_manager import *
import unittest

class TestUI(unittest.TestCase):
  def testBuilderUsesTheAppropriateFile(self):
    mock_builder = MockBuilder()
    mock_file_manager = MockFileManager()
    ui = UI(mock_builder, mock_file_manager)
    self.assertTrue(mock_builder.did_add_from_file("argonaut.ui"))

  def testDataAttributesPulledFromBuilder(self):
    mock_builder = MockBuilder()
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

    self.assertTrue(all([accessor in dir(ui) for accessor in widget_accessors]))
    self.assertTrue(all([mock_builder.did_get_object(widget_name) for widget_name in widgets]))

if __name__ == "__main__":
  unittest.main()
